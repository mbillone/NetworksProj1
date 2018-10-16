/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* Sept. 13, 2018 */     

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024

char input[13];
char testing[2];
char storeString[30];
char sentence[STRING_SIZE];
void createString(){
    /*This is used for creating the transaction and for creating the account used.*/
    strncpy(testing, sentence, 2);
    strcat(storeString,sentence);
    strcat(storeString," ");
    strcat(input,testing);
    strcat(input," ");
}

int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

//    char sentence[STRING_SIZE];  /* send message */
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   /* open a socket */

   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   printf("Enter hostname of server: ");
   scanf("%s", server_hostname);

   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   printf("Enter port number for server: ");
   scanf("%hu", &server_port);

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);
    /* connect to the server */
 		
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }

   /* user interface */
   /* We did this in three questions. The first question, please answer one of the four indicated
   transactions. In the second one, please specify savings for savings to checkings transfer, or
   vice versa. The last question will show up for everything but balance, and handles the amount
   to be utilized in the transaction*/
   printf("Please input a Transaction (Balance, Withdraw, Deposit, Transfer):\n");
   scanf("%s", sentence);
   createString();
   if (strncmp(storeString,"Transfer",2)!=0){
        printf("Which account would you like to use?\n");
        scanf("%s", sentence);
        createString();
   }
   else{
       printf("Transfer from savings or checkings (one word specification)?\n");
       scanf("%s", sentence);
       createString();
   }
   if (strncmp(storeString,"Balance",2)!=0){
        printf("What amount would you like?\n");
        scanf("%s", sentence);
        strcat(storeString,sentence);
        strcat(storeString," ");
        strcat(input,sentence);
   }
//    puts(input);
   msg_len = 13;
   /* send message */
   
   printf("Sent to server: %s\n",input); //What is sent to the server.
   printf("Meaning: %s\n",storeString); //Readable for the user.
   bytes_sent = send(sock_client, input, msg_len, 0);

   /* get response from server */
  
   bytes_recd = recv(sock_client, modifiedSentence, STRING_SIZE, 0); 
    //This is the response returned from the server. It is a character array of length 24 since
    //this is the largest length that can be returned with how we did our messages. This is not
    //the portion that is legible for the reader, just the simulation of the messages within the
    //server and client.
    printf("\nThe response from server is:\n");
    printf("%s\n",modifiedSentence);
    //This handles breaking the returned sentence into a string of multiple commands, which will
    //later be translated for user readability.
    char splitStrings[5][7];
    int i;
    int j;
    int cnt;
    for(i=0;i<=(strlen(modifiedSentence));i++){
        if(modifiedSentence[i]==' '||modifiedSentence[i]=='\0'){
            splitStrings[cnt][j]='\0';
            cnt++; 
            j=0; 
        }
        else{
            splitStrings[cnt][j]=modifiedSentence[i];
            j++;
        }
    }
    char finalString[100];
    //States the transaction type (Balance, Deposit, Withdraw, Transfer)
    if (strncmp(splitStrings[0],"Balance",2)==0){
        strcpy(finalString,"Transaction type = Balance\n");
    }
    else if (strncmp(splitStrings[0],"Deposit",2)==0){
        strcpy(finalString,"Transaction type = Deposit\n");
    }
    else if (strncmp(splitStrings[0],"Withdraw",2)==0){
        strcpy(finalString,"Transaction type = Withdraw\n");
    }
    else if (strncmp(splitStrings[0],"Transfer",2)==0){
        strcpy(finalString,"Transaction type = Transfer\n");
    }
    //If there is no error, it will skip this part. If there is, it translates the 
    //two character error code into one that is legible 
    if (strncmp(splitStrings[1],"NA",2)!=0){
        if (strncmp(splitStrings[1],"IB",2)==0){
            strcat(finalString,"Error Code: Insufficient Balance\n");
        }
        else if (strncmp(splitStrings[1],"MT",2)==0){
            strcat(finalString,"Error Code: You can only withdraw multiples of 20\n");
        }
        else{
            strcat(finalString,"Error Code: Please only withdraw from checkings\n");
        }
    }
    //This states whether the transaction was performed on checkings or savings.
    if (strncmp(splitStrings[2],"checkings",2)==0){
        strcat(finalString,"Transaction acted on checkings\n");
    }
    else if (strncmp(splitStrings[2],"savings",2)==0){
        strcat(finalString,"Transaction acted on savings\n");
    }
    //The next bit handles adding the balance before and the balance after to the final string.
    //It does this by concatinating the first portion of the sentence with the balance, then
    //adding this to the final string
    char balance[25] = "Account Balance Before: ";
    strcat(balance,splitStrings[3]);
    strcat(finalString,balance);
    strcat(finalString,"\n");
    char balancetwo[25] = "Account Balance After: ";
    strcat(balancetwo,splitStrings[4]);
    strcat(finalString,balancetwo);
    printf("\nThe Transaction Summary:\n");
    printf("%s\n\n", finalString);

    /* close the socket */

    close (sock_client);
}
