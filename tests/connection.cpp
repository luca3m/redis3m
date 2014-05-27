// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#include <redis3m/connection.h>
#include <redis3m/simple_pool.h>

#define BOOST_TEST_MODULE redis3m
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace redis3m;

class test_connection
{
public:
    test_connection()
    {
        c = redis3m::connection::create(getenv("REDIS_HOST"));
        c->run(command("FLUSHDB"));
    }

    inline redis3m::connection::ptr_t operator*()
    {
        return c;
    }

    inline redis3m::connection::ptr_t operator->()
    {
        return c;
    }

    redis3m::connection::ptr_t c;
};

BOOST_AUTO_TEST_CASE ( fail_connect )
{
    BOOST_CHECK_THROW(connection::create("localhost", 9090), unable_to_connect);
}

BOOST_AUTO_TEST_CASE( correct_connection )
{
    BOOST_CHECK_NO_THROW(connection::create(getenv("REDIS_HOST")));
}

BOOST_AUTO_TEST_CASE( test_info)
{
    test_connection tc;
    redis3m::reply r = tc->run(command("INFO"));
}

BOOST_AUTO_TEST_CASE( test_ping)
{
    test_connection tc;
    reply r = tc->run(command("PING"));
    BOOST_CHECK_EQUAL(r.str(), "PONG");
}

BOOST_AUTO_TEST_CASE( set_get)
{
    test_connection tc;

    BOOST_CHECK_EQUAL("", tc->run(command("GET")("foo")).str());
    BOOST_CHECK_NO_THROW(tc->run(command("SET")("foo")("bar")));
    BOOST_CHECK_EQUAL("bar", tc->run(command("GET")("foo")).str());
}

BOOST_AUTO_TEST_CASE( test_pool)
{
    simple_pool::ptr_t pool = simple_pool::create(getenv("REDIS_HOST"));

    connection::ptr_t c = pool->get();

    c->run(command("SET")("foo")("bar"));

    pool->put(c);

    c = pool->get();

    BOOST_CHECK_EQUAL(c->run(command("GET")("foo")).str(), "bar");
    pool->put(c);
}
