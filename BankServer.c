#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "structures.h"

#define PORT 8759
#define ERR_EXIT(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

int login(int sock);

int newAccount(int sock);
int adminOperation(int sock);

void serviceClient(int sock);
int performClientOperation(int sock,int accNo,int type);
char *ACC[2] = {"./database/accounts/individual", "./database/accounts/joint"};
char *CUS[2] = {"./database/customers", "./database/customers"};

int main(){
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
	return 0;
}
void serviceClient(int sock){
	int func_id;
	int option;
	printf("Client %d connected\n", sock);
	while(1){
		printf("Reading option\n");
		read(sock, &option, sizeof(int));
		printf("Read %d\n",option);
		if(option==3) {
			//admin menu
			while(adminOperation(sock)!=4);
		}
		else if(option==2 || option==1) {
			read(sock, &func_id, sizeof(func_id));
			if(func_id==1)
				login(sock);
			if(func_id==2) 
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
int adminOperation(int sock){
	
	int type,option, accNo, fd,fp,success=0;
	struct account temp;
	
		read(sock,&option,sizeof(int));	
		
		if(option ==1){
			read(sock, &type, sizeof(type));	
			read(sock, &accNo, sizeof(accNo));
			while(performClientOperation(sock,accNo,type)!=5);
		}
		if(option ==2){
			newAccount(sock);
		}
		if(option ==3){
		
			read(sock, &type, sizeof(type));	
			read(sock, &accNo, sizeof(accNo));

			if((fd = open(ACC[type-1], O_RDWR))==-1)printf("File Error\n");
			
			lseek(fd, (accNo - 202001)*sizeof(struct account), SEEK_CUR);

			//fp = lseek(fd, -1 * sizeof(struct account), SEEK_CUR);
				temp.type=0;
				temp.accountNumber=0;
				strcpy(temp.password,"-");
				temp.balance=0;
				write(fd, &temp, sizeof(temp));
				success =1;
				write(sock, &success, sizeof(success));
		}
		return option;
}
int login(int sock){
	int type, accNo, fd, valid=1, invalid=0, login_success=0;
	char password[15];
	struct account temp;
	read(sock, &type, sizeof(type));
	if(type == 3) {return 0;}
	read(sock, &accNo, sizeof(accNo));
	read(sock, &password, sizeof(password));	
	
	//printf("accno read from client  = %d password = %s",accNo,password);
	
	if((fd = open(ACC[type-1], O_RDWR))==-1)printf("File Error\n");
	
	lseek(fd, (accNo - 202001)*sizeof(struct account), SEEK_CUR);
	struct flock lock;

	lock.l_start = (accNo-202001)*sizeof(struct account);
	lock.l_len = sizeof(struct account);
	lock.l_whence = SEEK_SET;
	lock.l_pid = getpid();
	//printf("type in server->%d",type);
	
	//if(type == 1){
	//	printf("inside type 1");
		lock.l_type = F_WRLCK;
		fcntl(fd,F_SETLKW, &lock);
		read(fd, &temp, sizeof(struct account));
		//printf("pas1->%s\tacc->%d passcli = %s , strcpy =%d",temp.password,temp.accountNumber,password,strcmp(temp.password, password));
		if(temp.accountNumber == accNo){
			if(!strcmp(temp.password, password)){
				//printf("p1->%s\tp2->%s",temp.password, password);
				write(sock, &valid, sizeof(valid));
				//Window after login
				 while(performClientOperation(sock,accNo,type)!=6);
				login_success = 1;
			}
		}
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		if(login_success)
			return 3;
	//}
	/*else if(type == 2){
	
		//printf("inside type 1");
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
				//printf("p1->%s\tp2->%s",temp.password, password);
				
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
	*/
		
	write(sock, &invalid, sizeof(invalid));	
	return 3;	
} 
int newAccount(int sock){
	int type,fd,fd2;
	char password[15];
	char name[20];
	char name2[20];
	struct account temp;
	struct customer c,c1;
	read(sock, &type, sizeof(type));	//1)Individual 2)Joint 3)Back
	if(type == 3){ return 0;}	//when back is choosen
	if(type == 1){ 
				read(sock, &name, sizeof(name));
				read(sock, &password, sizeof(password));
				if((fd = open(ACC[type-1], O_RDWR))==-1){
					printf("File Error\n");
					ERR_EXIT("open()");
				}
				if((fd2 = open(CUS[type-1], O_RDWR))==-1){
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
				int fp1 = lseek(fd2, 0, SEEK_END);
				
				if(fp1==0){
					c.id = 1;
				}
				else{
					fp1 = lseek(fd2, -1 * sizeof(struct customer), SEEK_CUR);
					read(fd2, &c, sizeof(c));
					c.id++;
				}
				c.accountNumber = temp.accountNumber;
				strcpy(c.name,name); 
				write(fd2, &c, sizeof(c));
				strcpy(temp.password, password);
				temp.balance=0;
				temp.type=type;
				write(fd, &temp, sizeof(temp));
				//printf("Writing acc number %d\n",temp.accountNumber);
				write(sock, &temp.accountNumber, sizeof(temp.accountNumber));
				write(sock, &c.id, sizeof(c.id));
				close(fd);
				close(fd2);	
	}
	if(type == 2){ 
				read(sock, &name, sizeof(name));
				read(sock, &name2, sizeof(name2));
				read(sock, &password, sizeof(password));
				if((fd = open(ACC[type-1], O_RDWR))==-1){
					printf("File Error\n");
					ERR_EXIT("open()");
				}
				if((fd2 = open(CUS[type-1], O_RDWR))==-1){
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
				int fp1 = lseek(fd2, 0, SEEK_END);
				
				if(fp1==0){
					c.id = 1;
				}
				else{
					fp1 = lseek(fd2, -1 * sizeof(struct customer), SEEK_CUR);
					read(fd2, &c, sizeof(c));
					c.id++;
				}
				c.accountNumber = temp.accountNumber;
				strcpy(c.name,name); 
				write(fd2, &c, sizeof(c));
				//fp1 = lseek(fd2, 0, SEEK_END);
				
				fp1 = lseek(fd2, -1 * sizeof(struct customer), SEEK_CUR);
					read(fd2, &c1, sizeof(c1));
					c1.id++;
				c1.accountNumber = temp.accountNumber;
				strcpy(c1.name,name2); 
				write(fd2, &c1, sizeof(c1));
				
				strcpy(temp.password, password);
				temp.balance=0;
				temp.type=type;
				write(fd, &temp, sizeof(temp));
				printf("Writing acc number %d\n",temp.accountNumber);
				write(sock, &temp.accountNumber, sizeof(temp.accountNumber));
				write(sock, &c.id, sizeof(c.id));
				write(sock, &c1.id, sizeof(c1.id));
				close(fd);
				close(fd2);	
	}
		
	return 3;
}
void displayAccountDetails(struct account acc){
	if((acc).type==1) {
		printf("Individual account\n");
		//printf("Name : %s\n",((acc).customer1));
//		printf("Date of birth : %s\n",(acc).c1);
		printf("accountNumber : %d\n",(acc).accountNumber);
		printf("Password : %s\n",(acc).password);
		printf("Balance : %ld\n",(acc).balance);
	}
	else if((acc).type==2){
		printf("Joint account	\n");
		//printf("Name:%s\n",((acc).customer1));
		printf("accountNumber : %d\n",(acc).accountNumber);
		//printf("Date of birth :%s\n",((acc).c1).dob);
		//printf("Name:%s\n",((acc).customer2));
		//printf("Date of birth :%s\n",((acc).c2).dob);
		printf("Password : %s\n",(acc).password);
		printf("Balance : %ld\n",(acc).balance); 
	}
}
int performClientOperation(int sock,int accNo,int type){
	int fd,fp;
	struct account temp;
	int option;
	int amount,valid,balance;
	char Password[15];
	struct account acc;
  
		printf("Reading option in client operation\n");
		read(sock, &option, sizeof(int));
		printf("Read %d\n",option);
		
	
	if((fd = open(ACC[type-1], O_RDWR))==-1)printf("File Error\n");
	
	lseek(fd, (accNo - 202001)*sizeof(struct account), SEEK_CUR);
	
	//fp = lseek(fd, -1 * sizeof(struct account), SEEK_CUR);
	read(fd, &temp, sizeof(temp));
	printf("temp password->%s\ttemp acc->%d ",temp.password,temp.accountNumber);
	 switch(option){
  case 1:{
					read(sock, &amount, sizeof(amount));
		 		  temp.balance+=amount;//printf("%d Deposited to account number%d\n",amount,accNo);							
  				//break;
  				fp = lseek(fd, -1 * sizeof(struct account), SEEK_CUR);
  				write(fd, &temp, sizeof(temp));
					break;
  			}
  case 2:{
					read(sock, &amount, sizeof(amount));
			 	  if(temp.balance>=amount){
			 	   temp.balance-=amount;
			 	   valid =1;
			 	  }
			 	  else
			 	  	valid=0;
  				fp = lseek(fd, -1 * sizeof(struct account), SEEK_CUR);
  				write(fd, &temp, sizeof(temp));
			 	  
			 	  write(sock, &valid, sizeof(valid));	
					break;
  	}
  case 3:{
  				balance = temp.balance;
  				
  				write(sock, &balance, sizeof(balance));  				
  				break;
  			}
  case 4:{
  				read(sock, &Password, sizeof(Password));
  				strcpy(temp.password,Password);
  				fp = lseek(fd, -1 * sizeof(struct account), SEEK_CUR);
  				write(fd, &temp, sizeof(temp));  			
  			//	printf("Password change for account number successful!\n");
  				break;
  			}
  case 5:{
 					acc = temp;			 	
  				write(sock, &acc, sizeof(acc));
  				break;
  			}
  }
  return option;
}
