#include <iostream>
#include <PropertyTree.h>
#include <Logger.h>

using namespace std;

int main()
{
    try 
    {
        PropertyTree config;
        config.Init("skit_conf.xml");
        //config.Init("example.xml");
        
        string logfileName = config.GetData<string>("logfile");
        if (!logfileName.empty())
        {
            cout << "logfile : " << logfileName << endl;
        }
        else
        {
            LOG(logWARNING) << "logfile is empty !!!";
        }
        
        string pidfileName = config.GetData<string>("pidfile");
        if (!pidfileName.empty())
        {
            cout << "pidfile : " << pidfileName << endl;
        }
        else
        {
            LOG(logWARNING) << "pidfile is empty !!!";
        }
        
        int threads = config.GetData<int>("threads");
        cout << "threads :" << threads << endl;
        
        string runAsDaemon = config.GetData<string>("daemon");
        if (!runAsDaemon.empty())
        {
            cout << "runAsDaemon :" << runAsDaemon << endl;
        }
        else
        {
            LOG(logWARNING) << "daemon is empty !!!";
        }
        
        for ( PropertyTree::Iterator it = config.Begin("admin"); it != config.End("admin"); ++it )
        {
            cout << it.GetName() << ":" << it.GetData() << endl;
        }
        
        for ( PropertyTree::Iterator it = config.Begin("servers"); it != config.End("servers"); ++it )
        {
            cout << it.GetName() << ":";
            cout << it.GetData("name") << endl;
            cout << it.GetData("listen") << endl;
            if ( it.GetData("name") == "rtmp")
            {
                PropertyTree tree = it.GetTree("options");
                cout << "Options ----- \n";
                for ( PropertyTree::Iterator itOpt = tree.Begin(); itOpt != tree.End(); ++itOpt )
                {
                    cout << itOpt.GetName() << ":";
                    cout << itOpt.GetData() << endl;
                }
                cout << "Options ----- \n";
            }
        }

        for ( PropertyTree::Iterator it = config.Begin("streams"); it != config.End("streams"); ++it )
        {
            cout << it.GetName() << ":";
            cout << it.GetData("name") << endl;
            cout << it.GetData("source") << endl;
        }
    }
    catch (exception& ex)
    {
        cout << " Exception: " << ex.what() << endl;
    }
    
    return 0;
}


