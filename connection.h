//
//  connection.h
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <string>
#include <hiredis/hiredis.h>
#include "utils.h"
#include <vector>
#include "reply.h"

namespace redis3m {
    REDIS3M_EXCEPTION(unable_to_connect)

    class connection
    {
    public:
        connection(const std::string& host="localhost", const unsigned port=6379);
        virtual ~connection();
        void append_commands(const std::vector<std::string>& commands);
        reply get_reply();
    private:
        redisContext *c;
    };
}