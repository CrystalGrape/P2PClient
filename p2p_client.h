#ifndef p2p_client_h
#define p2p_client_h
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
class P2PClient
{
public:
	static P2PClient *GetInstance(const char *userid, int socket)
	{
		static P2PClient *instance=new P2PClient(userid, socket);
		return instance;
	}
	void Login()
	{
		char login[500];
		sprintf(login,"{\"head\":{\"srcid\":\"%s\",\"pkgtype\":%d,\"sendtime\":0,\"index\":0},\"playload\":{}}",uid,0x7006);
		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr("192.168.1.2");
		server_addr.sin_port = htons(8000);
		sendto(s, login, strlen(login),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	}
	void Heart()
	{
		char login[500];
		sprintf(login,"{\"head\":{\"srcid\":\"%s\",\"pkgtype\":%d,\"sendtime\":0,\"index\":0},\"playload\":{}}",uid,0x7001);
		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr("192.168.1.2");
		server_addr.sin_port = htons(8000);
		sendto(s, login, strlen(login),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	}
private:
	P2PClient(const char *userid, int socket)
	{
		s=socket;
		strcpy(uid,userid);
	};
	P2PClient(const P2PClient &um){};
	P2PClient &operator=(const P2PClient &um){};
	int s;
	char uid[32];
};
#endif
