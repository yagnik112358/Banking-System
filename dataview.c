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


#define PORT 8759
#define ERR_EXIT(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)
struct account{
	int type; //0 if individual,1 if joint;
	int accountNumber;
	char customer1[20];
	char customer2[20]; 
	char password[15];
	long balance; 
};
char *ACC[2] = {"./database/accounts/individual", "./database/accounts/joint"};
int view();
int main(){
	while(view()!=3);//code to view all data
	return 0;
}
int view(){
	int option;
  int fd,fp;
  system("clear");
	printf("--------------- View ---------------\n");
	printf("1) Individual accounts\n");
	printf("2) Joint accounts\n");
	printf("3) Exit\n");
	printf("Enter Your Choice :\n");
	scanf("%d", &option);
    printf("--------------------------------------------------\n");
    printf("--------------------------------------------------\n");
    while(option > 3 || option < 1){
  		printf("Enter valid Choice :\n");
			scanf("%d", &option);
    }
//    printf("here before switch\n");
        
    switch (option)
    {
    case 1:{
		    //printf("hereinside switch case 1\n");
        
        fd = open(ACC[0],O_RDONLY);
        if(fd==-1){printf("error in opening");}
        fp = lseek(fd,0,SEEK_SET);
        struct account acc;
        printf("Name\t\tAccount number\t\tBalance\t\tPassword\n");
        while(read(fd,&acc,sizeof(acc)))
            printf("%s\t\t%d\t\t%ld\t\t%s\n",acc.customer1,acc.accountNumber,acc.balance,acc.password);
        getchar();
        close(fd);
        break;
        }
    
    case 2:{
        fd = open(ACC[1],O_RDONLY);
        fp = lseek(fd,0,SEEK_SET);
        struct account acc;
        printf("Name1\t\tName2\t\tAccount number\t\tBalance\t\tPassword\n");
        while(read(fd,&acc,sizeof(acc))){
        	printf("%s\t\t%s\t\t%d\t\t%ld\t\t%s\n",acc.customer1,acc.customer2,acc.accountNumber,acc.balance,acc.password);
}
				getchar();       
				close(fd);
        break;}
    case 3:
    	break;
	}
	getchar();
	return option;
}
