objects += $(patsubst %.cpp, %.o, $(wildcard */*.cpp))
CPPFLAGS = -I../core -O0 -g3 -Wall

all: $(objects)

clean:
	rm -f $(objects) *~
