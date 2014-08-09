CC=g++
CFLAGS=-I../../core -Wall -O0 -g3
LDFLAGS=-lpthread -lboost_system -lboost_thread
OBJECTS=../../core/Task.o
SOURCES=task_test.cpp
EXECUTABLE=task_test

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(OBJECTS) $(LDFLAGS)
		
clean: 
	$(RM) $(EXECUTABLE) *.o *~
