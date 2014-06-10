CPPFLAGS = -I./ -O0 -g3 -Wall -fPIC
LDLIBS = -ldl -lpthread -lboost_system -lboost_thread

all: libskit.so Skit

objects += $(patsubst %.cpp, %.o, $(filter-out Skit.cpp, $(wildcard *.cpp)))

clean:
	rm -f $(objects) libskit.so Skit *~

libskit.so: $(objects)
	g++ -shared -Wl,-soname=libskit.so -o $@ $(objects)

Skit: libskit.so Skit.cpp
	g++ -I./ -L./ -O0 -g3 -Wall Skit.cpp -o $@ -lskit $(LDLIBS)
