//
// PropertyTree.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef PROPERTY_TREE_H_
#define PROPERTY_TREE_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

using namespace std;

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
            _iterImpl = tree.begin();
        }
        
        // Return a subtree from the given key
        PropertyTree GetTree(const string& path = "")
        {
            PropertyTree tree;
            if (path.empty())
                tree._treeImpl = _iterImpl->second;
            else
                tree._treeImpl = _iterImpl->second.get_child(path);

            return tree;
        }
        
        // throws exception if key is not found
        template <class T>
        T inline GetData(const string& key = "")
        {
            if (key.empty())
                return boost::lexical_cast<T>( _iterImpl->second.data() );

            return _iterImpl->second.get<T>(key);
        }
        
        // return default value if path is not found
        template <class T>
        T inline GetData(const string& key, T defaultVal)
        {
            return _iterImpl->second.get<T>(key, defaultVal);
        }

        /*
        string GetData(const string& key = "")
        { 
            return GetData<string>(key);
        }*/
 
        // return default value if path is not found
        string GetData(const string& key, const string defaultVal = "")
        {
            return GetData<string>(key, defaultVal);
        }

        string GetName()
        { 
            return _iterImpl->first;
        }
                
        Iterator& operator= (boost::property_tree::ptree::iterator it)
        {
            _iterImpl = it;
            return *this;
        }
        
        void operator++ ()
        { 
            _iterImpl++;
        }
        
        friend bool operator == (const PropertyTree::Iterator& ita, const PropertyTree::Iterator& itb)
        { 
            return ita._iterImpl == itb._iterImpl;
        }
        
        friend bool operator != (const PropertyTree::Iterator& ita, const PropertyTree::Iterator& itb)
        { 
            return ita._iterImpl != itb._iterImpl;
        }
        
    private: 
        boost::property_tree::ptree::iterator _iterImpl;
    };
    
    void Init(const string& fileName)
    {
        read_xml(fileName, _treeImpl);
    }
    
    // throws exception if path is not found
    Iterator Begin(const string& path = "")
    {
        Iterator it;
        if (path.empty())
            return it = _treeImpl.begin();

        return it = _treeImpl.get_child(path).begin();
    }
    
    // throws exception if path is not found
    Iterator End(const string& path = "")
    {
        Iterator it;
        if (path.empty())
            return it = _treeImpl.end();

        return it = _treeImpl.get_child(path).end();
    }
    
    Iterator Find(const string& key)
    {
        Iterator it;
        return it = _treeImpl.to_iterator( _treeImpl.find(key) );
    }
    
    // throws exception if path is not found
    template <class T>
    T inline GetData(const string& path)
    {
        //return _treeImpl.get_v
        return _treeImpl.get<T>(path);
    }
    
    // throws exception if path is not found
    string GetData(const string& path)
    {
        return GetData<string>(path);
    }

    // return default value if path is not found
    template <class T>
    T inline GetData(const string& path, T defaultVal)
    {
        return _treeImpl.get(path, defaultVal);
    }
    
    // return default value if path is not found
    string GetData(const string& path, const string defaultVal = "")
    {
        return GetData<string>(path, defaultVal);
    }

private:
    friend class Iterator;
    PropertyTree(boost::property_tree::ptree tree) : _treeImpl(tree) {}

    boost::property_tree::ptree _treeImpl;
};

#endif // PROPERTY_TREE_H_



