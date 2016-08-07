CC=g++
INC=-I/opt/curl/include -I/opt/json/include
LIB=-L/opt/curl/lib -L/opt/json/lib 
all:base64.o http_get.o jpeg_streamer.o
	${CC}  -o jpeg_streamer base64.o http_get.o jpeg_streamer.o ${INC} ${LIB} -lcurl -ljsoncpp -lpthread 
base64.o:
	${CC} -c base64.cpp ${INC}
http_get.o:
	${CC} -c http_get.cpp ${INC}
jpeg_streamer.o:
	${CC} -c jpeg_streamer.cpp ${INC}
clean:
	-rm jpeg_streamer
	-rm *.o
