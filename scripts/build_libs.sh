g++  -I../include -O3 -Wall -fPIC -g -c ../HTTP/HTTPRequest.cpp -o HTTPRequest.o
g++  -I../include -O3 -Wall -fPIC -g -c ../HTTP/HTTPResponse.cpp -o HTTPResponse.o 
g++  -I../include -O3 -Wall -fPIC -g -c ../HTTP/HTTPUtils.cpp -o HTTPUtils.o
g++  -shared -fPIC -Wl,-soname,libHTTP.so -o libHTTP.so HTTPRequest.o HTTPResponse.o HTTPUtils.o -lc
rm -rf *.o