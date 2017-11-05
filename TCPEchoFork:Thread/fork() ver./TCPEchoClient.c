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

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort; 
    char *servIP;
    char str[1024];
    char *echoString = str;
    char echoBuffer[RCVBUFSIZE]; 
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;
    int flag = 1;

    //create socket
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    echoServAddr.sin_port = htons(4000);

    //connect to server
    if(connect(sock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    
    printf("server ip : %s\n", inet_ntoa(echoServAddr.sin_addr));
    printf("port : %d\n", echoServAddr.sin_port);

    //start echo chatting
    for(;;){
        //send data
        if(flag){
            strcpy(str, "hello");
            str[strlen(str)] = '\0';
            flag=0;
        }
        else{
            printf(">> ");
            gets(str);  
            str[strlen(str)] = '\0';
        }        

        echoStringLen = strlen(echoString);
        if(send(sock, echoString, echoStringLen, 0) != echoStringLen)
            DieWithError("send() sent a different number of bytes than expected");
        printf("msg-> %s\n", echoString);

        //data receive
        totalBytesRcvd = 0;
        while(totalBytesRcvd < echoStringLen){
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd += bytesRcvd;
            echoBuffer[bytesRcvd] = '\0';
        }
        

        if(strcmp(echoBuffer, "/quit")==0){
            printf("Close Client Socket\n");
            close(sock);
            exit(0);
        }    
        else{
            printf("msg<- %s\n", echoBuffer);
        }
    }
}