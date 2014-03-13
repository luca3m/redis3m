
//
//  main.cpp
//  unittest
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include <redis3m/connection.h>
#include <redis3m/patterns/script_exec.h>
#include <redis3m/patterns/scheduler.h>

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
        c = redis3m::connection::create(getenv("REDIS_HOST"));
        c->run_command(boost::assign::list_of("flushdb"));
    }

    inline redis3m::connection::ptr_t operator*()
    {
        return c;
    }

    inline redis3m::connection::ptr_t operator->()
    {
        return c;
    }

private:
    redis3m::connection::ptr_t c;
};

using namespace redis3m;

BOOST_AUTO_TEST_CASE ( script_exec )
{
    test_connection tc;

    patterns::script_exec ping_script("return redis.call(\"PING\")");

    reply r = ping_script.exec(*tc);
    BOOST_CHECK_EQUAL(r.str(), "PONG");
}

BOOST_AUTO_TEST_CASE ( scheduler_test )
{
    test_connection tc;
    patterns::scheduler s("test-queue");

    // Enqueue
    s.enqueue(*tc, "testid", 1);

    // Not expired yet
    BOOST_CHECK_EQUAL( s.find_expired(*tc), "");
    sleep(2);

    // Now it should expire
    std::string found_id = s.find_expired(*tc, 2);
    BOOST_CHECK_EQUAL( found_id, "testid");
    sleep(3);

    // Expire again after lock_for passed
    found_id = s.find_expired(*tc, 2);
    BOOST_CHECK_EQUAL( found_id, "testid");

    // Dequeue
    s.dequeue(*tc, found_id);

    reply r = tc->run_command(boost::assign::list_of<std::string>("ZCARD")("test-queue"));
    BOOST_CHECK_EQUAL(r.integer(), 0);
}
