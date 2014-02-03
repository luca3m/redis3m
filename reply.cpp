//
//  reply.cpp
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "reply.h"

using namespace redis3m;

reply::reply(redisReply *reply):
c(reply)
{
}

reply::~reply()
{
    freeReplyObject(c);
}