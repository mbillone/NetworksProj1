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

    int savings = 100;
    int checkings = 100;
int convertToInt(char num[20]){
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
void transferAmount(char depositAmount[3][20]){
    int quantity = convertToInt(depositAmount[2]);
    printf("Amount to Transfer: %d from %s\n",quantity,depositAmount[1]);
    if (strncmp(depositAmount[1],"checkingstosavings",2)==0){
        if (quantity<=checkings){
            checkings -= quantity;
            savings+=quantity;
	        printf("Checkings After: %d\n", checkings);
	        printf("Savings After: %d\n", savings);
        }
        else{
            printf("Insufficient Balance\n");
        }
    }
    else if (strncmp(depositAmount[1],"savingstocheckings",2)==0){
        if (quantity<=savings){
            checkings += quantity;
            savings-=quantity;
	        printf("Checkings After: %d\n", checkings);
	        printf("Savings After: %d\n", savings);
        }
        else{
            printf("Insufficient Balance\n");
        }
    }
}

void withdrawAmount(char depositAmount[3][20]){
    int quantity = convertToInt(depositAmount[2]);
    printf("Amount to Withdraw: %d from %s\n",quantity,depositAmount[1]);
    if (strncmp(depositAmount[1],"checkings",2)==0){
        if (quantity>checkings){
            printf("Insufficient Balance\n");
        }
        else if (quantity%20!=0){
            printf("Not a multiple of 20\n");
        }
        else{
            checkings-=quantity;
	        printf("Checkings After: %d\n", checkings);
        }
    }
    else if (strncmp(depositAmount[1],"savings",2)==0){
        printf("Please withdraw from checkings\n");
    }
}

void checkBalance(char depositAmount[3][20]){
    if (strncmp(depositAmount[1],"checkings",2)==0){
        printf("Balance of checkings account: %d\n",checkings);
    }
    else if (strncmp(depositAmount[1],"savings",2)==0){
        printf("Balance of savings account: %d\n", savings);
    }
}

void depositCheck(char depositAmount[3][20]){
    /*
    This handles changing deposit amounts. It first calculates the int by calling convertToInt() then it adds this
    amount to either the checkings or the savings depending on the second portion of the string.
    */
    int quantity = convertToInt(depositAmount[2]);
    printf("Amount Deposited: %d to %s\n",quantity,depositAmount[1]);
    if (strncmp(depositAmount[1],"checkings",2)==0){
        checkings +=quantity;
	    printf("Checkings After: %d\n", checkings);
    }
    else if (strncmp(depositAmount[1],"savings",2)==0){
        savings += quantity;
	    printf("Savings After: %d\n", savings);
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
   printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n"); 
         close(sock_server);
         exit(1);
      }
 
      /* receive the message */

      bytes_recd = recv(sock_connection, sentence, STRING_SIZE, 0);
      if (bytes_recd > 0){
         printf("Received Sentence is:\n");
         printf("%s", sentence);
         printf("\nwith length %d\n\n", bytes_recd);
        //  printf("%c\n",sentence[0]);
        char splitStrings[3][20]; //can store 10 words of 10 characters
        int i;
        int j;
        int cnt;
        // str = sentence;
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
        if (strncmp(splitStrings[0],"Balance",2)==0){
            checkBalance(splitStrings);
        }
        else if (strncmp(splitStrings[0],"Deposit",2)==0){
            depositCheck(splitStrings);
        }
        else if (strncmp(splitStrings[0],"Withdraw",2)==0){
            withdrawAmount(splitStrings);
        }
        else if (strncmp(splitStrings[0],"Transfer",2)==0){
            transferAmount(splitStrings);
        }
        else{
            printf("%s\n","Not an option buckaroo");
        }
        /* prepare the message to send */

         msg_len = bytes_recd;

         for (i=0; i<msg_len; i++)
            modifiedSentence[i] = toupper (sentence[i]);

         /* send message */
 
         bytes_sent = send(sock_connection, modifiedSentence, msg_len, 0);
      }

      /* close the socket */

      close(sock_connection);
   } 
}
