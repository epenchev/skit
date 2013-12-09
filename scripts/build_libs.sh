#g++  -I../include -O0 -Wall -fPIC -c mod_source_example.cpp -o mod_source_example.o
#g++ -shared -Wl,-soname,libmod_source_example.so -o libmod_source_example.so mod_source_example.o -lc

g++ -I../include -O0 -Wall -fPIC -c http_module.cpp -o http_module.o
g++ -shared -Wl,-soname,libhttp_module.so -o libhttp_module.so http_module.o -lc

g++  -I../include -O3 -Wall -fPIC -g -c ../HTTP/HTTPRequest.cpp -o HTTPRequest.o
g++  -I../include -O3 -Wall -fPIC -g -c ../HTTP/HTTPResponse.cpp -o HTTPResponse.o 
g++  -I../include -O3 -Wall -fPIC -g -c ../HTTP/HTTPUtils.cpp -o HTTPUtils.o
g++  -shared -fPIC -Wl,-soname,libHTTP.so -o libHTTP.so HTTPRequest.o HTTPResponse.o HTTPUtils.o -lc
rm -rf *.o
