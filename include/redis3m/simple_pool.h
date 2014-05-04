// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <boost/noncopyable.hpp>
#include <redis3m/connection.h>
#include <set>
#include <memory>
#include <mutex>

namespace redis3m
{

/**
 * @brief Manages a pool of connections to a single Redis server
 */
class simple_pool: boost::noncopyable
{
public:
    typedef std::shared_ptr<simple_pool> ptr_t;
    REDIS3M_EXCEPTION(too_much_retries)

    static inline ptr_t create(const std::string& host, unsigned int port)
    {
        return ptr_t(new simple_pool(host, port));
    }

    /**
     * @brief Get a working connection
     * @return
     */
    connection::ptr_t get();

    /**
     * @brief Put back a connection for reuse
     * @param conn
     */
    void put(connection::ptr_t conn);

    //void run_with_connection(std::function<void(connection::ptr_t)> f, unsigned int retries=5);

    /**
     * @brief Set default database, all connection will be initialized selecting
     * this database.
     * @param value
     */
    inline void set_database(unsigned int value) { _database = value; }

private:
    simple_pool(const std::string& host, unsigned int port);

    std::string _host;
    unsigned int _port;
    unsigned int _database;
    std::set<connection::ptr_t> connections;
    std::mutex access_mutex;
};
}
