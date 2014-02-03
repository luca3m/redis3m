//
//  connection.cpp
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "connection.h"
#include <boost/foreach.hpp>

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

void connection::append_commands(const std::vector<std::string> &commands)
{
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;
    
    BOOST_FOREACH(std::string item, commands)
    {
        argv.push_back(item.c_str());
        argvlen.push_back(item.size());
    }
    redisAppendCommandArgv(c, static_cast<int>(commands.size()), argv.data(), argvlen.data());
}

reply connection::get_reply()
{
    redisReply *r;
    redisGetReply(c, reinterpret_cast<void**>(&r));
    return reply(r);
}