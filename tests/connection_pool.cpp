// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#include <redis3m/connection_pool.h>

#define BOOST_TEST_MODULE redis3m
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>
#include <boost/thread.hpp>
#include <redis3m/utils/logging.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;

void producer_f(connection_pool::ptr_t pool, const std::string& queue_name)
{
    while (!boost::this_thread::interruption_requested())
    {
        try
        {
            connection::ptr_t conn = pool->get();
            reply counter = conn->run(command("INCR")("counter"));
            conn->run(command("LPUSH")(queue_name)(boost::lexical_cast<std::string>(counter.integer())));
            pool->put(conn);
        }
        catch (const redis3m::transport_failure& ex)
        {
            logging::debug("Failure on producer");
        }
        boost::this_thread::yield();
        usleep(10000);
    }
}

void consumer_f(connection_pool::ptr_t pool, const std::string& queue_name)
{
    while (!boost::this_thread::interruption_requested())
    {
        try
        {
            connection::ptr_t conn = pool->get();
            reply r = conn->run(command("BRPOP")(queue_name)("1"));
            pool->put(conn);
        }
        catch (const redis3m::transport_failure& ex)
        {
            logging::debug("Failure on consumer");
        }
        boost::this_thread::yield();
        usleep(10000);
    }
}

BOOST_AUTO_TEST_CASE( test_pool)
{
    connection_pool::ptr_t pool = connection_pool::create(getenv("REDIS_HOST"), "test");

    connection::ptr_t c = pool->get(connection::MASTER);

    c->run(command("SET")("foo")("bar"));

    pool->put(c);

    c = pool->get(connection::SLAVE);

    BOOST_CHECK_EQUAL(c->run(command("GET")("foo")).str(), "bar");
}

BOOST_AUTO_TEST_CASE (crash_test)
{
    connection_pool::ptr_t pool = connection_pool::create(getenv("REDIS_HOST"), "test");

    boost::thread_group producers;
    boost::thread_group consumers;

    for (int i=0; i < 4; ++i)
    {
        producers.add_thread(new boost::thread(boost::bind(&producer_f, pool, "test-queue")));
        consumers.add_thread(new boost::thread(boost::bind(&consumer_f, pool, "test-queue")));
    }

    //producers.interrupt_all();
    //consumers.interrupt_all();
    producers.join_all();
    consumers.join_all();
}

