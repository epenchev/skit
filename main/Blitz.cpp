/*
 * Blitz.cpp
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Created on: Jan 6, 2013
 *      Author: emo
 */

#include "server/ServerController.h"

int main(int argc, char* argv[])
{
#if 0


    if (args.size() == 3)
    {
        if (args[1] == "-c")
        {
            config_filename = args[2];
        }
        else
        {
            printf("Usage: %s -c conf.xml \n", argv[0]);
            return 1;
        }
    }
    else
    {
        printf("Usage: %s -c conf.xml \n", argv[0]);
        return 1;
    }
#endif

    ServerController::StartServer();
    while(1) {} // block here
    return 0;
}


