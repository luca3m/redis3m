#pragma once

#include <redis3m/connection.h>
#include <map>
#include <string>
#include <algorithm>
#include <boost/foreach.hpp>
#include <redis3m/patterns/model.h>
#include <redis3m/patterns/script_exec.h>
#include <msgpack.hpp>
#include <redis3m/utils/file.h>

namespace redis3m
{
namespace patterns
{

template<typename Model>
class orm {
public:

    // Find
    Model find_by_id(connection::ptr_t conn, const std::string& id)
    {
        reply r = conn->run(command("HGETALL")(model_key(id)));
        if (r.elements().size() > 0 )
        {
            std::map<std::string, std::string> map;
            for (unsigned long i = 0; i < r.elements().size(); i+=2 )
            {
                map[r.elements()[i].str()] = r.elements()[i+1].str();
            }
            return Model(id, map);
        }
        else
        {
            return Model();
        }
    }

    Model find_by_unique_field(connection::ptr_t conn, const std::string& field, const std::string& value)
    {
        std::string id = conn->run(command("HGET")(unique_field_key(field))(value)).str();
        if (!id.empty())
        {
            return find_by_id(conn, id);
        }
        else
        {
            return Model();
        }
    }

    bool exists_by_id(connection::ptr_t conn, const std::string& id)
    {
        return conn->run(command("SISMEMBER")(collection_key(), id)).integer() == 1;
    }

    // Basic attribute handling
    std::string save(connection::ptr_t conn, const Model& model)
    {
        std::map<std::string, std::string> model_map;
        model_map["name"] = model.model_name();
        if (!model.id().empty())
        {
            model_map["id"] = model.id();
        }
        std::vector<std::string> args;
        msgpack::sbuffer sbuf;  // simple buffer

        // Pack model data
        msgpack::pack(&sbuf, model_map);
        args.push_back(std::string(sbuf.data(), sbuf.size()));
        sbuf.clear();

        // pack model attributes
        std::map<std::string, std::string> attributes = model.to_map();
        std::vector<std::string> attributes_vector;
        typedef std::pair<std::string, std::string> strpair;
        BOOST_FOREACH(const strpair& item, attributes)
        {
            attributes_vector.push_back(item.first);
            attributes_vector.push_back(item.second);
        }

        msgpack::pack(&sbuf, attributes_vector);
        args.push_back(std::string(sbuf.data(), sbuf.size()));
        sbuf.clear();

        // pack model indices
        std::vector<std::pair<std::string, std::string> > indices;
        BOOST_FOREACH(const std::string& index, model.indices())
        {
            indices.push_back(std::make_pair(index, attributes[index]));
        }
        msgpack::pack(&sbuf, indices);
        args.push_back(std::string(sbuf.data(), sbuf.size()));
        sbuf.clear();

        // pack model uniques
        std::map<std::string, std::string> uniques;
        BOOST_FOREACH(const std::string& index, model.uniques())
        {
            uniques[index] = attributes[index];
        }
        msgpack::pack(&sbuf, uniques);
        args.push_back(std::string(sbuf.data(), sbuf.size()));

        reply r = save_script.exec(conn, std::vector<std::string>(), args);

        return r.str();
    }

    void remove(connection::ptr_t conn, const Model& model)
    {
        std::map<std::string, std::string> model_map;
        model_map["name"] = model.model_name();
        model_map["id"] = model.id();
        model_map["key"] = model_key(model.id());

        std::vector<std::string> args;
        msgpack::sbuffer sbuf;  // simple buffer

        // Pack model data
        msgpack::pack(&sbuf, model_map);
        args.push_back(std::string(sbuf.data(), sbuf.size()));
        sbuf.clear();

        // pack model uniques
        std::map<std::string, std::string> attributes = model.to_map();
        std::map<std::string, std::string> uniques;
        BOOST_FOREACH(const std::string& index, model.uniques())
        {
            uniques[index] = attributes[index];
        }
        msgpack::pack(&sbuf, uniques);
        args.push_back(std::string(sbuf.data(), sbuf.size()));
        sbuf.clear();

        // TODO: support tracked keys
        msgpack::pack(&sbuf, std::vector<std::string>());
        args.push_back(std::string(sbuf.data(), sbuf.size()));
        sbuf.clear();

        remove_script.exec(conn, std::vector<std::string>(), args);
    }

    std::vector<std::string> list_members(connection::ptr_t conn, const Model& m, const std::string& list_name)
    {
        std::vector<std::string> ret;
        reply lrange = conn->run(command("LRANGE")
                            (subentry_collection_key(m.id(), list_name))
                            ("0")("-1"));
        BOOST_FOREACH(const reply& r, lrange.elements())
        {
            ret.push_back(r.str());
        }
        return ret;
    }

    void set_add(connection::ptr_t conn, const Model& m, const std::string& set_name, const std::string& entry)
    {
        conn->run(command("SADD")(subentry_collection_key(m.id(), set_name))(entry));
    }

    void set_remove(connection::ptr_t conn, const Model& m, const std::string& set_name, const std::string& entry)
    {
        conn->run(command("SREM")(subentry_collection_key(m.id(), set_name)(entry)));
    }

    std::set<std::string> set_members(connection::ptr_t conn, const Model& m, const std::string& set_name)
    {
        reply r = conn->run(command("SMEMBERS")(subentry_collection_key(m.id(), set_name)));
        std::set<std::string> ret;
        BOOST_FOREACH(const reply& i, r.elements())
        {
            ret.insert(i.str());
        }
    }

    inline std::string collection_key()
    {
        return Model::model_name() + ":all";
    }

    inline std::string collection_id_key()
    {
        return Model::model_name() + ":id";
    }

    inline std::string model_key(const std::string& id)
    {
        return Model::model_name() + ":" + id;
    }

    inline std::string subentry_collection_key(const std::string& id, const std::string& collection_name)
    {
        return model_key(id) + ":" + collection_name;
    }

    inline std::string indexed_field_key(const std::string& field, const std::string& value)
    {
        return Model::model_name() + ":indices:" + field + ":" + value;
    }

    inline std::string unique_field_key(const std::string& field)
    {
        return Model::model_name() + ":uniques:" + field;
    }

    static script_exec save_script;
    static script_exec remove_script;
};

template<typename Model>
script_exec orm<Model>::save_script(utils::datadir("/lua/save.lua"), true);

template<typename Model>
script_exec orm<Model>::remove_script(utils::datadir("/lua/delete.lua"), true);

}
}
