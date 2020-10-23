#include<stdio.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
struct sample{
	int value;
};

int main(char *argv[]){
	struct sample temp;
	(temp).value=9;
	
	struct sockaddr_in serv,cli;
	int s,sz,nsd;
	char buff[80];
	s=socket(AF_UNIX,SOCK_STREAM,0);
	serv.sin_family=AF_UNIX;
	serv.sin_addr.s_addr=INADDR_ANY;
	serv.sin_port=htons(7512);
	bind(s,(void *)(&serv),sizeof(serv));
	listen(s,5);
	sz=sizeof(cli);
	nsd=accept(s,(void *)(&cli),&sz);
	read(nsd,buff,sizeof(buff));
	printf("message from client :%s\n",buff);
	send(nsd , "Server e mokalyu" , strlen("Server e mokalyu") , 0 ); 
	close(s);
	//write(nsd,"RESPONSE FROM SERVER\n",sizeof("RESPONSE FROM SERVER\n"));
	
} 
