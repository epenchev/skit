CPPFLAGS = -O0 -g3 -Wall
LDLIBS = -lpthread -lboost_system -lboost_thread #-lboost_iostreams

all: Skit

objects += $(patsubst %.cpp, %.o, $(filter-out Skit.cpp, $(wildcard *.cpp)))
object_modules = $(wildcard ../modules/*/*.o)

clean:
	rm -f $(objects) Skit *~

Skit: $(objects) $(object_modules) Skit.cpp
	g++ -O0 -g3 -Wall Skit.cpp $(objects) $(object_modules) -o $@ $(LDLIBS)

