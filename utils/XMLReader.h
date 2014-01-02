/*
 * XMLReader.h
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

#ifndef XMLREADER_H_
#define XMLREADER_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include "ErrorCode.h"

typedef boost::property_tree::ptree::iterator BoostXMLIter;
typedef boost::property_tree::ptree XMLTree;

/**
* Structure to represent XML tree node.
*/
struct XMLItem
{
    std::string name;
    std::string data;
    XMLTree tree;
};

/**
* Class to iterate over XML tree nodes.
*/
class XMLIterator
{
public:
    XMLIterator(XMLTree& tree);
    ~XMLIterator() {}

    /**
    * Set iterator to first element.
    */
    void First();

    /**
    * Move iterator to next element.
    */
    void Next();

    /**
    * Check if iterator is reached the end.
    * @return true if successfully, false otherwise.
    */
    bool IsDone() const;

    /**
    * Return XMLItem currently pointed by iterator.
    */
    XMLItem CurrentItem() const;

private:
    BoostXMLIter mptreeIterCurrent; /**< current element in iterator */
    BoostXMLIter mptreeIterBegin;   /**< first element in iterator */
    BoostXMLIter mptreeIterEnd;     /**< last element in iterator */
};

/**
* Class for reading XML based preferences file.
*/
class XMLReader
{
public:
    XMLReader() {}
    virtual ~XMLReader() {}

    /**
    * Read file into XMLTree structure for later processing.
    * @param fileName - The name of the XML file.
    * @return true if successfully, false otherwise.
    */
    bool ReadXMLFile(const std::string& fileName);

    /**
    * Get XML tree representation of the loaded file.
    * @return XMLTree reference.
    */
    XMLTree& GetDocumentTree() { return mTree; }

    /**
    * Get XML subtree.
    * @param tree - XML tree to use for base.
    * @param xmlTag - Tag(node) to branch the new tree from.
    * @return XMLTree
    */
    XMLTree& GetSubTree(XMLTree& tree, const char* xmlTag);

    /**
    * Get data from XML tree by XML tag id.
    * @param tree - XML tree to use .
    * @param xmlTag - tag(node) to read data from.
    * @return string with data .
    */
    std::string GetData(XMLTree& tree, const char* xmlTag);

    /**
    * Get the error from the last operation.
    * @return ErrorCode reference with last operation error.
    */
    ErrorCode& GetLastError() { return mErrCode; }

private:
    ErrorCode   mErrCode;  /**< error code of last executed operation */
    XMLTree     mTree;     /**< default XML tree which is boost property tree */
};

#endif /* XMLREADER_H_ */
