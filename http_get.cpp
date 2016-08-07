#include "http_get.h"

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    int len = size * nmemb;
    int written = len;
    struct image_buffer_t *image = (struct image_buffer_t *)stream;
    int buffer_len=image->length;
    memcpy(image->image_data+buffer_len, ptr, len);
	image->length+=len;
    return written;
}

void http_get(const char* url, struct image_buffer_t *image) {
    CURL *curl;
    curl = curl_easy_init();
    if (curl) {  	
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)image);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}


void start_http_streamer(ImageList &imglist)
{
	struct image_buffer_t image;
	memset(&image, 0, sizeof(image));
	//http_get("http://127.0.0.1:8080/?action=snapshot", &image);
	http_get("http://127.0.0.1/a.jpeg", &image);
	SmartLock lock(&buffer_lock);
	if(imglist.size() >= 20){
		imglist.pop_front();
	}
	imglist.push_back(image);
}

/*
int main()
{
	ImageList userlist;
	start_http_streamer(userlist);
	struct image_buffer_t image = userlist.front();
	userlist.pop_front();
	FILE *fp = fopen("a.jpg","ab");
	fwrite(image.image_data,image.length,1,fp);
	fclose(fp);
	return 0;
}*/
