//
// PropertyTree.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef PROPERTY_TREE_H_
#define PROPERTY_TREE_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

namespace Skit {

// Boost property_tree and property_tree::iterator wrappers
class PropertyTree
{
public:
    PropertyTree() {}
    
    class Iterator
    {
    public:
        Iterator() {}
        Iterator(boost::property_tree::ptree& tree)
        { 
            m_iterImpl = tree.begin();
        }
        
        // Return a subtree from the given key
        PropertyTree GetTree(const std::string& path = "")
        {
            PropertyTree tree;
            if (path.empty())
                tree.m_treeImpl = m_iterImpl->second;
            else tree.m_treeImpl = m_iterImpl->second.get_child(path);

            return tree;
        }
        
        template <class T>
        T GetData(const std::string& key = "")
        {
            if (key.empty())
                return boost::lexical_cast<T>( m_iterImpl->second.data() );
            else return m_iterImpl->second.get<T>(key);
        }
        
        std::string GetData(const std::string& key = "")
        { 
            return GetData<std::string>(key);
        }
 
        std::string GetName()
        { 
            return m_iterImpl->first;
        }
                
        Iterator& operator= (boost::property_tree::ptree::iterator it)
        {
            m_iterImpl = it; return *this; 
        }
        
        void operator++ ()
        { 
            m_iterImpl++;
        }
        
        friend bool operator == (const PropertyTree::Iterator& ita, const PropertyTree::Iterator& itb)
        { 
            return ita.m_iterImpl == itb.m_iterImpl;
        }
        
        friend bool operator != (const PropertyTree::Iterator& ita, const PropertyTree::Iterator& itb)
        { 
            return ita.m_iterImpl != itb.m_iterImpl; 
        }
        
    private: 
        boost::property_tree::ptree::iterator m_iterImpl;
    };
    
    void Init(const std::string& fileName)
    {
        read_xml(fileName, m_treeImpl);
    }
    
    Iterator Begin(const std::string& path = "")
    {
        Iterator it;
        if (path.empty())
            return it = m_treeImpl.begin();
        else return it = m_treeImpl.get_child(path).begin();
    }
    
    Iterator End(const std::string& path = "")
    {
        Iterator it;
        if (path.empty())
            return it = m_treeImpl.end();
        else return it = m_treeImpl.get_child(path).end();
    }
    
    Iterator Find(const std::string& key)
    {
        Iterator it;
        return it = m_treeImpl.to_iterator( m_treeImpl.find(key) );
    }
    
    // throws exception if path is not found
    template <class T>
    T GetData(const std::string& path)
    {
        return m_treeImpl.get<T>(path);
    }
    
    // return default value if path is not found
    template <class T>
    T GetData(const std::string& path, T defaultVal)
    {
        return m_treeImpl.get(path, defaultVal);
    }
    
private:
    friend class Iterator;
    PropertyTree(boost::property_tree::ptree tree) : m_treeImpl(tree) {}

    boost::property_tree::ptree m_treeImpl;
};

} // end Skit

#endif // PROPERTY_TREE_H_



