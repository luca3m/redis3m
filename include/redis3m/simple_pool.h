// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <boost/noncopyable.hpp>
#include <redis3m/connection.h>
#include <boost/thread/mutex.hpp>
#include <set>

namespace redis3m
{

/**
 * @brief Manages a pool of connections to a single Redis server
 */
class simple_pool: boost::noncopyable
{
public:
    typedef boost::shared_ptr<simple_pool> ptr_t;

    inline ptr_t create(const std::string& host, unsigned int port)
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
    boost::mutex access_mutex;
};
}
