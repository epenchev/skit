/*
 * XMLReader.cpp
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
 *  Created on: Jul 16, 2013
 *      Author: emo
 */

#include "XMLReader.h"

bool XMLReader::ReadXMLFile(const std::string& fileName)
{
    bool returnCode = true;
    mErrCode = false;
    try
    {
        if (!fileName.empty())
        {
        	read_xml(fileName, mTree);
        }
    }
    catch (std::exception& exceptionErr)
    {
        mErrCode.SetMessage(exceptionErr.what());
        returnCode = false;
    }
    return returnCode;
}

XMLTree& XMLReader::GetSubTree(XMLTree& tree, const char* xmlTag)
{
	mErrCode = false;
    try
    {
        if (xmlTag)
        {
        	return tree.get_child(xmlTag);
        }
    }
    catch (std::exception& exceptionErr)
    {
        mErrCode.SetMessage(exceptionErr.what());
        return mTree;
    }
    return mTree;
}

std::string XMLReader::GetData(XMLTree& tree, const char* xmlTag)
{
    std::string data;
    mErrCode = false;
    if (xmlTag)
    {
        try
        {
            data = tree.get<std::string>(xmlTag);
        }
        catch (std::exception& exceptionErr)
        {
            // Error geting data
            mErrCode.SetMessage(exceptionErr.what());
            data.clear();
        }
    }
    return data;
}

XMLIterator::XMLIterator(XMLTree& tree)
{
    mptreeIterBegin = tree.begin();
    mptreeIterEnd = tree.end();
}

void XMLIterator::First()
{
    mptreeIterCurrent = mptreeIterBegin;
}

void XMLIterator::Next()
{
    mptreeIterCurrent++;
}

bool XMLIterator::IsDone() const
{
    return !(mptreeIterCurrent != mptreeIterEnd);
}

XMLItem XMLIterator::CurrentItem() const
{
    XMLItem item;
    if (!IsDone())
    {
        //(*iter).second;
        item.name = mptreeIterCurrent->first;
        item.data = mptreeIterCurrent->second.data();
        item.tree = mptreeIterCurrent->second;
    }
    return item;
}



