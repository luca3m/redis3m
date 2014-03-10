//
//  script_exec.cpp
//  redis3m
//
//  Created by Luca Marturana on 09/03/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include <redis3m/patterns/script_exec.h>
#include <redis3m/utils/sha1.h>

using namespace redis3m;

patterns::script_exec::script_exec(const std::string& script):
_script(script)
{
    unsigned char hash[20];
    char hexstring[41];
    sha1::calc(script.c_str(),script.size(),hash); // 10 is the length of the string
    sha1::toHexString(hash, hexstring);
    _sha1.assign(hexstring);
}

reply patterns::script_exec::exec(connection::ptr_t connection,
                const std::vector<std::string>& keys,
                const std::vector<std::string>& args)
{
    // TODO: use execsha1, then exec   
}
