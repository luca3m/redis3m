//
//  connection.cpp
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "connection.h"
#include <boost/assign.hpp>
#include <vector>

using namespace redis3m;

connection::connection(const std::string& host, const unsigned port)
{
    c = redisConnect(host.c_str(), port);
    if (c->err != REDIS_OK)
    {
        throw unable_to_connect();
    }
}

connection::~connection()
{
    redisFree(c);
}

void connection::append_command(const std::list<std::string> &commands)
{
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;

    for (std::list<std::string>::const_iterator it = commands.begin(); it != commands.end(); ++it) {
        argv.push_back(it->c_str());
        argvlen.push_back(it->size());
    }

    int ret = redisAppendCommandArgv(c, static_cast<int>(commands.size()), argv.data(), argvlen.data());
    if (ret != REDIS_OK)
    {
        throw transport_failure();
    }
}

reply connection::get_reply()
{
    redisReply *r;
    int error = redisGetReply(c, reinterpret_cast<void**>(&r));
    if (error != REDIS_OK)
    {
        throw transport_failure();
    }
    reply ret(r);
    freeReplyObject(r);
    return ret;
}

std::vector<reply> connection::get_replies(int count)
{
    std::vector<reply> ret;
    redisReply *r;
    for (int i=0; i < count; ++i)
    {
        int error = redisGetReply(c, reinterpret_cast<void**>(&r));
        if (error != REDIS_OK)
        {
            throw transport_failure();
        }
        ret.push_back(reply(r));
    }
    freeReplyObject(r);
    return ret;
}

void connection::set(const std::string &key, const std::string &value)
{
    append_command(boost::assign::list_of(std::string("SET"))(key)(value));
    get_reply();
}

std::string connection::get(const std::string &key)
{
    append_command(boost::assign::list_of(std::string("GET"))(key));
    return get_reply().str();
}

void connection::flushdb()
{
    append_command(boost::assign::list_of("FLUSHDB"));
    get_reply();
}