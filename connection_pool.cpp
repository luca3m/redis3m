//
//  connection_pool.cpp
//  redis3m
//
//  Created by Luca Marturana on 05/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "connection_pool.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

using namespace redis3m;

connection_pool::connection_pool(const std::string& sentinel_host,
                                 const std::string& master_name,
                                 unsigned int sentinel_port):
sentinel_host(sentinel_host),
master_name(master_name),
sentinel_port(sentinel_port)
{

}

connection::ptr_t connection_pool::get(connection::role_t type)
{
    connection::ptr_t ret;

    // Look for a cached connection
    access_mutex.lock();
    std::set<connection::ptr_t>::iterator it;
    switch (type) {
        case connection::ANY:
            it = connections.begin();
            break;
        case connection::MASTER:
        case connection::SLAVE:
            it = std::find_if(connections.begin(), connections.end(),
                           ( boost::lambda::bind(&connection::_role, *boost::lambda::_1) == type ));
            break;
    }
    for (; it != connections.end(); ++it)
    {
        connections.erase(it);
        if (it->get()->is_valid())
        {
            ret = *it;
            break;
        }
    }
    access_mutex.unlock();

    // If no connection found, create a new one
    if (!ret)
    {
        switch (type) {
            case connection::SLAVE:
            {
                ret = create_slave_connection();
                ret->_role = connection::SLAVE;
                break;
            }
            case connection::ANY:
            {
                try {
                    ret = create_slave_connection();
                    ret->_role = connection::SLAVE;
                    break;
                } catch (const cannot_find_slave& ex) {
                    // Go ahead, looking for a master, no break istruction
                }
            }
            case connection::MASTER:
                ret = create_master_connection();
                ret->_role = connection::MASTER;
                break;
        }
    }
    return ret;
}

void connection_pool::put(connection::ptr_t conn)
{
    boost::unique_lock<boost::mutex> lock(access_mutex);
    connections.insert(conn);
}

connection::ptr_t connection_pool::sentinel_connection()
{
    std::vector<std::string> real_sentinels = resolv::get_addresses(sentinel_host);
    REDIS3M_LOG(boost::str(
                           boost::format("Found %d redis sentinels: %s")
                           % real_sentinels.size()
                           % boost::algorithm::join(real_sentinels, ", ")
                           )
                );
    BOOST_FOREACH( const std::string& real_sentinel, real_sentinels)
    {
        REDIS3M_LOG(boost::str(boost::format("Trying sentinel %s") % real_sentinel));
        try
        {
            return connection::create(real_sentinel, sentinel_port);
        } catch (const unable_to_connect& ex)
        {
            REDIS3M_LOG(boost::str(boost::format("%s is down") % real_sentinel));
        }
    }

    throw cannot_find_sentinel("Cannot find sentinel");
}

connection::ptr_t connection_pool::create_slave_connection()
{
    connection::ptr_t sentinel = sentinel_connection();
    sentinel->append_command(boost::assign::list_of<std::string>("SENTINEL")("slaves")(master_name));
    reply response = sentinel->get_reply();
    for (std::vector<reply>::const_iterator it = response.elements().begin();
         it != response.elements().end(); ++it)
    {
        const std::vector<reply>& properties = it->elements();
        if (properties.at(9).str() == "slave")
        {
            std::string host = properties.at(3).str();
            unsigned int port = boost::lexical_cast<unsigned int>(properties.at(5).str());
            try
            {
                return connection::create(host, port);
            } catch (const unable_to_connect& ex)
            {
                REDIS3M_LOG(boost::str(boost::format("Error on connection to Slave %s:%d declared to be up") % host % port));
            }
        }
    }
    throw cannot_find_slave();
}

connection::ptr_t connection_pool::create_master_connection()
{
    connection::ptr_t sentinel = sentinel_connection();

    unsigned int connection_retries = 0;
    while(connection_retries < 5)
    {
        sentinel->append_command(boost::assign::list_of(std::string("SENTINEL"))
                                 (std::string("get-master-addr-by-name"))
                                 (master_name)
                                 );
        reply response = sentinel->get_reply();
        std::string master_ip = response.elements().at(0).str();
        unsigned int master_port = boost::lexical_cast<unsigned int>(response.elements().at(1).str());

        try
        {
            return connection::create(master_ip, master_port);
        } catch (const unable_to_connect& ex)
        {
            REDIS3M_LOG(boost::str(boost::format("Error on connection to Master %s:%d declared to be up, waiting") % master_ip % master_port));
        }
        connection_retries++;
        sleep(5);
    }
    throw cannot_find_master(boost::str(boost::format("Unable to find master of name: %s (too much retries") % master_name));
}