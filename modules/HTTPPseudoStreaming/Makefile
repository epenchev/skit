# All Target
all: HTTPPseudoStreamModule.so

clean:
	rm -rf *.o *.so

# Tool invocations
HTTPPseudoStreamModule.so: HTTPPseudoStreamModule.o HTTPPseudoStream.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -shared -L../../lib -Wl,-soname=HTTPPseudoStreamModule.so -o "HTTPPseudoStreamModule.so" HTTPPseudoStreamModule.o HTTPPseudoStream.o -lblitz
	@echo 'Finished building target: $@'
	@echo ' '
	
HTTPPseudoStreamModule.o: HTTPPseudoStreamModule.cpp
	@echo 'Building target: $@'
	g++ -Wall -I../../ -O0 -Wall -fPIC -c HTTPPseudoStreamModule.cpp -o HTTPPseudoStreamModule.o

HTTPPseudoStream.o : HTTPPseudoStream.cpp HTTPPseudoStream.h
	@echo 'Building target: $@'
	g++ -Wall -I../../ -O0 -Wall -fPIC -c HTTPPseudoStream.cpp -o HTTPPseudoStream.o

