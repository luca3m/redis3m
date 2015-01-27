// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <string>
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
    enum class type_t
    {
        TYPE_STRING = 1,
        TYPE_ARRAY = 2,
        TYPE_INTEGER = 3,
        TYPE_NIL = 4,
        TYPE_STATUS = 5,
        TYPE_ERROR = 6
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

    inline operator const std::string&() const { return _str; }

    inline operator long long() const { return _integer; }

    inline bool operator==(const std::string& rvalue) const
    {
		if (_type == type_t::TYPE_STRING || _type == type_t::TYPE_ERROR || _type == type_t::TYPE_STATUS)
        {
            return _str == rvalue;
        }
        else
        {
            return false;
        }
     }

    inline bool operator==(const long long rvalue) const
    {
		if (_type == type_t::TYPE_INTEGER)
        {
            return _integer == rvalue;
        }
        else
        {
            return false;
        }
    }

private:
    reply(redisReply *reply);

    type_t _type;
    std::string _str;
    long long _integer;
    std::vector<reply> _elements;

    friend class connection;
};
}
