/*
 * MediaSessionDB.h
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
 * Created on: Apr 10, 2013
 *      Author: emo
 */

#ifndef MEDIASESSIONDB_H_
#define MEDIASESSIONDB_H_

#include <map>
#include <vector>
#include <string>
#include <boost/thread/mutex.hpp>

namespace blitz {

typedef unsigned long int record_id;

class MediaSessionDB
{
public:
    MediaSessionDB();
    virtual ~MediaSessionDB();

    struct Record
    {
        record_id id;                 /**< session ID, unique key for the record */
        std::string ip_address;       /**< IP address of the connected host */
        unsigned int livetime_sec;    /**< live time in seconds */
    };

    void InsertRecord(MediaSessionDB::Record& rec_in);
    void UpdateRecord(MediaSessionDB::Record& rec_in);
    bool SelectRecord(record_id id, MediaSessionDB::Record& rec_out);
    void DeleteRecord(record_id id);
    std::map<record_id,MediaSessionDB::Record>::iterator BeginIterator();
    std::map<record_id,MediaSessionDB::Record>::iterator EndIterator();
    std::size_t GetRecordCount();
    record_id GetRecordId(unsigned int index);

private:
    std::map<record_id,MediaSessionDB::Record> m_dbrecords;
    boost::mutex m_io_mutex;
    std::vector<record_id> m_rec_id;
};

typedef std::map<record_id,MediaSessionDB::Record>::iterator RecordIterator;

} // blitz

#endif /* MEDIASESSIONDB_H_ */
