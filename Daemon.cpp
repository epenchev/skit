/*
 * Daemon.cpp
 *
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
 *  Created on: Feb 16, 2013
 *      Author: emo
 */
extern "C" {
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
}

#include "Daemon.h"
#include "Log.h"

// TODO signal handler for caller
// TODO port for windows

namespace blitz {

void Daemon::daemonize(const char* lockfile, const char* logfile)
{
	if (1 == getppid())
	{
		return; /* already a daemon */
	}

	if (pid_t pid = fork())
	{
		if (pid > 0)
		{
			exit(0);
		}
	    else
	    {
	    	BLITZ_LOG_ERROR("fork failed");
	        exit(1);
	    }
	}

	setsid(); /* obtain a new process group */
	for (int i = getdtablesize(); i>=0; --i)
	{
		close(i); /* close all descriptors */
	}

	// We don't want the daemon to have any standard input.
    if (open("/dev/null", O_RDONLY) < 0)
	{
    	syslog(LOG_ERR | LOG_USER, "Unable to open /dev/null: %m");
	    exit(1);
	}

	// Send standard output to a log file.
	if (logfile)
	{
		const int flags = O_WRONLY | O_CREAT | O_TRUNC;
		const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

		if (open(logfile, flags, mode) < 0)
		{
			syslog(LOG_ERR | LOG_USER, "Unable to open output log file %s: %m", logfile);
			exit(1);
		}
	}

	// Also send standard error to the same log file.
	if (dup(1) < 0)
	{
		syslog(LOG_ERR | LOG_USER, "Unable to dup output descriptor: %m");
	    exit(1);
	}

	if (lockfile)
	{
		int lockfp;
		char pid[10];

		lockfp = open(lockfile, O_RDWR | O_CREAT, 0640);
		if (lockfp < 0)
		{
			BLITZ_LOG_ERROR("Can't open lockfile: %s", lockfile);
			exit(1);
		}

		sprintf(pid, "%d\n", getpid());
		write(lockfp, pid, strlen(pid)); /* record pid to lockfile */

		signal(SIGCHLD,SIG_IGN);
		signal(SIGTSTP,SIG_IGN);
		signal(SIGTTOU,SIG_IGN);
		signal(SIGTTIN,SIG_IGN);
	}
}

} // blitz


