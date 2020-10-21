#include<stdio.h>
#include<stdlib.h>

struct customer{
	char *name;
	char *dob;
};
struct account{
	int type; //0 if individual,1 if joint;
	struct customer c1;
	struct customer c2; 
	char *password;
	long balance; 
};
void displayAccountDetails(struct account acc){
	if((acc).type==0) {
		printf("Individual account\n");
		printf("Name : %s\n",((acc).c1).name);
		printf("Date of birth : %s\n",((acc).c1).dob);
		printf("Password : %s\n",(acc).password);
		printf("Balance : %ld\n",(acc).balance);
	}
	else if((acc).type==1){
		printf("Joint account\n");
		printf("Name:%s\n",((acc).c1).name);
		printf("Date of birth :%s\n",((acc).c1).dob);
		printf("Name:%s\n",((acc).c2).name);
		printf("Date of birth :%s\n",((acc).c2).dob);
		printf("Password : %s\n",(acc).password);
		printf("Balance : %ld\n",(acc).balance); 
	}
}
void createIndividualAccount(){
	char *nam=malloc(20);
	char *dat = malloc(10);
	char *password = malloc(10);
	printf("Enter your name:");
	scanf("%s",nam);
	printf("Enter your Date of Birth:");
	scanf("%s",dat);
	printf("Enter your Date of Birth:");
	scanf("%s",password);	
	struct customer c={nam,dat};
	struct account acc = {0,c,NULL,password,0};
	displayAccountDetails(acc);
}
void createJointAccount(){
	char *nam=malloc(20);
	char *dat = malloc(10);
	char *password = malloc(10);
	//long  amt=0;
	printf("Enter your name:");
	scanf("%s",nam);
	printf("Enter your Date of Birth:");
	scanf("%s",dat);
	struct customer c={nam,dat};
	
	char *name1=malloc(20);
	char *date1 = malloc(10);
	printf("Enter customer 2 name:");
	scanf("%s",name1);
	printf("Enter customer 2 Date of Birth:");
	scanf("%s",date1);
	printf("Enter your Date of Birth:");
	scanf("%s",password);	
	struct customer c1={name1,date1};
	
	struct account acc = {1,c,c1,password,0};
	
	displayAccountDetails(acc);
}
void accountCreation(){
	int option;
	printf("Select type of account:\n");
	printf("1)Individual\n");			
  printf("2)Joint Account\n");			
  scanf("%d",&option);
  switch(option){
  case 1:
  	createIndividualAccount();//printf("Individual\n");
  	break;
  case 2:
  	createJointAccount();//printf("Joint Account\n");
  	break;
  default:
  	printf("Wrong choice\n");
  }
}
int main(){
	//char *nam="harshmamma";
	//char *dat="12/02/1998";
	//struct customer c={nam,dat};
	//struct account acc = {1,c,NULL,0};
	//printf("Individual account\n");
	//printf("%s\n",((acc).c1).name);
	//printf("Date of birth :%s\n",acc.c1.dob);
	//printf("Balance:%ld\n",acc.balance);
	//displayAccountDetails(acc);
	printf("----------------------------------------------------\n");			
  printf("---------------BANK MANAGEMENT SYSTEM---------------\n");			
  printf("----------------------------------------------------\n");	
  printf("1)Login\n");			
  printf("2)New account\n");			
  char option;
  scanf("%c",&option);
  switch(option){
  case '1':
  	printf("login\n");
  	break;
  case '2':
  	accountCreation();//printf("reg\n");
  	break;
  default:
  	printf("Wrong choice\n");
  }
  return 0;
}
