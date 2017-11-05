/*
* assignment #4
* fork() ver.
*/
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5
#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket);

int AcceptTCPConnection(int servSock)
{
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);
    
    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
           &clntLen)) < 0)
        DieWithError("accept() failed");
    
    /* clntSock is connected to a client! */
    
    printf("client ip : %s\n", inet_ntoa(echoClntAddr.sin_addr));
    printf("port : %d\n", echoClntAddr.sin_port);

    return clntSock;
}

int main(int argc, char *argv[]){
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort;
    unsigned int clntLen;
    unsigned int childProcCount = 0;    //num of childe processes

    //create server socket
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() falied");  

    //setting socket address & port
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(4000);

    //bind & listen
    if(bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");    

    if(listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed"); 

    //client accept
    for(;;){
        clntSock = AcceptTCPConnection(servSock);

        pid_t processID = fork();

        if(processID <0)
            DieWithError("fork() failed");
        else if(processID ==0){
            close(servSock);
            HandleTCPClient(clntSock);

            exit(0);
        }

        /*
        clntLen = sizeof(echoClntAddr);
        if((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) <= 0)
            DieWithError("accept() failed");
        
        printf("client ip : %s\n", inet_ntoa(echoClntAddr.sin_addr));
        printf("port : %d\n", echoClntAddr.sin_port);

        HandleTCPClient(clntSock);
        */       
    }
}