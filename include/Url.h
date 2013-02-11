/*
 * Url.h
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
 *  Created on: Jan 13, 2013
 *      Author: emo
 */

#ifndef URL_H_
#define URL_H_

#include <string>

namespace blitz {

/**
* URL container.
* Supports HTTP for now, can be extended to support RTSP, RTP, UDP ...
*/
class Url
{
public:
    Url(const std::string& url);

    std::string serverName();
    std::string resource();
    std::string service();
private:
    /**
    * Reads HTTP URL and extracts parameters from it.
    * @param url: URL string
    */
    void readUrl(const std::string& url);

    std::string m_server;   /**< server name. */
    std::string m_service;  /**< network port numeric or string service ("http") */
    std::string m_resource; /**< resource to be fetched */
};

#endif /* URL_H_ */

} // blitz
