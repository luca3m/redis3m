//
//  sentinel_connection.h
//  redis3m
//
//  Created by Luca Marturana on 05/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#pragma once

#include "connection.h"

namespace redis3m {
    class sentinel_connection: connection
    {
        sentinel_connection(const std::string& host="localhost", unsigned int port=26379);


    };
}
