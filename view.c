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

char *ACC[2] = {"./database/accounts/individual", "./database/accounts/joint"};
char *CUS[2] = {"./database/customers", "./database/customers"};
int view();

int main(){
	while(view()!=4);//code to view all data
	return 0;
}
int view(){
	int option;
  int fd,fp;
  system("clear");
	printf("--------------- View ---------------\n");
	printf("1) Individual accounts\n");
	printf("2) Joint accounts\n");
	printf("3) Customers\n");
	printf("4) Exit\n");
	printf("Enter Your Choice :\n");
	scanf("%d", &option);
    printf("--------------------------------------------------\n");
    printf("--------------------------------------------------\n");
    while(option > 4 || option < 1){
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
        printf("Account number\tBalance\tPassword\n");
        while(read(fd,&acc,sizeof(acc)))
            printf("%d\t\t%ld\t\t%s\n",acc.accountNumber,acc.balance,acc.password);
        getchar();
        close(fd);
        break;
        }
    
    case 2:{
        fd = open(ACC[1],O_RDONLY);
        fp = lseek(fd,0,SEEK_SET);
        struct account acc;
        printf("Account number\tBalance\tPassword\n");
        while(read(fd,&acc,sizeof(acc))){
        	printf("%d\t\t%ld\t\t%s\n",acc.accountNumber,acc.balance,acc.password);
        	}
				getchar();       
				close(fd);
        break;}
    case 3:{
        fd = open(CUS[1],O_RDONLY);
        fp = lseek(fd,0,SEEK_SET);
        struct customer c;
        printf("Account number\tCustomer_ID\tName\n");
        while(read(fd,&c,sizeof(c))){
        	printf("%d\t\t%d\t\t%s\n",c.accountNumber,c.id,c.name);
        	}
				getchar();       
				close(fd);
        break;}
	}
	getchar();
	return option;
}
