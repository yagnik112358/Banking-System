#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8759

//static int I_number=202000;
//static int J_number=202000; 
/*struct account{
	int type; //0 if individual,1 if joint;
	int accountNumber;
	struct customer c1;
	struct customer c2; 
	char *password;
	long balance; 
};*/
int banking(int sock);
void createIndividualAccount(int sock);
void createJointAccount(int sock);
void accountCreation(int sock);
void login(int sock);
int main(int argc,char *argv[]){
	//char *nam="harshmamma";
	//char *dat="12/02/1998";
	//struct customer c={nam,dat};
	//struct account acc = {1,c,NULL,0};
	//printf("Individual account\n");
	//printf("%s\n",((acc).c1).name);
	//printf("Date of birth :%s\n",acc.c1.dob);
	//printf("Balance:%ld\n",acc.balance);
	//displayAccountDetails(acc);
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
	
	while(banking(client_fd)!=3);
	close(client_fd);

		/*struct account acc;
  	int fd;
  	printf("Accessing from database\n");
    fd = open("database",O_RDONLY);
    read(fd,&acc,sizeof(acc));
    displayAccountDetails(acc);
    printf("done... \n");
    close(fd);*/
  return 0;
}
int banking(int sock){
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
  	login(sock);//printf("login\n");
  	break;
  case 2:
  	accountCreation(sock);//printf("reg\n");
  	break;
  case 3:{
  	printf("Thankyou for banking with us\n");
  	break;}
  }
  return option;
}
void accountCreation(int sock){
	int type;
	//struct account act;
	printf("Select type of account:\n");
	printf("1)Individual\n");			
  printf("2)Joint Account\n");
  printf("3)Exit\n");									
  scanf("%d",&type);
  while(type > 3 || type < 1) {
			printf("Invalid Choice!\n");
			printf("Enter Your Choice : ");
			scanf("%d", &type);
		}
	switch(type){
  case 1: //creates Individual Account
  	createIndividualAccount(sock);//printf("Individual\n");
  	break;
  case 2://creates Joint Account
  	createJointAccount(sock);//printf("Joint Account\n");
  	break;
  case 3:{	//go back to main menu
  		write(sock, &type, sizeof(type));
  		break;
			}
  default:
  	printf("Wrong choice\n");
  }
  
}
void createIndividualAccount(int sock){
	int accNo,type = 0;
	char customer1[20];
	char customer2[20];
//char *name=malloc(20);
	char password[15];
	printf("Enter your name:");
	scanf("%s",customer1);
	strcpy(customer2,customer1);
	strcpy(password,getpass("Enter Password:"));
	//printf("");
	//scanf("%s",pass);	
	//char *c=name;
	//struct account acc = {0,I_number,c,c,pass,0};
	
	
	write(sock, &type, sizeof(type));
	write(sock, &customer1, sizeof(customer1));
	write(sock, &customer2, sizeof(customer2));
	write(sock, &password, strlen(password));
	read(sock, &accNo, sizeof(accNo));
	printf("Your account number is : %d.\nRemember the account number for login\n", accNo);
		while(getchar()!='\n');
		getchar();
	/*I_number+=1;
	//printf("Password in create: %s\n",(acc).password);
	displayAccountDetails(acc);
	return acc;*/
}
void createJointAccount(int sock){
	int accNo;
	int type=1;
	char customer1[20];
	char customer2[20];
	char password[15];
	printf("Enter your name:");
	scanf("%s",customer1);
	
	printf("Enter JointHolder's name:");
	scanf("%s",customer2);
	strcpy(password,getpass("Enter Password:"));
	//char *c2=name1;
	write(sock, &type, sizeof(type));
	write(sock, &customer1, sizeof(customer1));
	write(sock, &customer2, sizeof(customer2));
	write(sock, &password, strlen(password));
		
	read(sock, &accNo, sizeof(accNo));
	printf("Your account number is : %d.\nRemember the account number for login\n", accNo);
		while(getchar()!='\n');
		getchar();
	/*struct account acc = {1,J_number,c,c1,password,0};
	
	displayAccountDetails(acc);
	return acc;*/
}
void login(int sock){
	int type,accNo;
	char password[15];
	int validLogin;
	printf("Select type of account:\n");
	printf("1)Individual\n");			
  printf("2)Joint Account\n");
  printf("3)Exit\n");									
  scanf("%d",&type);
  while(type > 3 || type < 1) {
			printf("Invalid Choice!\n");
			printf("Enter Your Choice : ");
			scanf("%d", &type);
		}
		
		write(sock, &type, sizeof(type));
					
		if(type ==1 || type ==2){
				printf("Enter Your Account Number: ");
				scanf("%d", &accNo);
				strcpy(password,getpass("Enter password: "));
				write(sock, &accNo, sizeof(accNo));
				write(sock, &password, strlen(password));
		}					
		
		read(sock, &validLogin, sizeof(validLogin));
		printf("You are a valid or not -> %d\n",validLogin);
		if(validLogin == 1){
			//Login window
			printf("You are a valid client");
			getchar();
			system("clear");
		}
		else{
			printf("Login Failed\n");
			while(getchar()!='\n');
			getchar();
			
		}
}
/*void withdraw(struct account *acc,int amount){
	if((*acc).balance>=amount){
		if(amount<50000){
		(*acc).balance-=amount;
		}
		else
			printf("amount too large for single transaction");
	}
	else{
		printf("tera baap chod k gaya tha ki teri maa?");
	}
}*/
/*void deposit(struct account *acc,int amount){
	(*acc).balance+=amount;
}*/


