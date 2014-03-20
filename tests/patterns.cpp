
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
#include <redis3m/patterns/simple_obj_store.h>
#include <redis3m/patterns/model.h>
#include <redis3m/patterns/orm.h>

#define BOOST_TEST_MODULE redis3m
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>

using namespace redis3m;

class test_model: public redis3m::patterns::model
{
public:
    test_model(){}

    test_model(const std::string& test_id, const std::string& field):
        _field(field)
    {
        _id = test_id;
        _loaded = true;
    }

    std::map<std::string, std::string> to_map() const
    {
        return boost::assign::map_list_of("field", _field);
    }

    void from_map(const std::map<std::string, std::string> &map)
    {
        _field = map.at("field");
        _loaded = true;
    }

    static std::string model_name()
    {
        return "Test";
    }

    static std::vector<std::string> indices()
    {
        return boost::assign::list_of("field");
    }

    static std::vector<std::string> uniques()
    {
        return std::vector<std::string>();
    }

    REDIS3M_MODEL_RO_ATTRIBUTE(std::string, field)
    REDIS3M_MODEL_RO_ATTRIBUTE(std::string, field2)
};

class test_connection
{
public:
    test_connection()
    {
        c = redis3m::connection::create(getenv("REDIS_HOST"));
        c->run(command("flushdb"));
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

    reply r = tc->run(command("ZCARD")("test-queue"));
    BOOST_CHECK_EQUAL(r.integer(), 0);
}

BOOST_AUTO_TEST_CASE ( simple_obj_store_test )
{
    test_connection tc;

    patterns::simple_obj_store store;

    test_model new_m("xxx", "test");

    store.save(*tc, new_m);

    test_model restored;

    BOOST_CHECK_EQUAL(store.find(*tc, "xxx", restored), true);

    BOOST_CHECK_EQUAL(restored.field(), "test");
}


BOOST_AUTO_TEST_CASE ( orm_save_test )
{
    test_connection tc;

    patterns::orm store;

    test_model new_m("xxx", "test");

    store.save(*tc, new_m);

//    test_model restored;

//    BOOST_CHECK_EQUAL(store.find(*tc, "xxx", restored), true);

//    BOOST_CHECK_EQUAL(restored.field(), "test");
}
