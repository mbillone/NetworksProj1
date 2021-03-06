/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 13, 2018 */    

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024   

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 3636

int count;
int savings = 0; //Total amount in savings at current moment
int checkings = 0; //Total amount in checkings at current moment
// int errorCode = -1;
char errorCode[1]; //If there is an error, the errorCode will be stored here as IB, MT, or SA.
char balanceBefore[7]; //Balance before the current transaction
char balanceAfter[7]; //Balance after the current transaction
char returnString[24]; //String that will be returned by the server. 24 is the maximum largest size of this string.

int convertToInt(char num[13]){
    /*
    This converts any string of length 20 to an int.
    */
    int dec = 0, i, j, len;
    len = strlen(num);
	for(i=0; i<len; i++){
		dec = dec * 10 + ( num[i] - '0' );
	}
	return dec;
}

void transferAmount(char transferQuant[3][13]){
    /*Takes the money from one account and transfers it to the other one*/
    int quantity = convertToInt(transferQuant[2]);
    printf("Amount to Transfer: %d from %s\n",quantity,transferQuant[1]);
    if (strncmp(transferQuant[1],"checkings",2)==0){
        //Transfer from checkings to savings
        sprintf(balanceBefore,"%d",savings); //stores the balance of savings before the transfer
        if (quantity<=checkings){
            //Removes money from checkings and puts it into savings
            checkings -= quantity;
            savings+=quantity;
	        printf("Checkings After: %d\n", checkings);
	        printf("Savings After: %d\n", savings);
        }
        else{
            //If savings amount > requested transfer, this creates an error code.
            strcpy(errorCode,"IB");
            printf("Insufficient Balance");
        }
        sprintf(balanceAfter,"%d",savings); //stores the balance of savings after the transfer
    }
    else if (strncmp(transferQuant[1],"savings",2)==0){
        //Transfer from savings to checkings
        sprintf(balanceBefore,"%d",checkings); //stores the balance of checkings before the transfer
        if (quantity<=savings){
            //Removes money from savings and puts it into checkings
            checkings += quantity;
            savings-=quantity;
	        printf("Checkings After: %d\n", checkings);
	        printf("Savings After: %d\n", savings);
        }
        else{
            //If savings amount > requested transfer, this creates an error code.
            strcpy(errorCode,"IB");
            printf("Insufficient Balance");
        }
        sprintf(balanceBefore,"%d",checkings); //stores the balance of checkings after the transfer
    }
}

void withdrawAmount(char depositAmount[3][13]){
    /*Allows the server to take away an amount from the specified account*/
    int quantity = convertToInt(depositAmount[2]); //Amount to withdraw
    printf("Amount to Withdraw: %d from %s\n",quantity,depositAmount[1]);
    if (strncmp(depositAmount[1],"checkings",2)==0){
        //This handles withdrawals from checkings
        sprintf(balanceBefore,"%d",checkings); //Stores the balance of checkings before the withdrawal
        if (quantity>checkings){
            //If the quantity being withdrawn is larger than the amount in checking, this error code is flagged
            strcpy(errorCode,"IB");
            printf("Insufficient Balance");
        }
        else if (quantity%20!=0){
            //If the quantity being withdrawn is not a multiple of 20, then this error code is flagged.
            strcpy(errorCode,"MT");
            printf("Not a multiple of 20\n");
        }
        else{
            //None of the error codes were flagged and the transaction succeeds.
            checkings-=quantity;
	        printf("Checkings After: %d\n", checkings);
        }
        sprintf(balanceAfter,"%d",checkings);//Records the quantity of checkings after the withdrawal.
    }
    else if (strncmp(depositAmount[1],"savings",2)==0){
        //You can only withdraw from checkings, so if savings is requested this error code is called.
        sprintf(balanceBefore,"%d",savings);
        strcpy(errorCode,"SA");
        printf("Please withdraw from checkings\n");
        sprintf(balanceAfter,"%d",savings);
    }
}

void checkBalance(char checkAmount[3][13]){
    //This handles just checking the balance of the account that is called.
    if (strncmp(checkAmount[1],"checkings",2)==0){ //When you are calling the balance of checking
        sprintf(balanceBefore,"%d",checkings);
        printf("Balance of checkings account: %d\n",checkings);
        sprintf(balanceAfter,"%d",checkings);
    }
    else if (strncmp(checkAmount[1],"savings",2)==0){ //When you are calling the balance of savings.
        sprintf(balanceBefore,"%d",savings);
        printf("Balance of savings account: %d\n", savings);
        sprintf(balanceAfter,"%d",savings);
    }
}

void depositCheck(char depositAmount[3][13]){
    /*
    This handles changing deposit amounts. It first calculates the int by calling convertToInt() then it adds this
    amount to either the checkings or the savings depending on the second portion of the string.
    */
    int quantity = convertToInt(depositAmount[2]);
    printf("Amount Deposited: %d to %s\n",quantity,depositAmount[1]);
    if (strncmp(depositAmount[1],"checkings",2)==0){
        sprintf(balanceBefore,"%d",checkings);
        checkings +=quantity;
	    printf("Checkings After: %d\n", checkings);
        sprintf(balanceAfter,"%d",checkings);
    }
    else if (strncmp(depositAmount[1],"savings",2)==0){
            sprintf(balanceBefore,"%d",savings);
        savings += quantity;
	    printf("Savings After: %d\n", savings);
            sprintf(balanceAfter,"%d",savings);
    }
}

