// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <string>
#include <redis3m/utils/exception.h>
#include <vector>

struct redisReply;

namespace redis3m {
    
    class connection;
    /**
     * @brief Represent a reply received from redis server
     */
    class reply
    {
    public:
        /**
         * @brief Define reply type
         */
        enum type_t
        {
            STRING = 1,
            ARRAY = 2,
            INTEGER = 3,
            NIL = 4,
            STATUS = 5,
            ERROR = 6
        };

        /**
         * @brief Type of reply, other field values are dependent of this
         * @return
         */
        inline type_t type() const { return _type; }
        /**
         * @brief Returns string value if present, otherwise an empty string
         * @return
         */
        inline const std::string& str() const { return _str; }
        /**
         * @brief Returns integer value if present, otherwise 0
         * @return
         */
        inline long long integer() const { return _integer; }
        /**
         * @brief Returns a vector of sub-replies if present, otherwise an empty one
         * @return
         */
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
