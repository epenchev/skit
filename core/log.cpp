/*
 * log.cpp
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
 *
 * File logger utility, support different levels of logging.
 *
 */

#include <cstdio>
#include <cstdarg>
#include <string>
#include <stdexcept>
#include <exception>

#ifdef __GNUC__
#define ATTRIB_LOG_FORMAT __attribute__((format(printf,2,3)))
#else
#define ATTRIB_LOG_FORMAT
#endif

///  General error class used for throwing exceptions from clog.
class clog_error : public std::runtime_error
{
public:
    ///  Construct error
    clog_error(const std::string& what_arg) : runtime_error(what_arg)
    {
    }

    ~clog_error() throw()
    {
    }
};

///  Log messages to file with different levels of logging.
class clog
{
public:

    enum log_level_t { error = 0, warning, info, critical, debug, disable };
    class clog_globals
    {
    public:
        clog_globals() : m_file(NULL), m_logLevel(clog::error)
        {
        }
        FILE*       m_file;
        log_level_t m_logLevel;
    };

    clog()
    {
    }

    ~clog(void)
    {
    }

    ///  Open log file, throws exception clog_error on error.
    static void open(const std::string& filepath);

    /// close log file.
    static void close();

    /// Add log entry to file.
    static void log(log_level_t loglevel, const char *format, ... ) ATTRIB_LOG_FORMAT;

    /// Get current log level.
    static void level(int level);

    /// Set current log level.
    static int  level();
};

static clog::clog_globals g_log_globals;

static void now_time(char* outstr_time, size_t len)
{
    char buffer[11] = {0};
    time_t t;
    time(&t);
    struct tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    snprintf(outstr_time, len, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
}

void clog::open(const std::string& filepath = "")
{
    if (!g_log_globals.m_file)
    {
        if (!filepath.empty())
        {
            g_log_globals.m_file = fopen(filepath.c_str(), "w");
            if (!g_log_globals.m_file)
                throw clog_error("Error opening log file : " + filepath);
        }
        else
        {
            g_log_globals.m_file = stdout;
        }
    }
}

void clog::close()
{
    if (g_log_globals.m_file)
    {
        fclose(g_log_globals.m_file);
        g_log_globals.m_file = NULL;
    }
}

void clog::log(log_level_t loglevel, const char *format, ...)
{
    va_list arg;
    FILE *log_file = g_log_globals.m_file;
    char strtime[100] = {0};

    now_time(strtime, sizeof(strtime));
    fprintf(log_file, "- %s %s: ", strtime, severity_name[level]);
    va_start(arg, format);
    vfprintf(log_file, format, arg);
    va_end(arg);
    fflush(log_file);
}

int main()
{
    return 0;
}








