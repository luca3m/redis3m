//
//  reply.cpp
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "reply.h"
#include <hiredis/hiredis.h>

using namespace redis3m;

reply::reply(redisReply *c_reply):
_type(ERROR),
_integer(0)
{
    _type = static_cast<type_t>(c_reply->type);
    switch (_type) {
        case ERROR:
        case STRING:
        case STATUS:
            _str = std::string(c_reply->str, c_reply->len);
            break;
        case INTEGER:
            _integer = c_reply->integer;
            break;
        case ARRAY:
            for (size_t i=0; i < c_reply->elements; ++i) {
                _elements.push_back(reply(c_reply->element[i]));
            }
            break;
        default:
            break;
    }
}