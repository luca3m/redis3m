// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#include <redis3m/utils/logging.h>
#include <iostream>

using namespace redis3m;

logging::ptr_t logging::logger(new logging_impl());

void logging_impl::debug_impl(const std::string &string)
{
    std::lock_guard<std::mutex> lock(access);
    std::cerr << string << std::endl;
}

void logging_impl::warning_impl(const std::string &string)
{
    std::lock_guard<std::mutex> lock(access);
    std::cerr << string << std::endl;
}

void logging_impl::error_impl(const std::string &string)
{
    std::lock_guard<std::mutex> lock(access);
    std::cerr << string << std::endl;
}

