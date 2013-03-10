/*
 * Config.h
 *
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
 *  Created on: Feb 17, 2013
 *      Author: emo
 */

// TODO read class names from configuration file

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

namespace blitz {

struct HttpPipelineConfig
{
    unsigned short id;
    std::string name;
    std::string source_url;
    std::string source_ip;
    std::string sink_ip;
    unsigned short sink_port;
    unsigned short sink_sessions;
};

class Config
{
public:
    Config() {}
    void readConfig(const std::string &filename);

    std::string& getLogfile(void) { return m_logfile; }
    std::string& getPidfile(void) { return m_pidfile; }
    unsigned short getNumThreads(void)  { return m_threads; }
    unsigned getNumPipeline(void) { return m_pipeline_configs.size(); }

    // pipeline specific configuration API information
    unsigned short getPipelineID(unsigned id);
    unsigned short getPipelineSinkPort(unsigned id);
    unsigned short getPipelineSinkMaxSesssions(unsigned id);
    std::string& getPipelineName(unsigned id);
    std::string& getPipelineSourceURL(unsigned id);
    std::string& getPipelineSinkIP(unsigned id);

private:
    std::string m_logfile;
    std::string m_pidfile;
    unsigned short m_threads;
    std::vector<HttpPipelineConfig> m_pipeline_configs;
};

} // blitz

#endif /* CONFIG_H_ */
