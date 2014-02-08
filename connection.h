//
//  connection.h
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <string>
#include "utils.h"
#include <list>
#include "reply.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

struct redisContext;

namespace redis3m {
    REDIS3M_EXCEPTION(unable_to_connect)
    REDIS3M_EXCEPTION(transport_failure)
    class connection: boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<connection> ptr_t;
        inline static ptr_t create(const std::string& host="localhost", const unsigned int port=6379)
        {
            return ptr_t(new connection(host, port));
        }
        virtual ~connection();
        void append_command(const std::list<std::string>& args);
        reply get_reply();
        std::vector<reply> get_replies(int count);

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
        connection(const std::string& host, const unsigned int port);
        redisContext *c;
    };
}