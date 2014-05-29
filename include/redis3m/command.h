// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>

namespace redis3m
{

class command
{
public:
    inline command() {}

    inline command(const std::string& arg)
    {
        _args.push_back(arg);
    }

    inline command& operator<<(const unsigned long arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator()(const unsigned long arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator<<(const float arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator()(const float arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator<<(const double arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator()(const double arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator<<(const long arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator()(const long arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline command& operator<<(const char* arg)
    {
        _args.push_back(arg);
        return *this;
    }

    inline command& operator()(const char* arg)
    {
        _args.push_back(arg);
        return *this;
    }

    inline command& operator<<(const std::string& arg)
    {
        _args.push_back(arg);
        return *this;
    }

    inline command& operator()(const std::string& arg)
    {
        _args.push_back(arg);
        return *this;
    }

    inline operator std::vector<std::string>& () {
        return _args;
    }

private:
    std::vector<std::string> _args;
};

}
