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

using namespace redis3m;

connection_pool::connection_pool(const std::string& sentinel_host,
                                 const std::string& master_name,
                                 unsigned int sentinel_port):
    sentinel_host(sentinel_host),
    master_name(master_name),
    sentinel_port(sentinel_port)
{

}

connection::ptr_t connection_pool::sentinel_connection()
{
    connection::ptr_t sentinel;

    std::vector<std::string> real_sentinels = resolv::get_addresses(sentinel_host);
    REDIS3M_LOG(boost::str(
                           boost::format("Found %d redis sentinels: %s")
                           % real_sentinels.size()
                           % boost::algorithm::join(real_sentinels, ", ")
                           )
                );
    BOOST_FOREACH( std::string real_sentinel, real_sentinels)
    {
        REDIS3M_LOG(boost::str(boost::format("Trying sentinel %s") % real_sentinel));
        try
        {
            sentinel.reset(new connection(real_sentinel, sentinel_port));
            break;
        } catch (const unable_to_connect& ex)
        {
            REDIS3M_LOG(boost::str(boost::format("%s is down") % real_sentinel));
        }
    }

    if (!sentinel)
    {
        throw cannot_find_sentinel("Cannot find sentinel");
    }
    return sentinel;
}