//
// RegFactory.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef REG_FACTORY_TMPL_H_
#define REG_FACTORY_TMPL_H_

#include <map>
#include <set>
#include <string>

// Abstract factory from template, register classes at runtime.
template <typename T>
class RegFactory
{
public:
    typedef T* (*CreateFunc)();
    
    static void Registrate(const std::string& name, RegFactory::CreateFunc func)
    {
        if ( GetRegistry().end() == GetRegistry().find(name) )
        {
            GetRegistry()[name] = func;
        }
    }
    
    static T* CreateInstance(const std::string& name)
    {
        typename std::map<std::string, RegFactory::CreateFunc>::iterator it = GetRegistry().find(name);
        return it == GetRegistry().end() ? NULL : (it->second)();
    }

    static void GetRegList(std::set<std::string>& outlist)
    {
        outlist.clear();
        typename std::map<std::string, RegFactory::CreateFunc>::iterator it;
        for ( it = GetRegistry().begin(); it != GetRegistry().end(); it++ )
        {
            outlist.insert(it->first);
        }
    }

    template <typename D>
    struct Registrator
    {
        Registrator(const std::string& name)
        {
            RegFactory::Registrate(name, D::CreateItem);
        }
    private: // non-copyable
        Registrator(const Registrator&);
        void operator=(const Registrator&);
    };

protected:    
    static std::map<std::string, RegFactory::CreateFunc>& GetRegistry()
    {
        static std::map<std::string, RegFactory::CreateFunc> s_registry;
        return s_registry;
    }
};

template <class Listener, class Reg>
struct ListenerRegistrator
{
	ListenerRegistrator()
    {
		Reg::Register(Listener::CreateListener);
    };
private: // non-copyable
	ListenerRegistrator(const ListenerRegistrator&);
    void operator=(const ListenerRegistrator&);
};

#endif // REG_FACTORY_TMPL_H_


