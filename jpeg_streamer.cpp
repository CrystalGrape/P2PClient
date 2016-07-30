#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include <json/json.h>

#include "http_get.h"
#include "base64.h"

using namespace Json;
//数据包类型
#define P2P_HEART		0x7001	//心跳包
#define P2P_REQUEST		0x7002	//连接请求包
#define P2P_RST			0x7003	//回复包
#define P2P_STREAM		0x7004	//视频流包
#define P2P_ONLINE		0x7005	//上线请求
//错误类型
#define P2P_ERR_TIMEOUT		0x6001	//超时 
#define P2P_ERR_PKGWRONG	0x6002	//数据包解析错误

#define MAX_BURST_SIZE			(1024)

//发送器 
class Sender
{
public:
	int NewSender(struct image_buffer_t image);
	int Send(int innerIndex, string &out);
	int GetInnerIndex(){return MaxInnerIndex;}
private:
	string base64_code;		//待发送的base64图片
	int MaxInnerIndex;
};

int Sender::NewSender(struct image_buffer_t image)
{
	base64_code = base64_encode(
	(unsigned char *)image.image_data
	,image.length);
	MaxInnerIndex = base64_code.length()/MAX_BURST_SIZE + 1;
	return 0;
}

int Sender::Send(int innerIndex, string &out)
{
	if(innerIndex > MaxInnerIndex)
		return 1;
	//如果是最后一个数据包
	if(innerIndex == MaxInnerIndex){
		out = base64_code.substr((innerIndex-1)*MAX_BURST_SIZE, base64_code.length());
	}else{
		out = base64_code.substr((innerIndex-1)*MAX_BURST_SIZE, innerIndex*MAX_BURST_SIZE);
	}
	return 0;
}

ImageList image_buffer;
void SendImage(int innerIndex, Sender sender, int socket, struct sockaddr_in dest_addr);
void SetJpegStreamer();
int main()
{
	
	int listen;
	char buffer[1024];
	struct sockaddr_in listen_addr;
	struct sockaddr_in client_addr;
	int slen  = sizeof(client_addr);	
	Sender sender;
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(12111);
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if((listen = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		return -1;
	if(bind(listen, (struct sockaddr *)&listen_addr,sizeof(listen_addr)) < 0)
		return -1;
	pthread_mutex_init(&buffer_lock,NULL);
	SetJpegStreamer();
	while(1){
		int len = recvfrom(listen, buffer, 1024, 0, (struct sockaddr*)&client_addr, (socklen_t *)&slen);
		if(len <= 0){
			continue;
		}
		buffer[len] = '\0';
		Reader reader;
		Value root;
		if(!reader.parse(buffer,root))
			continue;
		if(root["head"]["pkgtype"] == P2P_RST){
			if(root["head"]["index"] == 1){
				//图片发送成功，发送下一张图片
				pthread_mutex_lock(&buffer_lock);
				struct image_buffer_t image = image_buffer.front();
				image_buffer.pop_front();
				pthread_mutex_unlock(&buffer_lock);
				sender.NewSender(image);
				SendImage(1, sender, listen, client_addr);
				/*
				for(int i = 1; i < sender.GetInnerIndex(); i++){
					SendImage(i, sender, listen, client_addr);
					usleep(1000);
				}*/
			}
			else{
				//存在丢包，重发
				Value array = root["playload"]["miss"];
				for(int i = 0;i < root["playload"]["missNum"].asInt(); i++){
					SendImage(array[i].asInt(), sender, listen, client_addr);
				}
			}
		}
	}
	return 0;
}


void SendImage(int innerIndex, Sender sender, int socket, struct sockaddr_in dest_addr)
{
	FastWriter writer;
	Value head;
	Value playload;
	string content;
	
	head["pkgtype"] = P2P_STREAM;
	head["srcid"] = "12345";
	head["index"] = (sender.GetInnerIndex() == innerIndex)?1:0;
	head["sendtime"] = 0;
	
	playload["total"] = sender.GetInnerIndex();
	playload["name"] = "stream.jpg";
	playload["section"] = innerIndex;
	if(sender.Send(innerIndex, content) > 0)
		return;
	playload["content"] = content;
	Value pkg;
	pkg["head"] = head;
	pkg["playload"] = playload;
	string json_str=writer.write(pkg);
	
	sendto(socket, json_str.c_str(), json_str.size(), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

void TimerOut(int signo)
{
	start_http_streamer(image_buffer);
	signal(SIGALRM, TimerOut);
	struct itimerval new_value,old_value;
	new_value.it_interval.tv_sec = 0;
	new_value.it_interval.tv_usec = 100000;
	new_value.it_value.tv_sec = 0;
	new_value.it_value.tv_usec = 100000;
	setitimer(ITIMER_REAL, &new_value, &old_value);
}
void SetJpegStreamer()
{
	signal(SIGALRM, TimerOut);
	struct itimerval new_value,old_value;
	new_value.it_interval.tv_sec = 0;
	new_value.it_interval.tv_usec = 100000;
	new_value.it_value.tv_sec = 0;
	new_value.it_value.tv_usec = 100000;
	setitimer(ITIMER_REAL, &new_value, &old_value);
}


