//
//  reply.h
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <string>
#include <redis3m/utils/common.h>
#include <vector>

struct redisReply;

namespace redis3m {
    
    class connection;
    class reply
    {
    public:
        enum type_t
        {
            STRING = 1,
            ARRAY = 2,
            INTEGER = 3,
            NIL = 4,
            STATUS = 5,
            ERROR = 6
        };

        inline type_t type() const { return _type; }
        inline const std::string& str() const { return _str; }
        inline long long integer() const { return _integer; }
        inline const std::vector<reply>& elements() const { return _elements; }

    private:
        reply(redisReply *reply);

        type_t _type;
        std::string _str;
        long long _integer;
        std::vector<reply> _elements;

        friend class connection;
    };
}
