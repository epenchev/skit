CC=g++
CFLAGS=-I../../core -Wall -O0 -g3
LDFLAGS=-lpthread -lboost_system -lboost_thread
SOURCES=buffer_test.cpp
EXECUTABLE=buffer_test

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS)
		
clean: 
	$(RM) $(EXECUTABLE) *.o *~
