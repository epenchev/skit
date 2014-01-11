# All Target
all: FileSourceModule.so

clean:
	rm -rf *.o *.so

# Tool invocations
FileSourceModule.so: FileSourceModule.o FileSource.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -shared -L../../lib -Wl,-soname=FileSourceModule.so -o "FileSourceModule.so" FileSourceModule.o FileSource.o -lblitz
	@echo 'Finished building target: $@'
	@echo ' '
	
FileSourceModule.o: FileSourceModule.cpp
	@echo 'Building target: $@'
	g++ -Wall -I../../ -O0 -Wall -fPIC -c FileSourceModule.cpp -o FileSourceModule.o

FileSource.o : FileSource.cpp FileSource.h
	@echo 'Building target: $@'
	g++ -Wall -I../../ -O0 -Wall -fPIC -c FileSource.cpp -o FileSource.o

