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
    inline command(const std::string& arg)
    {
        _args.push_back(arg);
    }

    template<typename Type, typename boost::enable_if< boost::is_arithmetic< Type >, int >::type = 0>
    inline command& operator<<(const Type arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    template<typename Type, typename boost::enable_if< boost::is_arithmetic< Type >, int >::type = 0>
    inline command& operator()(const Type arg)
    {
        _args.push_back(boost::lexical_cast<std::string>(arg));
        return *this;
    }

    template<typename Type, typename boost::enable_if< boost::is_same<std::string, Type >, int >::type = 0>
    inline command& operator<<(const Type arg)
    {
        _args.push_back(arg);
        return *this;
    }

    template<typename Type, typename boost::enable_if< boost::is_same<std::string, Type >, int >::type = 0>
    inline command& operator()(const Type arg)
    {
        _args.push_back(arg);
        return *this;
    }

    template<typename Type, typename boost::enable_if< boost::is_same<const char*, Type >, int >::type = 0>
    inline command& operator<<(const Type arg)
    {
        _args.push_back(arg);
        return *this;
    }

    template<typename Type, typename boost::enable_if< boost::is_same<const char*, Type >, int >::type = 0>
    inline command& operator()(const Type arg)
    {
        _args.push_back(arg);
        return *this;
    }

    inline operator std::vector<std::string>() {
        return _args;
    }

private:
    std::vector<std::string> _args;
};

}
