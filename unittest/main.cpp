//
//  main.cpp
//  unittest
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "connection.h"
#define BOOST_TEST_MODULE redis3m
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>

using namespace redis3m;

class test_connection
{
public:
    test_connection()
    {
        c = redis3m::connection::create();
        c->flushdb();
    }
    redis3m::connection::ptr_t c;
};

BOOST_AUTO_TEST_CASE ( fail_connect )
{
    BOOST_CHECK_THROW(connection::create("localhost", 9090), unable_to_connect);
}

BOOST_AUTO_TEST_CASE( correct_connection)
{
    BOOST_CHECK_NO_THROW(connection::create());
}

BOOST_AUTO_TEST_CASE( test_ping)
{
    connection::ptr_t conn = connection::create();
    conn->append_command(boost::assign::list_of("PING"));
    reply r = conn->get_reply();
    BOOST_CHECK_EQUAL(r.str(), "PONG");
}

BOOST_AUTO_TEST_CASE( set_get)
{
    test_connection tc;

    BOOST_CHECK_EQUAL("", tc.c->get("foo"));
    BOOST_CHECK_NO_THROW(tc.c->set("foo", "bar"));
    BOOST_CHECK_EQUAL("bar", tc.c->get("foo"));
}