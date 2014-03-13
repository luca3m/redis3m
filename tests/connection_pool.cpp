//
//  main.cpp
//  unittest
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include <redis3m/connection_pool.h>

#define BOOST_TEST_MODULE redis3m
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>

using namespace redis3m;

BOOST_AUTO_TEST_CASE( test_pool)
{
        connection_pool::ptr_t pool = connection_pool::create(getenv("REDIS_HOST"), "test");

    connection::ptr_t c = pool->get(connection::MASTER);

    c->run_command(boost::assign::list_of("SET")("foo")("bar"));

    pool->put(c);

    c = pool->get(connection::SLAVE);

    BOOST_CHECK_EQUAL(c->run_command(boost::assign::list_of("GET")("foo")).str(), "bar");
}

