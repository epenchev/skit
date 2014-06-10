#include <iostream>
#include <PropertyTree.h>
#include <Logger.h>

int main()
{
    try 
    {
        Skit::PropertyTree config;      
        config.Init("skit_conf.xml");
        
        std::string logfileName = config.GetData<std::string>("logfile");
        if (!logfileName.empty())
            std::cout << "logfile : " << logfileName << std::endl;
        else LOG(logWARNING) << "logfile is empty !!!";
        
        std::string pidfileName = config.GetData<std::string>("pidfile");
        if (!pidfileName.empty())
            std::cout << "pidfile : " << pidfileName << std::endl;
        else LOG(logWARNING) << "pidfile is empty !!!";
        
        int threads = config.GetData<int>("threads");
        std::cout << "threads :" << threads << std::endl;
        
        std::string runAsDaemon = config.GetData<std::string>("daemon");
        if (!runAsDaemon.empty())
            std::cout << "runAsDaemon :" << runAsDaemon << std::endl;
        else LOG(logWARNING) << "daemon is empty !!!";
        
        for ( Skit::PropertyTree::Iterator it = config.Begin("admin"); it != config.End("admin"); ++it )
        {
            std::cout << it.GetName() << ":" << it.GetData() << std::endl;
        }
        
        for ( Skit::PropertyTree::Iterator it = config.Begin("servers"); it != config.End("servers"); ++it )
        {
            std::cout << it.GetName() << ":";
            std::cout << it.GetData("name") << std::endl;
            std::cout << it.GetData("listen") << std::endl;
            if ( it.GetData("name") == "rtmp")
            {
                Skit::PropertyTree tree = it.GetTree("options");
                for ( Skit::PropertyTree::Iterator itOpt = tree.Begin(); itOpt != tree.End(); ++itOpt )
                {
                    std::cout << itOpt.GetName() << ":";
                    std::cout << itOpt.GetData() << std::endl;
                }
            }
        }

        for ( Skit::PropertyTree::Iterator it = config.Begin("streams"); it != config.End("streams"); ++it )
        {
            std::cout << it.GetName() << ":";
            std::cout << it.GetData("name") << std::endl;
            std::cout << it.GetData("source") << std::endl;
        }
    }
    catch (std::exception& ex)
    {
        std::cout << " Exception: " << ex.what() << std::endl;
    }
    
    return 0;
}


