//
//  connection.cpp
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "connection.h"

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