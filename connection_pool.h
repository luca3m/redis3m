//
//  connection_pool.h
//  redis3m
//
//  Created by Luca Marturana on 05/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <string>
#include "connection.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace redis3m {
    class connection_pool: boost::enable_shared_from_this<connection_pool>
    {
    public:

        enum node_role_t{
            ANY = 0,
            MASTER = 1,
            SLAVE = 2,
        };

        connection_pool(const std::string& sentinel_host, unsigned int sentinel_port=26379);
        virtual ~connection_pool();

        connection get(const std::string& master_name, node_role_t type=MASTER);
        void put(const connection& conn );

    private:
        boost::thread sentinel_subscriber;
        boost::mutex access_mutex;
        std::string sentinel_host;
        unsigned int sentinel_port;
    };
}
