#pragma once

#include <string>
#include <redis3m/connection.h>
#include <map>
#include <stdexcept>
#include <boost/foreach.hpp>

namespace redis3m
{
namespace patterns
{

class simple_obj_store
{
public:
    simple_obj_store(){}

    template<typename Model>
    bool find(connection::ptr_t connection, const std::string& id, Model& m)
    {
        redis3m::reply r = connection->run(redis3m::command("HGETALL")(m.model_name() + ":" +  id));
        std::map<std::string, std::string> serialized;
        const std::vector<redis3m::reply>& key_values = r.elements();
        for(int i=0; i < key_values.size()/2; i+=2)
        {
            serialized[key_values.at(i).str()] = key_values.at(i+1).str();
        }
        serialized["id"] = id;
        try
        {
            m.from_map(serialized);
        } catch (const std::out_of_range& ex)
        {
            return false;
        }
        return true;
    }

    template<typename Model>
    void append_save(connection::ptr_t connection, const Model& m)
    {
        std::map<std::string, std::string> serialized = m.to_map();

        std::vector<std::string> hmset_command;
        hmset_command.push_back("HMSET");
        hmset_command.push_back(m.model_name() + ":" + m.id());

        std::pair<std::string, std::string> item;
        BOOST_FOREACH(item, serialized)
        {
            hmset_command.push_back(item.first);
            hmset_command.push_back(item.second);
        }

        connection->append(hmset_command);
    }

    template<typename Model>
    void save(connection::ptr_t connection, const Model& m)
    {
        append_save(connection, m);
        connection->get_reply();
    }

    template<typename Model>
    void append_remove(connection::ptr_t connection, const Model& m)
    {
        connection->append(command("DEL")(m.model_name() + ":" + m.id()));
    }

    template<typename Model>
    void remove(connection::ptr_t connection, const Model& m)
    {
        append_remove(connection, m);
        connection->get_reply();
    }
};
}
}
