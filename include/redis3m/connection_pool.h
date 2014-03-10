//
//  connection_pool.h
//  redis3m
//
//  Created by Luca Marturana on 05/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <string>
#include <set>
#include "connection.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "utils.h"

namespace redis3m {
    REDIS3M_EXCEPTION(cannot_find_sentinel)
    REDIS3M_EXCEPTION(cannot_find_master)
    REDIS3M_EXCEPTION(cannot_find_slave)
    class connection_pool: boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<connection_pool> ptr_t;

        connection_pool(const std::string& sentinel_host,
                        const std::string& master_name,
                        unsigned int sentinel_port=26379);

        connection::ptr_t get(connection::role_t type=connection::MASTER);
        
        void put(connection::ptr_t conn );

    private:
        connection::ptr_t create_slave_connection();
        connection::ptr_t create_master_connection();
        connection::ptr_t sentinel_connection();

        boost::mutex access_mutex;
        std::set<connection::ptr_t> connections;

        std::string sentinel_host;
        unsigned int sentinel_port;
        std::string master_name;
    };
}
