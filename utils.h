//
//  utils.h
//  redis3m
//
//  Created by Luca Marturana on 03/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include <exception>
#include <string>

namespace redis3m {
    class exception: public std::exception
    {
    public:
        exception(const std::string& what):
        _what(what){}
        
        inline virtual const char* what()
        {
            return _what.c_str();
        }
    private:
        std::string _what;
    };
}

#define REDIS3M_EXCEPTION(name) class name: public redis3m::exception {\
public: name(const std::string& what=""): exception(what){}};