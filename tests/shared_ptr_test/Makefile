# All Target
all: shared_ptr_test weak_ptr_test

weak_ptr_test : weak_ptr_test.cpp
	@echo 'Building target: $@'
	g++ -Wall -I/usr/local/boost/include -L/usr/lib -O0 -g3 weak_ptr_test.cpp -o weak_ptr_test -lboost_system

clean:
	rm -rf *.o *.so

shared_ptr_test : shared_ptr_test.cpp
	@echo 'Building target: $@'
	g++ -Wall -I/usr/local/boost/include -I../../ -L../../lib -L/usr/lib -O0 -g3 shared_ptr_test.cpp -o shared_ptr_test -lblitz -lboost_system
