//
//  reply.h
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <boost/enable_shared_from_this.hpp>

namespace redis3m {
    class connection;
    class reply: boost::enable_shared_from_this<reply>
    {
    public:
        inline std::string str()
        {
            return std::string(c->str, c->len);
        }
        virtual ~reply();

    private:
        reply(redisReply *reply);
        friend class connection;
        redisReply *c;
    };
}