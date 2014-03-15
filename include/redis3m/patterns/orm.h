#pragma once

#include <redis3m/connection.h>
#include <map>
#include <string>
#include <boost/foreach.hpp>
#include <redis3m/patterns/model.h>

namespace redis3m
{
namespace patterns
{

class orm;

class orm {
public:

    // Find
    template<typename Entity>
    bool find_by_id(connection::ptr_t conn, const std::string& id, Entity& entity)
    {
        reply r = conn->run(command("HGETALL")(entity_key<Entity>(id)));
        if (r.elements().size() > 0 )
        {
            std::map<std::string, std::string> map;
            for (unsigned long i = 0; i< r.elements(); i=i+2 )
            {
                map[r.elements()[i].str()] = r.elements()[i+1].str();
            }
            map["id"] = id;
            entity.from_map(map);
            return true;
        }
        else
        {
            return false;
        }
    }

    template<class Entity>
    bool find_by_unique_field(connection::ptr_t conn, const std::string& field, const std::string& value, Entity& entity)
    {
        std::string id = conn->run(command("HGET")
                           (unique_field_key<Entity>(field), value)).str();
        if (!id.empty())
        {
            return find_by_id(conn, id, entity);
        }
        else
        {
            return false;
        }
    }

    template<class Entity>
    bool exists_by_id(connection::ptr_t conn, const std::string& id)
    {
        return conn->run(command("SISMEMBER")(collection_key<Entity>(), id)).integer() == 1;
    }

    // Basic attribute handling
    template<class Entity>
    std::string save(connection::ptr_t conn, const Entity& entity)
    {
        std::string new_id = entity.id();
        // Create a new id if object is new, using redis INCR command
        if (new_id.empty())
        {
            uint64_t new_id_int = conn->run(command("INCR")(collection_id_key<Entity>())).integer();
            new_id = boost::lexical_cast<std::string>(new_id_int);
        }

        std::string key = entity_key<Entity>(new_id);

        std::map<std::string, std::string> serialized_entity = entity.to_map();

        std::vector<std::string> hmset_command;

        hmset_command.reserve(2+serialized_entity.size()*2);

        hmset_command.push_back("HMSET");
        hmset_command.push_back(key);

        // Flatten map and put on argv
        std::pair<std::string, std::string> item;
        BOOST_FOREACH(item, serialized_entity)
        {
            hmset_command.push_back(item.first);
            hmset_command.push_back(item.second);
        }

        std::vector<std::string> indexes = Entity::indexes();

        conn->append(command("MULTI"));
        conn->append(command("SADD")(collection_key<Entity>())
                             (new_id));
        //conn->append_command(boost::assign::list_of("DEL")(key));

        BOOST_FOREACH(std::string field, indexes)
        {
            conn->append(command("SADD")
                                 (indexed_field_key<Entity>(field, serialized_entity[field]))
                                 (new_id)
                                 );
        }
        conn->append(command("EXEC"));

        conn->get_replies(3+indexes.size());

        return new_id;
    }

    template<class Entity>
    void remove(const std::string& id)
    {
        std::string key = entity_key<Entity>(id);

        std::vector<std::string> indexes = Entity::indexes();
        std::vector<std::string> values;
        BOOST_FOREACH(std::string field, indexes)
        {
            values.push_back(hget(key, field));
        }

        redisReply* reply;
        int redisCommandRet;
        // Watch command let transaction fail if sameone concurrently change entity fields
        // REF: http://www.redis.io/commands/watch
        redisAppendCommand(c, "WATCH %s", key.c_str());
        redisAppendCommand(c, "MULTI");
        redisAppendCommand(c, "SREM %s %s", collection_key<Entity>().c_str(), id.c_str());
        for ( uint16_t i=0; i < indexes.size(); ++i)
        {
            redisAppendCommand(c, "SREM %s %s",
                               indexed_field_key<Entity>(indexes.at(i), values.at(i)).c_str(),
                               id.c_str());
        }
        redisAppendCommand(c, "DEL %s", key.c_str());
        redisAppendCommand(c, "EXEC");

        // redis will send 3 status replies for commands multi, sadd, del, hmset
        for (uint16_t i=0; i < 4 + indexes.size() ;++i)
        {
            redisCommandRet = redisGetReply(c, reinterpret_cast<void**>(&reply));
            if (redisCommandRet != REDIS_OK || reply->type != REDIS_REPLY_STATUS)
            {
                throw redis_fatal("Cannot remove resource");
            }
            freeReplyObject(reply);
        }

        redisCommandRet = redisGetReply(c, reinterpret_cast<void**>(&reply));

        if (redisCommandRet != REDIS_OK || reply->type != REDIS_REPLY_ARRAY
                || reply->elements == 0)
        {
            freeReplyObject( reply );
            throw redis_fatal("Cannot remove resource");
        }
        freeReplyObject( reply );
    }

    // Subentities
    template<class Entity>
    bool sub_entity_add(const std::string& entity_id, const std::string& collection, const std::string& subentity_id)
    {
        return sadd(subentity_collection_key<Entity>(entity_id, collection), subentity_id);
    }

    template<class Entity>
    bool sub_entity_remove(const std::string& entity_id, const std::string& collection, const std::string& subentity_id)
    {
        return srem(subentity_collection_key<Entity>(entity_id, collection), subentity_id);
    }

    template<class Entity>
    std::vector<std::string> all_subentities(const std::string& entity_id, const std::string& collection)
    {
        return smembers(subentity_collection_key<Entity>(entity_id, collection));
    }

    template<class Entity>
    bool subentity_exists_by_id(const std::string& entity_id, const std::string& collection, const std::string& subentity_id)
    {
        return sismember(subentity_collection_key<Entity>(entity_id, collection), subentity_id);
    }

    template<class Entity>
    void subentity_exists_by_id_throw(const std::string& entity_id, const std::string& collection, const std::string& subentity_id)
    {
        if (! sismember(subentity_collection_key<Entity>(entity_id, collection), subentity_id))
        {
            throw Entity_not_found(subentity_id + "is not on " + Entity::entity_name() + " " + collection);
        }
    }

    template<class Entity, class redis_subentity>
    bool has_subentity_with_indexed_field(const std::string& entity_id, const std::string& collection,
                                          const std::string& subentity_indexed_field, const std::string& subentity_indexed_field_value)
    {
        std::vector<std::string> keys_to_intersect =
                boost::assign::list_of
                (subentity_collection_key<Entity>(entity_id, collection))
                (indexed_field_key<redis_subentity>(subentity_indexed_field, subentity_indexed_field_value));
        std::vector<std::string> intersection_result = sinter(keys_to_intersect);

        return intersection_result.size() > 0;
    }

protected:

    template<class Entity>
    inline std::string collection_key()
    {
        return Entity::entity_name() + ":all";
    }

    template<class Entity>
    inline std::string collection_id_key()
    {
        return Entity::entity_name() + ":id";
    }

    template<typename Entity>
    inline std::string entity_key(const std::string& id)
    {
        return Entity::entity_name() + ":" + id;
    }

    template<class Entity>
    inline std::string subentity_collection_key(const std::string& id, const std::string& collection_name)
    {
        return entity_key<Entity>(id) + ":" + collection_name;
    }

    template<class Entity>
    inline std::string indexed_field_key(const std::string& field, const std::string& value)
    {
        return Entity::entity_name() + ":indices:" + field + ":" + value;
    }

    template<class Entity>
    inline std::string unique_field_key(const std::string& field)
    {
        return Entity::entity_name() + ":uniques:" + field;
    }
};
}
}
