/*
 * MediaSessionDB.cpp
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
 *  Created on: Apr 10, 2013
 *      Author: emo
 */

#include "MediaSessionDB.h"
#include "Log.h"

namespace blitz {

MediaSessionDB::MediaSessionDB()
{}

MediaSessionDB::~MediaSessionDB()
{}

void MediaSessionDB::InsertRecord(MediaSessionDB::Record& rec_in)
{
    MediaSessionDB::Record insert_rec;
    insert_rec = rec_in;

    boost::mutex::scoped_lock lock(m_io_mutex);
    m_dbrecords.insert(std::pair<record_id, MediaSessionDB::Record>(insert_rec.id, insert_rec));
    m_rec_id.push_back(insert_rec.id);
}

void MediaSessionDB::UpdateRecord(MediaSessionDB::Record& rec_in)
{
    record_id id = rec_in.id;
    boost::mutex::scoped_lock lock(m_io_mutex);
    m_dbrecords.at(id) = rec_in;
}

bool MediaSessionDB::SelectRecord(record_id id, MediaSessionDB::Record& rec_out)
{
    if (m_dbrecords.count(id) > 0)
    {
        rec_out = m_dbrecords.at(id);
        return true;
    }
    else
    {
        BLITZ_LOG_WARNING("No record with this ID: %lu", id);
        return false;
    }
}

void MediaSessionDB::DeleteRecord(record_id id)
{
    boost::mutex::scoped_lock lock(m_io_mutex);

    int it_idx = 0;
    std::map<record_id,MediaSessionDB::Record>::iterator it = m_dbrecords.find(id);
    m_dbrecords.erase(it);

    for (std::vector<record_id>::iterator it = m_rec_id.begin() ; it != m_rec_id.end(); ++it)
    {
        if (id == *it) {
            break;
        }
        it_idx++;
    }

    m_rec_id.erase(m_rec_id.begin() + it_idx);
}

record_id MediaSessionDB::GetRecordId(unsigned int index)
{
    if (index >= m_rec_id.size())
        return 0;
    else
        return m_rec_id[index];
}

std::size_t MediaSessionDB::GetRecordCount()
{
    return m_dbrecords.size();
}

RecordIterator MediaSessionDB::BeginIterator()
{
    return m_dbrecords.begin();
}

RecordIterator MediaSessionDB::EndIterator()
{
    return m_dbrecords.end();
}

} // blitz
