# All Target
all: HTTPPseudoStreamModule.so

clean:
	rm -rf *.o *.so

# Tool invocations
HTTPPseudoStreamModule.so: HTTPPseudoStreamModule.o HTTPPseudoStreamDispatcher.o HTTPPseudoStreamSink.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -shared -L../../lib -L/usr/local/boost/lib -Wl,-soname=HTTPPseudoStreamModule.so -o "HTTPPseudoStreamModule.so" HTTPPseudoStreamModule.o HTTPPseudoStreamDispatcher.o HTTPPseudoStreamSink.o -lblitz
	@echo 'Finished building target: $@'
	@echo ' '
	
HTTPPseudoStreamModule.o: HTTPPseudoStreamModule.cpp
	@echo 'Building target: $@'
	g++ -Wall -I../../ -I/usr/local/boost/include -O0 -g3 -Wall -fPIC -c HTTPPseudoStreamModule.cpp -o HTTPPseudoStreamModule.o

HTTPPseudoStreamDispatcher.o : HTTPPseudoStreamDispatcher.cpp HTTPPseudoStreamDispatcher.h
	@echo 'Building target: $@'
	g++ -Wall -I../../ -I/usr/local/boost/include  -O0 -g3 -Wall -fPIC -c HTTPPseudoStreamDispatcher.cpp -o HTTPPseudoStreamDispatcher.o

HTTPPseudoStreamSink.o : HTTPPseudoStreamSink.cpp HTTPPseudoStreamSink.h
	@echo 'Building target: $@'
	g++ -Wall -I/usr/local/boost/include -I../../ -O0 -g3 -Wall -fPIC -c HTTPPseudoStreamSink.cpp -o HTTPPseudoStreamSink.o
