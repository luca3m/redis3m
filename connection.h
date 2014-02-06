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
#include <list>
#include "reply.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace redis3m {
    REDIS3M_EXCEPTION(unable_to_connect)
    REDIS3M_EXCEPTION(transport_failure)
    class connection: boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<connection> ptr_t;
        connection(const std::string& host="localhost", const unsigned port=6379);
        virtual ~connection();
        void append_command(const std::list<std::string>& args);
        reply::ptr_t get_reply();
        std::vector<reply::ptr_t> get_replies(int count);

        inline redisContext* c_ptr()
        {
            return c;
        }
        
        // Strings
        void set(const std::string& key, const std::string& value);
        std::string get(const std::string& key);

        // Server

        void flushdb();
    private:
        redisContext *c;
    };
}