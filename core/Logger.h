/*
 * Logger.h
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
 *  Created on: Oct 23, 2013
 *      Author: emo
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <sstream>
#include <string>
#include <cstdio>

inline std::string NowTime();

enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4, logDISABLE};

class Log
{
public:
    Log();
    virtual ~Log();
    std::ostringstream& Get(TLogLevel level = logINFO,
                            const char* func = "", const char* file = "", int line = 0);
public:
    static FILE*& Stream();
    static TLogLevel& ReportingLevel();
    static std::string ToString(TLogLevel level);
    static TLogLevel FromString(const std::string& level);
protected:
    std::ostringstream os;
    static void Output(const std::string& msg);
private:
    Log(const Log&);
    Log& operator =(const Log&);
};

inline Log::Log()
{}

inline std::ostringstream& Log::Get(TLogLevel level,
                             const char* func, const char* file, const int line)
{
	os << "- " << NowTime();
    os << " " << ToString(level) << ": ";
    //if (level >= logDEBUG)
    os << "[ " << file << ":" << line << " " << func << "()" << " ] ";
    os << std::string(level > logDEBUG ? level - logDEBUG : 0, ' ');

    return os;
}

inline Log::~Log()
{
    os << std::endl;
    Output(os.str());
}

inline TLogLevel& Log::ReportingLevel()
{
    static TLogLevel reportingLevel = logDEBUG4;
    return reportingLevel;
}

inline std::string Log::ToString(TLogLevel level)
{
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
    return buffer[level];
}

inline TLogLevel Log::FromString(const std::string& level)
{
    if (level == "DEBUG4")
        return logDEBUG4;
    if (level == "DEBUG3")
        return logDEBUG3;
    if (level == "DEBUG2")
        return logDEBUG2;
    if (level == "DEBUG1")
        return logDEBUG1;
    if (level == "DEBUG")
        return logDEBUG;
    if (level == "INFO")
        return logINFO;
    if (level == "WARNING")
        return logWARNING;
    if (level == "ERROR")
        return logERROR;
    Log().Get(logWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return logINFO;
}

inline FILE*& Log::Stream()
{
    static FILE* pStream = stderr;
    return pStream;
}

inline void Log::Output(const std::string& msg)
{
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

#ifdef LOG_DISABLE
#define LOG(level) \
    if (logDISABLE >= Log::ReportingLevel() || !Log::Stream()) ; \
    else Log().Get(level, __FUNCTION__, __FILE__, __LINE__)
#else
#define LOG(level) \
    if (level > Log::ReportingLevel() || !Log::Stream()) ; \
    else Log().Get(level, __FUNCTION__, __FILE__, __LINE__)
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

inline std::string NowTime()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}

#else

#include <sys/time.h>

inline std::string NowTime()
{
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
    return result;
}

#endif /* WIN32 */

#endif /* LOGGER_H_ */
