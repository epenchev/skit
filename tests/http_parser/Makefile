CC=g++
CFLAGS=-I../../core -Wall -O0 -g3
LDFLAGS=-L../../core -lskit -lpthread -lboost_system -lboost_thread
SOURCES=http_parser_test.cpp
EXECUTABLE=http_parser_test

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS)
		
clean: 
	$(RM) $(EXECUTABLE) *.o *~
