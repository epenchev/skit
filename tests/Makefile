# All Target
all: freader_test

clean:
	rm -rf *.o *.so freader_test

freader_test : FileReaderTest.cpp
	@echo 'Building target: $@'
	g++ -Wall -I/usr/local/boost/include -I../ -L../lib -L/usr/lib -O0 -g3 -Wall FileReaderTest.cpp -o freader_test -lblitz -lboost_system
