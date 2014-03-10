//
//  utils.cpp
//  redis3m
//
//  Created by Luca Marturana on 05/02/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include "redis3m/utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


std::vector<std::string> resolv::get_addresses(const std::string &hostname)
{
    std::vector<std::string> ret_v;
    struct addrinfo hints, *ret_addrinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_INET;

    int addrinfo_ret = getaddrinfo(hostname.c_str(), NULL, &hints, &ret_addrinfo);
    if (addrinfo_ret != 0)
    {
        throw resolv::cannot_resolve_hostname("getaddrinfo returned != 0");
    }

    struct addrinfo *ret_ptr = ret_addrinfo;
    while(ret_ptr)
    {
        char addrstr[100];
        inet_ntop (ret_ptr->ai_family, &((struct sockaddr_in *) ret_ptr->ai_addr)->sin_addr, addrstr, 100);
        ret_v.push_back(std::string(addrstr));
        ret_ptr = ret_ptr->ai_next;
    }

    freeaddrinfo(ret_addrinfo);
    return ret_v;
}
