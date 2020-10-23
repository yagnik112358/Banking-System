#include<stdio.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>

#define PORT 8759
#define ERR_EXIT(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

/*struct customer{
	char *name;
	char *dob;
};*/

struct account{
	int type; //0 if individual,1 if joint;
	int accountNumber;
	char customer1[20];
	char customer2[20]; 
	char password[15];
	long balance; 
};
void displayAccountDetails(struct account acc);
int login(int sock);

int newAccount(int sock);

void serviceClient(int sock);

char *ACC[2] = {"./database/accounts/individual", "./database/accounts/joint"};

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1) {
		//if socket creation fails
		printf("socket creation failed\n");
		ERR_EXIT("socket()");
	}
	int optval = 1;
	int optlen = sizeof(optval);
	/*
	to close socket automatically while terminating process
	SOL_SOCKET : to manipulate option at API level o/w specify level
	*/
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, optlen)==-1){
		printf("set socket options failed\n");
		ERR_EXIT("setsockopt()");
	}
	struct sockaddr_in sadr;
	sadr.sin_family = AF_INET;
	sadr.sin_addr.s_addr = htonl(INADDR_ANY);
	sadr.sin_port = htons(PORT);
	/*
	sockaddr_in is from family AF_INET (ip(7)) and varies as per the adress family
	*/
	printf("Binding socket...\n");
	if(bind(sockfd, (struct sockaddr *)&sadr, sizeof(sadr))==-1){
		printf("binding port failed\n");
		ERR_EXIT("bind()");
	}
	//2nd arg : size of backlog
	if(listen(sockfd, 100)==-1){
		printf("listen failed\n");
		ERR_EXIT("listen()");
	}
	printf("Listening...\n");
	while(1){
		int connectedfd;
		if((connectedfd = accept(sockfd, (struct sockaddr *)NULL, NULL))==-1){
			printf("Connection error\n");
			ERR_EXIT("accept()");
		}
		// pthread_t cli;
		if(fork()==0){
			serviceClient(connectedfd);
			exit(1);
		}
	}
	close(sockfd);
	printf("Connection closed!\n");
}
void serviceClient(int sock){
	int func_id;
	printf("Client %d connected\n", sock);
	while(1){
		printf("Reading option\n");
		read(sock, &func_id, sizeof(int));
		printf("Read %d\n",func_id);
		if(func_id==1) {
			login(sock);
		}
		else if(func_id==2) {
			newAccount(sock);
		}
		else { 
			printf("Other choice!\n"); 
			break;
		}
	}
	close(sock);
	printf("Client %d disconnected\n", sock);
}
int login(int sock){
	int type, accNo, fd, valid=1, invalid=0, login_success=0;
	char password[15];
	struct account temp;
	read(sock, &type, sizeof(type));
	if(type == 3) {return 0;}
	read(sock, &accNo, sizeof(accNo));
	read(sock, &password, sizeof(password));	
	
	if((fd = open(ACC[type-1], O_RDWR))==-1)printf("File Error\n");
	
	lseek(fd, (accNo - 202001)*sizeof(struct account), SEEK_CUR);
	struct flock lock;

	lock.l_start = (accNo-1)*sizeof(struct account);
	lock.l_len = sizeof(struct account);
	lock.l_whence = SEEK_SET;
	lock.l_pid = getpid();
	printf("type in server->%d",type);
	
	if(type == 1){
		lock.l_type = F_WRLCK;
		fcntl(fd,F_SETLKW, &lock);
		read(fd, &temp, sizeof(struct account));
		printf("p1->%s\tacc->%d",temp.password,temp.accountNumber);
				
		if(temp.accountNumber == accNo){
			if(!strcmp(temp.password, password)){
				printf("p1->%s\tp2->%s",temp.password, password);
				write(sock, &valid, sizeof(valid));
				//Window after login
				login_success = 1;
			}
		}
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		if(login_success)
		return 3;
	}
	else if(type == 2){
		lock.l_type = F_RDLCK;
		fcntl(fd,F_SETLKW, &lock);
		read(fd, &temp, sizeof(struct account));
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		lock.l_type = F_WRLCK;
		fcntl(fd,F_SETLKW, &lock);
		if(temp.accountNumber == accNo){
			if(!strcmp(temp.password, password)){
				printf("p1->%s\tp2->%s",temp.password, password);
				
				write(sock, &valid, sizeof(valid));
				//Window after login
				login_success =1;
				return 3;
			}
		}
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		if(login_success)
		return 3;
	}
		
		write(sock, &invalid, sizeof(invalid));	
	return 3;	
} 
int newAccount(int sock){
	int type,fd;
	char password[15];
	char customer1[20];
	char customer2[20];
	struct account temp;
	read(sock, &type, sizeof(type));	//1)Individual 2)Joint 3)Back
	if(type == 3){ return 0;}	//when back is choosen
	read(sock, &customer1, sizeof(customer1));
	read(sock, &customer2, sizeof(customer2));
	read(sock, &password, sizeof(password));

	if((fd = open(ACC[type], O_RDWR))==-1){
		printf("File Error\n");
		ERR_EXIT("open()");
	}


	int fp = lseek(fd, 0, SEEK_END);

	if(fp==0){
		temp.accountNumber = 202001;
	}
	else{
		fp = lseek(fd, -1 * sizeof(struct account), SEEK_CUR);
		read(fd, &temp, sizeof(temp));
		temp.accountNumber++;
	}
	strcpy(temp.customer1, customer1);
	strcpy(temp.customer2, customer2);
	strcpy(temp.password, password);
	temp.balance=0;
	temp.type=type;
	write(fd, &temp, sizeof(temp));
	printf("Writing acc number %d\n",temp.accountNumber);
	write(sock, &temp.accountNumber, sizeof(temp.accountNumber));
	close(fd);
	return 3;
}
void displayAccountDetails(struct account acc){
	if((acc).type==0) {
		printf("Individual account\n");
		printf("Name : %s\n",((acc).customer1));
//		printf("Date of birth : %s\n",(acc).c1);
		printf("accountNumber : %d\n",(acc).accountNumber);
		printf("Password : %s\n",(acc).password);
		printf("Balance : %ld\n",(acc).balance);
	}
	else if((acc).type==1){
		printf("Joint account	\n");
		printf("Name:%s\n",((acc).customer1));
		printf("accountNumber : %d\n",(acc).accountNumber);
		//printf("Date of birth :%s\n",((acc).c1).dob);
		printf("Name:%s\n",((acc).customer2));
		//printf("Date of birth :%s\n",((acc).c2).dob);
		printf("Password : %s\n",(acc).password);
		printf("Balance : %ld\n",(acc).balance); 
	}
}
/*void cpy(struct customer c1,struct customer c2){
	
}*/
