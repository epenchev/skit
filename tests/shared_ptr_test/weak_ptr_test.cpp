/*
 * weak_ptr_test.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: emo
 */

#include <list>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class MyListener
{
public:
	void notify() { std::cout << "notify from listener \n"; }
};

typedef boost::shared_ptr<MyListener> MyListenerPtr;
typedef boost::weak_ptr<MyListener> MyListenerWeakPtr;

class SimpleListenerManagement
{
public:
	void addListener(MyListenerPtr listener)
	{
		MyListenerWeakPtr wptr(listener);
		listeners_.push_back(wptr);
	}
    void removeListener(MyListenerPtr listener) {}
    void notifyListeners();
private:
      std::list<MyListenerWeakPtr> listeners_; // using weak_ptr
};

void SimpleListenerManagement::notifyListeners()
{
	std::list<MyListenerWeakPtr>::iterator iter = listeners_.begin();
	while(iter != listeners_.end())
	{
		if ((*iter).expired())
        {
			std::cout << "Warning invalid listener !!!\n";
			iter = listeners_.erase(iter);
        }
        else
        {
        	MyListenerPtr listener = (*iter).lock(); // create a shared_ptr from the weak_ptr
            listener->notify();
            ++iter;
        }
	}
}

int main(int argc, char* argv[])
{
	MyListenerPtr l(new MyListener());
	SimpleListenerManagement mng;

	mng.addListener(l);
	MyListener* lp = l.get();
	delete lp;
	lp = NULL;
	mng.notifyListeners();

	return 0;
}
