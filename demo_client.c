#include "demo_client.h"

void usage(const char* msg)
{
	printf("usage : %s [ip][port]",msg);
} 

int main(int argc,char* argv[])
{
	if(argc != 3){
		usage(argv[0]);
		exit(1);
	}
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1){
		perror("socket");
	}

	int port = atoi(argv[2]);
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = inet_addr(argv[1]);
	
	if(connect(sock,(struct sockaddr*)&remote,sizeof(remote)) < 0){
		perror("connect");
		exit(2);
	}

	const char *cmd = argv[1];
	int len = strlen(cmd);
	ssize_t size = send(sock,cmd,len,0);
	if(size <= 0){
		perror("send");
		close(sock);
		return 1;
	}
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	while(recv(sock,buf,sizeof(buf)-1,0) > 0){
		printf("%s\n",buf);
		memset(buf,'\0',sizeof(buf)-1);
	}
	printf("\n");
	close(sock);
	return 0;
}
