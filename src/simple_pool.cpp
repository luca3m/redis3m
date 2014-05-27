// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#include <redis3m/simple_pool.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;

connection::ptr_t simple_pool::get()
{
    connection::ptr_t ret;

    access_mutex.lock();
    for (std::set<connection::ptr_t>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
        if (it->get()->is_valid())
        {
            ret = *it;
            connections.erase(it);
            break;
        }
        connections.erase(it);
    }
    access_mutex.unlock();

    if (!ret)
    {
        ret = connection::create(_host, _port);
        // Setup connections selecting db
        if (_database != 0)
        {
            ret->run(command("SELECT")(boost::lexical_cast<std::string>(_database)));
        }
    }
    return ret;
}


void simple_pool::put(connection::ptr_t conn)
{
    boost::unique_lock<boost::mutex> lock(access_mutex);
    connections.insert(conn);
}

simple_pool::simple_pool(const std::string &host, unsigned int port):
    _host(host),
    _port(port),
    _database(0)
{

}

template<>
void simple_pool::run_with_connection(boost::function<void(connection::ptr_t)> f,
                                unsigned int retries)
{
    while (retries > 0)
    {
        try
        {
            connection::ptr_t c = get();
            f(c);
            put(c);
            return;
        } catch (const transport_failure& ex)
        {
            --retries;
        }
    }
    throw too_much_retries();
}
