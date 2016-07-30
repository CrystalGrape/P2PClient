all:base64.o http_get.o jpeg_streamer.o
	g++ -g -o jpeg_streamer base64.o http_get.o jpeg_streamer.o -lcurl -ljsoncpp -lpthread
base64.o:
	g++ -g -c base64.cpp
http_get.o:
	g++ -g -c http_get.cpp
jpeg_streamer.o:
	g++ -g -c jpeg_streamer.cpp
clean:
	-rm jpeg_streamer
	-rm *.o
