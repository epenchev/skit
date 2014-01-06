# All Target
all: mod_http_example.so

clean:
	rm -rf *.o *.so

# Tool invocations
mod_http_example.so: mod_http_example.o HTTPPseudoStream.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -shared -L../../Library -Wl,-soname=mod_http_example.so -o "mod_http_example.so" mod_http_example.o HTTPPseudoStream.o -lblitz
	@echo 'Finished building target: $@'
	@echo ' '
	
mod_http_example.o: mod_http_example.cpp
	@echo 'Building target: $@'
	g++ -Wall -I../../ -O0 -Wall -fPIC -c mod_http_example.cpp -o mod_http_example.o

HTTPPseudoStream.o : HTTPPseudoStream.cpp HTTPPseudoStream.h
	@echo 'Building target: $@'
	g++ -Wall -I../../ -O0 -Wall -fPIC -c HTTPPseudoStream.cpp -o HTTPPseudoStream.o

