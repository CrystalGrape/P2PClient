#ifndef http_get_h
#define http_get_h
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <ctype.h>
#include <iconv.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <list>
#include "smart_lock.h"
using namespace std;

struct image_buffer_t
{
	char image_data[1024*100];					//20kb缓冲区
	int length;								//图片长度
};
typedef list<struct image_buffer_t> ImageList;
void start_http_streamer(ImageList &imglist);
#endif
