struct customer {
	int id;
	char name[20];
	int accountNumber;
};
struct account{
	int type; //1 if individual,2 if joint
	int accountNumber;
	char password[15];
	long int balance; 
};

