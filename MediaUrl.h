/*
 * MediaUrl.h
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

#ifndef MEDIAURL_H_
#define MEDIAURL_H_

#include <string>

namespace blitz {

/**
* Media URL container.
* Supports HTTP for now, can be extended to support RTSP, RTP, UDP ...
*/
class MediaUrl
{
public:
    MediaUrl(const std::string& url);

    std::string serverName();
    std::string resource();
    std::string service();
private:
    /**
    * Reads HTTP URL and extracts parameters from it.
    * @param url: URL string
    */
    void readUrl(const std::string& url);

    std::string server_; /**< server name. */
    std::string service_; /**< network port numeric or string service ("http") */
    std::string resource_; /**< resource to be fetched */
};

#endif /* MEDIAURL_H_ */

} // blitz
