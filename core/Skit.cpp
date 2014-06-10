//
// Skit.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "ServerController.h"
#include <cstdio>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    std::string filename;
    std::vector<std::string> args(argv, argv+argc);

    if (args.size() == 3)
    {
        if (args[1] == "-c")
        {
        	filename = args[2];
        }
        else
        {
            printf("Usage: %s -c filename \n", argv[0]);
            return 1;
        }
    }
    else
    {
        printf("Usage: %s -c filename \n", argv[0]);
        return 1;
    }

    Skit::ServerController& g_server = Skit::ServerController::Instance();
    g_server.Load(filename);
	
    return 0;
}

