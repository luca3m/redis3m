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
#include <redis3m/connection.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <redis3m/utils/common.h>
#include <boost/function.hpp>

namespace redis3m {
    REDIS3M_EXCEPTION(cannot_find_sentinel)
    REDIS3M_EXCEPTION(cannot_find_master)
    REDIS3M_EXCEPTION(cannot_find_slave)
    REDIS3M_EXCEPTION(too_much_retries)
    class connection_pool: boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<connection_pool> ptr_t;

        static inline ptr_t create(const std::string& sentinel_host,
                                   const std::string& master_name,
                                   unsigned int sentinel_port=26379)
        {
            return ptr_t(new connection_pool(sentinel_host, master_name, sentinel_port));
        }

        connection::ptr_t get(connection::role_t type=connection::MASTER);

        void put(connection::ptr_t conn );

        template<typename Ret>
        Ret run_with_connection(boost::function<Ret(connection::ptr_t)> f,
                                connection::role_t conn_type,
                                unsigned int retries=5)
        {
            while (retries > 0)
            {
                try
                {
                    connection::ptr_t c = get(conn_type);
                    Ret r = f(c);
                    put(c);
                    return r;
                } catch (const transport_failure& ex)
                {
                    --retries;
                }
            }
            throw too_much_retries();
        }

        inline void set_database(unsigned int value) { _database = value; }

    private:
        connection_pool(const std::string& sentinel_host,
                        const std::string& master_name,
                        unsigned int sentinel_port);
        connection::ptr_t create_slave_connection();
        connection::ptr_t create_master_connection();
        connection::ptr_t sentinel_connection();

        boost::mutex access_mutex;
        std::set<connection::ptr_t> connections;

        std::string sentinel_host;
        unsigned int sentinel_port;
        std::string master_name;
        unsigned int _database;
    };
}