int main(void) {
    int sock_server;  /* Socket on which server listens to clients */
    int sock_connection;  /* Socket on which server exchanges data with client */

    struct sockaddr_in server_addr;  /* Internet address structure that
                                            stores server address */
    unsigned int server_addr_len;  /* Length of server address structure */
    unsigned short server_port;  /* Port number used by server (local port) */

    struct sockaddr_in client_addr;  /* Internet address structure that
                                            stores client address */
    unsigned int client_addr_len;  /* Length of client address structure */

    char sentence[STRING_SIZE];  /* receive message */
    char modifiedSentence[STRING_SIZE]; /* send message */
    unsigned int msg_len;  /* length of message */
    int bytes_sent, bytes_recd; /* number of bytes sent or received */
    unsigned int i;  /* temporary loop variable */
    
    /* open a socket */
    if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Server: can't open stream socket");
        exit(1);                                                
    }
    /* initialize server address information */
        
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                            any host interface, if more than one
                                            are present */ 
    server_port = SERV_TCP_PORT; /* Server will listen on this port */
    server_addr.sin_port = htons(server_port);

    /* bind the socket to the local server port */

    if (bind(sock_server, (struct sockaddr *) &server_addr,
                                        sizeof (server_addr)) < 0) {
        perror("Server: can't bind to local address");
        close(sock_server);
        exit(1);
    }                     

        
    /* listen for incoming requests from clients */

    if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
        perror("Server: error on listen"); /* requests that will be queued */
        close(sock_server);
        exit(1);
    }
    printf("\n I am here to listen ... on port %hu\n\n", server_port);
    
    client_addr_len = sizeof (client_addr);
    
    for (;;) {
    /* wait for incoming connection requests in an indefinite loop */

    

        sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                            &client_addr_len);
                        /* The accept function blocks the server until a
                            connection request comes from a client */
        if (sock_connection < 0) {
            perror("Server: accept() error\n"); 
            close(sock_server);
            exit(1);
        }
        
        for(count = 0; count < 5;count++){
        /* receive the message */

            bytes_recd = recv(sock_connection, sentence, STRING_SIZE, 0);
                if (bytes_recd > 0){
                    //Resets all the stored code
                    strcpy(errorCode,"NA");
                    strcpy(balanceBefore,"");
                    strcpy(balanceAfter,"");
                    strcpy(returnString,"");
                    //What the server received from the client
                    printf("Received Sentence is:\n");
                    printf("%s", sentence);
                    printf("\nwith length %d\n\n", bytes_recd);
                    //Convers what we received from the client into three split strings. If we have balance, it
                    //still has three sections, although the third is empty.
                    char splitStrings[3][13];
                    int i;
                    int j;
                    int cnt;
                    j=0; 
                    cnt=0;
                    for(i=0;i<=(strlen(sentence));i++){
                        if(sentence[i]==' '||sentence[i]=='\0'){
                            splitStrings[cnt][j]='\0';
                            cnt++; 
                            j=0; 
                        }
                        else{
                            splitStrings[cnt][j]=sentence[i];
                            j++;
                        }
                    }
                    strcat(returnString,splitStrings[0]); //Keeps track of the transaction for the returnString
                    if (strncmp(splitStrings[0],"Balance",2)==0){
                        checkBalance(splitStrings);//Sends this off to the check balance function
                    }
                    else if (strncmp(splitStrings[0],"Deposit",2)==0){
                        depositCheck(splitStrings);//Sends this off to the deposit function
                    }
                    else if (strncmp(splitStrings[0],"Withdraw",2)==0){
                        withdrawAmount(splitStrings);//Sends this off to the withdraw function
                    }
                    else if (strncmp(splitStrings[0],"Transfer",2)==0){
                        transferAmount(splitStrings);//Sends this off to the transfer function
                    }
                    else{
                        printf("%s\n","Not an option buckaroo");
                    }
                    //These take all the information stored earlier that is necessary for the return string
                    strcat(returnString," ");//Spaces help to keep the message neat and more machine readable
                    strcat(returnString,errorCode);
                    strcat(returnString," ");
                    strcat(returnString,splitStrings[1]); //This is the account the transaction was on
                    strcat(returnString," ");
                    strcat(returnString,balanceBefore);
                    strcat(returnString," ");
                    strcat(returnString,balanceAfter);
                    printf("%s",returnString);
                    /* prepare the message to send */
                    msg_len = bytes_recd;

                    for (i=0; i<msg_len; i++)
                        modifiedSentence[i] = toupper (sentence[i]);

                    /* send message */
            
                    bytes_sent = send(sock_connection, returnString, 20, 0);
                }
        }

        /* close the socket */

        close(sock_connection);
    } 
}
