//
//  script_exec.cpp
//  redis3m
//
//  Created by Luca Marturana on 09/03/14.
//  Copyright (c) 2014 Luca Marturana. All rights reserved.
//

#include <redis3m/patterns/script_exec.h>
#include <redis3m/utils/sha1.h>
#include <boost/lexical_cast.hpp>

using namespace redis3m;

patterns::script_exec::script_exec(const std::string& script):
_script(script)
{
    unsigned char hash[20];
    char hexstring[41];
    sha1::calc(script.c_str(),script.size(),hash); 
    sha1::toHexString(hash, hexstring);
    _sha1.assign(hexstring);
}

reply patterns::script_exec::exec(connection::ptr_t connection,
                const std::vector<std::string>& keys,
                const std::vector<std::string>& args)
{
    std::vector<std::string> exec_command;
    exec_command.push_back("EVALSHA");
    exec_command.push_back(_sha1);
    exec_command.push_back(boost::lexical_cast<std::string>(keys.size()));
    exec_command.insert(exec_command.end(), keys.begin(), keys.end());
    exec_command.insert(exec_command.end(), args.begin(), args.end());
    reply r = connection->run_command(exec_command);
    if (r.type() == reply::ERROR)
    {
        exec_command[0] = "EVAL";
        exec_command[1] = _script;
        r = connection->run_command(exec_command);
    }
    return r;
}
