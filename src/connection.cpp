//
//  connection.cpp
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include <redis3m/connection.h>
#include <boost/assign/list_of.hpp>
#include <hiredis/hiredis.h>

using namespace redis3m;

boost::assign_detail::generic_list<std::string>(&redis3m::command)(const std::string&) = boost::assign::list_of<std::string>;

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

void connection::append(const std::vector<std::string> &commands)
{
    std::vector<const char*> argv;
    argv.reserve(commands.size());
    std::vector<size_t> argvlen;
    argvlen.reserve(commands.size());

    for (std::vector<std::string>::const_iterator it = commands.begin(); it != commands.end(); ++it) {
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
    for (int i=0; i < count; ++i)
    {
        ret.push_back(get_reply());
    }
    return ret;
}

bool connection::is_valid()
{
    return c->err == REDIS_OK;
}
