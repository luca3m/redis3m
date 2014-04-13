// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

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

template<typename Model>
class simple_obj_store
{
public:
    Model find(connection::ptr_t connection, const std::string& id)
    {
        redis3m::reply r = connection->run(redis3m::command("HGETALL")(Model::model_name() + ":" +  id));

        const std::vector<redis3m::reply>& key_values = r.elements();
        if (key_values.size() > 0)
        {
            std::map<std::string, std::string> serialized;

            for (int i=0; i < key_values.size(); i+=2)
            {
                serialized[key_values.at(i).str()] = key_values.at(i+1).str();
            }
            return Model(id, serialized);
        }
        else
        {
            return Model();
        }
    }

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

    void save(connection::ptr_t connection, const Model& m)
    {
        append_save(connection, m);
        connection->get_reply();
    }

    void append_remove(connection::ptr_t connection, const Model& m)
    {
        connection->append(command("DEL")(m.model_name() + ":" + m.id()));
    }

    void remove(connection::ptr_t connection, const Model& m)
    {
        append_remove(connection, m);
        connection->get_reply();
    }

    inline std::string model_key(const std::string& id)
    {
      return Model::model_name() + ":" + id;
    }
};
}
}
