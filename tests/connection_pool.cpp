// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#include <redis3m/connection_pool.h>

#define BOOST_TEST_MODULE redis3m
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>
#include <thread>
#include <redis3m/utils/logging.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;

void producer_f(connection_pool::ptr_t pool, const std::string& queue_name)
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
    std::this_thread::yield();
    usleep(10000);
}

void consumer_f(connection_pool::ptr_t pool, const std::string& queue_name)
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
    std::this_thread::yield();
    usleep(10000);
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

BOOST_AUTO_TEST_CASE (run_with_connection)
{
    connection_pool::ptr_t pool = connection_pool::create(getenv("REDIS_HOST"), "test");

    pool->run_with_connection<void>([](connection::ptr_t c)
    {
       c->run(command("SET")("foo")("bar"));
       BOOST_CHECK_EQUAL(c->run(command("GET")("foo")).str(), "bar");
    });
}

BOOST_AUTO_TEST_CASE (crash_test)
{
    connection_pool::ptr_t pool = connection_pool::create(getenv("REDIS_HOST"), "test");

    std::vector<std::shared_ptr<std::thread>> producers;
    std::vector<std::shared_ptr<std::thread>> consumers;

    for (int i=0; i < 4; ++i)
    {
        producers.push_back( std::make_shared<std::thread>(std::bind(&producer_f, pool, "test-queue")));
        consumers.push_back(std::make_shared<std::thread>(std::bind(&consumer_f, pool, "test-queue")));
    }

    //producers.interrupt_all();
    //consumers.interrupt_all();
    for (auto producer : producers)
    {
        producer->join();
    }
    for (auto consumer : consumers)
    {
        consumer->join();
    }
}

