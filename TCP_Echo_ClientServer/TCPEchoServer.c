#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket);

int main(int argc, char *argv[]){
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort;
    unsigned int clntLen;


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }
    
    echoServPort = atoi(argv[1]);

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() falied");  

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if(bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");    

    if(listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed"); 


    for(;;){
        clntLen = sizeof(echoClntAddr);
        if((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) <= 0)
            DieWithError("accept() failed");
        
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        HandleTCPClient(clntSock);

        /*
        클라이언트가 전달하는 메시지가 파일형태로 저장되도록
        파일명 : echo_history.log
        지속적으로 추가되어 저장되도록 구현
        */

        
    }


    /*
    clntLen = sizeof(echoClntAddr);

    if(accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen) <= 0)
        DieWithError("accept() failed");
    
    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    for(;;){
        HandleTCPClient(clntSock);
    }
    */
}