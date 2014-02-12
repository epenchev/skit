/*
 * StreamClientHTTP.h
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
 *  Created on: Feb 11, 2014
 *      Author: emo
 */

#ifndef STREAMCLIENTHTTP_H_
#define STREAMCLIENTHTTP_H_

#include "stream/StreamClient.h"
#include "server/HTTPServer.h"

class StreamClientHTTP : public StreamClient
{
public:
	virtual ~StreamClientHTTP();

	/* from StreamClient */
	unsigned long GetReadBytes();

	/* from StreamClient */
	unsigned long GetWrittenBytes();

	/* from StreamClient */
	void SetBandwidth(int mbits);

	HTTPConnectionPtr GetHTTPConn();
protected:
	StreamClientHTTP(HTTPConnectionPtr conn);

	HTTPConnectionPtr m_conn;  /**< HTTP session */
};

#endif /* STREAMCLIENTHTTP_H_ */
