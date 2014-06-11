// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

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

    template<typename Type>
    inline command& operator<<(const Type arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    template<typename Type>
    inline command& operator()(const Type arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    inline operator const std::vector<std::string>& () {
        return _args;
    }

private:
    std::vector<std::string> _args;
};

template<>
inline command& command::operator<<(const char* arg)
{
    _args.push_back(arg);
    return *this;
}

template<>
inline command& command::operator()(const char* arg)
{
    _args.push_back(arg);
    return *this;
}

template<>
inline command& command::operator<<(const std::string& arg)
{
    _args.push_back(arg);
    return *this;
}

template<>
inline command& command::operator()(const std::string& arg)
{
    _args.push_back(arg);
    return *this;
}
}
