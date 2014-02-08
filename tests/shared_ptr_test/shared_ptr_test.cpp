/*
 * shared_ptr_test.cpp
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
 *  Created on: Jan 29, 2014
 *      Author: emo
 */

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <system/Task.h>

class TestObjectToDel
{
public:
	TestObjectToDel() { std::cout << "TestObjectToDel created \n"; }
	virtual ~TestObjectToDel() { std::cout << "TestObjectToDel deleted \n"; }

	void TestFunc() { std::cout << "TestFunc() from TestObjectToDel \n"; }
private:
};

class TestObjectToDelDerived : public TestObjectToDel
{
public:
	TestObjectToDelDerived() { std::cout << "TestObjectToDelDerived created \n"; }
	virtual ~TestObjectToDelDerived() { std::cout << "TestObjectToDelDerived deleted \n"; }

	void AnotherTestFunc() { std::cout << "AnotherTestFunc() from TestObjectToDelDerived \n"; }
private:
};

typedef boost::shared_ptr<TestObjectToDel> TestObjectToDelPtr;
typedef boost::shared_ptr<TestObjectToDelDerived> TestObjectToDelDerivedPtr;

class Master
{
public:
	Master() { std::cout << "master create\n"; }
	Master(const Master& m)
	{
		std::cout << "master copy \n";
	}
	virtual ~Master() { std::cout << "master destroy \n"; }

	void FuncTestObjectToDelAsParam(TestObjectToDelPtr ptr)
	{
		ptr->TestFunc();
	}


	void FuncTestObjectToDelDerivedAsParam(TestObjectToDelDerivedPtr ptr)
	{
		ptr->AnotherTestFunc();
	}

};



#include <boost/enable_shared_from_this.hpp>
class Packet
{
public:
	Packet() { std::cout << "Packet created \n"; }
	virtual ~Packet() { std::cout << "Packet destroyed \n"; }
};

typedef boost::shared_ptr<Packet> PacketPtr;

class DerivedPacket : public Packet
{
public:
	DerivedPacket() { std::cout << "DerivedPacket created \n"; }
	virtual ~DerivedPacket() { std::cout << "DerivedPacket destroyed \n"; }

	void DerivedFunc() { std::cout << "DerivedPacket Func() \n"; }
};

class AMFPacket : public DerivedPacket
{
public:
	AMFPacket() { std::cout << "AMFPacket created \n"; }
	virtual ~AMFPacket() { std::cout << "AMFPacket destroyed \n"; }

	void AMFPacketFuc() { std::cout << "AMFPacket Func() \n"; }
};

int main(int argc, char* argv[])
{
	PacketPtr pptr(new AMFPacket());
	DerivedPacket* pderived = dynamic_cast<DerivedPacket*>(pptr.get());
	pderived->DerivedFunc();
	AMFPacket* pamf = dynamic_cast<AMFPacket*>(pptr.get());
	pamf->AMFPacketFuc();

#if 0
	Master m;
	//TestObjectToDelDerivedPtr ptr(new TestObjectToDelDerived());
	TestObjectToDelPtr ptr(new TestObjectToDelDerived());
	//TestObjectToDelDerivedPtr ptr_derived = dynamic_cast<TestObjectToDelDerivedPtr>(ptr);
	//ptr->TestFunc();
	TestObjectToDelDerived* derived = dynamic_cast<TestObjectToDelDerived*>(ptr.get());

	if (derived)
	{
		//derived->AnotherTestFunc();
	}
	//ptr.reset();
	delete derived;
	if (ptr)
		ptr->TestFunc();

	//m.FuncTestObjectToDelAsParam(ptr);
	//Task* sampleTask = new Task();
	//sampleTask->Connect(&Master::FuncTestObjectToDelAsParam, &m, ptr);
	//sampleTask->Run();


	//ptr_derived->AnotherTestFunc();
#endif
	return 0;
}
