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

int defaultpage(int sock);
int deleteAccount(int sock,int accNo,int type);
int adminBanking(int sock);
int deleteAccount(int sock,int accNo,int type);
int clientOperation(int sock,int accNo,int type);
int adminOperations(int sock);
void Operation(int sock,int option,int accNo,int type);
int banking(int sock,int);
void createIndividualAccount(int sock,int type);
void createJointAccount(int sock,int type);
void accountCreation(int sock,int type);
void login(int sock,int type);

int main(int argc,char *argv[]){
	
	char *ip = "127.0.0.1";
	if(argc==2){
		ip = argv[1];
	}
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd == -1){
		printf("socket creation failed\n");
		exit(0);
	}
	struct sockaddr_in cadr;
	cadr.sin_family=AF_INET;
	cadr.sin_port= htons(PORT);
	cadr.sin_addr.s_addr = inet_addr(ip);
	if(connect(client_fd, (struct sockaddr *)&cadr, sizeof(cadr))==-1){
		printf("connect() failed\n");
		exit(0);
	}
	printf("connection established\n");
	
	while(defaultpage(client_fd)!=4);
	close(client_fd);

  return 0;
}

int defaultpage(int sock){
	system("clear");
	printf("----------------------------------------------------\n");			
  printf("---------------BANK MANAGEMENT SYSTEM---------------\n");			
  printf("----------------------------------------------------\n");	
	printf("Enter Type of User:\n");
  printf("1)Normal User\n");
  printf("2)Joint Account User\n");
  printf("3)Admin\n");
  printf("4)Exit\n");			
  int option;
  printf("-----------------Enter Your Choice-----------------\n");		
  scanf("%d",&option);  
  while(option > 4 || option < 1)
	{
		printf("Invalid Choice!\n");
		printf("-----------------Enter Your Choice-----------------\n");
		scanf("%d", &option);
		getchar();
	}
	
  write(sock,&option,sizeof(int));
  
  if(option == 3) 
  	adminBanking(sock);
  else if(option == 2 ||option == 1)
  	banking(sock,option);
  	
  return option;							
}
int adminBanking(int sock){
	char id[5];
	char password[5];
	char oid[5] = "root\0";
	char opassword[5]="pass\0";
	system("clear");
	printf("----------------------------------------------------\n");			
  printf("---------------BANK MANAGEMENT SYSTEM---------------\n");			
  //printf("----------------------------------------------------\n");	
	printf("--------------------Admin Login---------------------\n");
				printf("Enter ID: ");
				scanf("%s", id);
				strcat(id,"\0");
				strcpy(password,getpass("Enter password: "));
				strcat(password,"\0");
	
			if(!strcmp(opassword, password) && !strcmp(oid, id)){
					//admin menu
						while(adminOperations(sock)!=4);					
					//printf("You are a valid Admin\n");
					getchar();
				}
			else{
					printf("Admin Login Failed\n");
					while(getchar()!='\n');
					getchar();
			}
}
int adminOperations(int sock){
	int option,accNo,type;
	printf("1)Modify account\n");			
  printf("2)Add account\n");
	printf("3)Delete account\n");
	printf("4)Back\n");
	printf("-----------------Enter Your Choice-----------------\n");
	scanf("%d",&option);  
  
  while(option > 4 || option < 1)
	{
		printf("Invalid Choice!\n");
		printf("-----------------Enter Your Choice-----------------\n");
		scanf("%d", &option);
	}
	write(sock,&option,sizeof(int));
  //getchar();
	if(option!=4){
		printf("Enter Type of Account:\n");
		printf("1)Normal Account \n");
		printf("2)Joint Account \n");
		scanf("%d",&type);	    
		if(option!=2){
			printf("Enter account Number:\n");			
			scanf("%d",&accNo);
			getchar();
		}
	}
	if(option ==1){
		int option1;
		printf("1)Deposit\n");			
		printf("2)Withdraw\n");
		printf("3)Balance Enquiry\n");						
		printf("4)Password Change\n");
		printf("5)Back\n");
		printf("-----------------Enter Your Choice-----------------\n");
		scanf("%d", &option1);
		while(option1 > 5 || option1 < 1)
		{
			printf("Invalid Choice!\n");
			printf("-----------------Enter Your Choice-----------------\n");
			scanf("%d", &option1);
			
		}
		if(option1==5){ return option;}
		printf("option,option1 in admin !%d | %d\n",option,option1);
		write(sock, &type, sizeof(type));	
		write(sock, &accNo, sizeof(accNo));
		Operation(sock,option1,accNo,type);
		
	}
	if(option == 2){
		//printf("option in admin =2!\n");
		accountCreation(sock,type);
	}
	if(option == 3){
		deleteAccount(sock,accNo,type);//working
	}
	
	return option;
}
int deleteAccount(int sock,int accNo,int type){
		write(sock, &type, sizeof(type));
		write(sock, &accNo, sizeof(accNo));
		getchar();
		int success;
		
		read(sock, &success, sizeof(success));
		printf("success in delete acc = %d",success);
		if(success ==1){
			printf("Account %d Deleted\n",accNo);
		}
		else
			{printf("Account %d does't Exist\n",accNo);}
}
int banking(int sock,int type){
	system("clear");
	printf("----------------------------------------------------\n");			
  printf("---------------BANK MANAGEMENT SYSTEM---------------\n");			
  printf("----------------------------------------------------\n");	
  printf("1)Login\n");			
  printf("2)New account\n");
  printf("3)Exit\n");						
  int option;
  printf("-----------------Enter Your Choice-----------------\n");		
  scanf("%d",&option);  
  while(option > 3 || option < 1)
	{
		printf("Invalid Choice!\n");
		printf("-----------------Enter Your Choice-----------------\n");
		scanf("%d", &option);
	} 
	write(sock,&option,sizeof(int));
  switch(option){
  case 1:
  	login(sock,type);//printf("login\n");
  	break;
  case 2:
  	accountCreation(sock,type);//printf("reg\n");
  	break;
  case 3:{
  	printf("Thankyou for banking with us\n");
  	break;}
  }
  return option;
}
void accountCreation(int sock,int type){
	
	switch(type){
  case 1: //creates Individual Account
  	createIndividualAccount(sock,type);//printf("Individual\n");
  	break;
  case 2://creates Joint Account
  	createJointAccount(sock,type);//printf("Joint Account\n");
  	break;
  default:	//go back to main menu
  		write(sock, &type, sizeof(type));
  		printf("Wrong choice\n");
  }
  
}
void createIndividualAccount(int sock,int type){
	int accNo,customer_id;
	char name[20];
	char password[15];
	printf("Enter your name:");
	scanf("%s",name);
	
	strcpy(password,getpass("Enter Password:"));
	write(sock, &type, sizeof(type));
	write(sock, &name, sizeof(name));
	write(sock, &password, strlen(password));
	read(sock, &accNo, sizeof(accNo));
	read(sock, &customer_id ,sizeof(customer_id));
	printf("Your account number is : %d and customer id is %d.\nRemember the account number for login\n", accNo,customer_id);
		while(getchar()!='\n');
		getchar();
}
void createJointAccount(int sock,int type){
	int accNo,customer_id,customer_id1;//,type = 2;
	char name[20];
	char name2[20];
	char password[15];
	printf("Enter your name:");
	scanf("%s",name);
	printf("Enter JointHolder's name:");
	scanf("%s",name2);
	
	strcpy(password,getpass("Enter Password:"));
	write(sock, &type, sizeof(type));
	write(sock, &name, sizeof(name));
	write(sock, &name2, sizeof(name2));
	write(sock, &password, strlen(password));
	read(sock, &accNo, sizeof(accNo));
	read(sock, &customer_id ,sizeof(customer_id));
	read(sock, &customer_id1 ,sizeof(customer_id1));
	printf("Your account number is : %d and customer id is %d and %d.\nRemember the account number for login\n", accNo,customer_id,customer_id1);
		while(getchar()!='\n');
		getchar();
}
void login(int sock,int type){
	int accNo;
	char password[15];
	int validLogin;
	
		write(sock, &type, sizeof(type));
					
		if(type ==1 || type ==2){
				printf("Enter Your Account Number: ");
				scanf("%d", &accNo);
				strcpy(password,getpass("Enter password: "));
				//strcat(password,"\0");
				//printf("password read:%s",password);
				write(sock, &accNo, sizeof(accNo));
				write(sock, &password, sizeof(password));
		}					
						
		read(sock, &validLogin, sizeof(validLogin));
		//printf("You are a valid or not -> %d\n",validLogin);
		if(validLogin == 1){
			//Login window
			//printf("You are a valid client\n");
			//getchar();
			getchar();
			
			while(clientOperation(sock,accNo,type)!=6);
			
			while(getchar()!='\n');
			//getchar();
			system("clear");
		}
		else{
			printf("Login Failed\n");
			while(getchar()!='\n');
			getchar();
			
		}
}

 
int clientOperation(int sock,int accNo,int type){
			//write(sock, &type, sizeof(type));
	
        
	system("clear");
	printf("----------------------------------------------------\n");			
  printf("---------------BANK MANAGEMENT SYSTEM---------------\n");			
  printf("----------------------------------------------------\n");	
  
  printf("-----------------Welcome %d-----------------\n",accNo);
  printf("1)Deposit\n");			
  printf("2)Withdraw\n");
  printf("3)Balance Enquiry\n");						
  printf("4)Password Change\n");
  printf("5)View details\n");
  printf("6)Exit\n");
  int option;
  printf("-----------------Enter Your Choice-----------------\n");		
  scanf("%d",&option);  
  while(option > 6 || option < 1)
	{
		printf("Invalid Choice!\n");
		printf("-----------------Enter Your Choice-----------------\n");
		scanf("%d", &option);
	}
//	 printf("-----------------option read %d-----------------\n",option);
	 write(sock, &option, sizeof(option));
	 
	 Operation(sock,option,accNo,type);//end of switch
	 return option;		
}
void Operation(int sock,int option,int accNo,int type){

int amount,valid,balance;
		char Password[15];
	struct account acc;

switch(option){
  case 1:{
					printf("Enter Amount:\n");
					scanf("%d",&amount);  
				 	write(sock, &amount, sizeof(amount));
		 		  getchar();
		 		  printf("%d Deposited to account number %d\n",amount,accNo);							
  				getchar();
  				break;
  			}
  case 2:{
					printf("Enter Amount:\n");
					scanf("%d",&amount);  
					write(sock, &amount, sizeof(amount));
			 	  getchar();
			 	  read(sock, &valid, sizeof(valid));
			 	  if(valid)
			 	  	printf("%d Withdrawn from account number %d\n",amount,accNo);
					else
						printf("Insufficient Amount:\n");			
					getchar();
					break;					
  	}
  case 3:{
  				read(sock, &balance, sizeof(balance));
  				printf("Balance of account number %d is %d\n",accNo,balance);
					getchar();
					getchar();
					break;
  			}
  case 4:{
  				printf("Enter new Password:\n");
					scanf("%s",Password);  
  				write(sock, &Password, sizeof(Password));
  				getchar();
  				printf("Password change for account number %d successful!\n",accNo);
					getchar();
  				break;
  			}
  case 5:{
  				read(sock, &acc, sizeof(acc));
  				getchar();
  				printf("Account number\tBalance\tPassword\n");
          printf("%d\t\t%ld\t\t%s\n",acc.accountNumber,acc.balance,acc.password);
					getchar();
  				break;
  			}
  }
  //return option;
}
