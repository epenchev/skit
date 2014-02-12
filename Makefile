CPPFLAGS = -I/usr/local/boost/include -I./ -O0 -g3 -Wall -fPIC
LDLIBS = -ldl -lpthread -lboost_system -lboost_thread

all: libblitz.so blitz

objects += $(patsubst %.cpp,%.o,$(wildcard system/*.cpp))
objects += $(patsubst %.cpp,%.o,$(wildcard stream/*.cpp))
objects += $(patsubst %.cpp,%.o,$(wildcard server/*.cpp))
objects += $(patsubst %.cpp,%.o,$(wildcard HTTP/*.cpp))
objects += $(patsubst %.cpp,%.o,$(wildcard utils/*.cpp))

clean:
	rm -f $(objects) lib/libblitz.so main/blitz

libblitz.so: $(objects)
	g++ -shared -L/usr/local/boost/lib -Wl,-soname=libblitz.so -o lib/$@ $(objects) $(LDLIBS)

blitz: libblitz.so main/Blitz.cpp
	g++ -I./ -I/usr/local/boost/include -Llib/ -O0 -g3 -Wall main/Blitz.cpp -o main/$@ -lblitz
