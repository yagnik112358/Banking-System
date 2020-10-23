#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
struct sample{
	int value;
};
int main()
{	
	struct sample temp;
	(temp).value=9;
	void *ptr = &temp;
	struct sockaddr_in serv,cli;
	int s;
	char buff[80];
	char *hello="kem che baka server";
	s=socket(AF_UNIX,SOCK_STREAM,0);
	serv.sin_family=AF_UNIX;
	serv.sin_addr.s_addr=INADDR_ANY;
	serv.sin_port=htons(7512);
	connect(s,(void *)(&serv),sizeof(serv));
	send(s , (char*)ptr , sizeof(temp) , 0 ); 
	//write(s,"hi from client\n",sizeof("hi from client\n"));
	read(s,buff,sizeof(buff));
	printf("message from server :%s\n",buff);
	
	close(s);
}
