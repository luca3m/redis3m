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

BOOST_AUTO_TEST_CASE ( fail_connect )
{
    BOOST_CHECK_THROW(connection("localhost", 9090), unable_to_connect);
}

BOOST_AUTO_TEST_CASE( correct_connection)
{
    BOOST_CHECK_NO_THROW(connection());
}

BOOST_AUTO_TEST_CASE( test_ping)
{
    connection conn;
    conn.append_commands(boost::assign::list_of("PING"));
    reply r = conn.get_reply();
    BOOST_CHECK_EQUAL(r.str(), "PONG");
}