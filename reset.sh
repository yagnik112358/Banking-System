rm -rf database
mkdir database
mkdir database/accounts
touch database/accounts/individual
touch database/accounts/joint
gcc -o client client.c
gcc -o server BankServer.c
gcc -o data dataview.c
