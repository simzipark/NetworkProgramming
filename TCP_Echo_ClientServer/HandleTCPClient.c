#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket){
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;

    FILE *fp;

    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    while(recvMsgSize > 0){

        echoBuffer[recvMsgSize] = '\0';
        printf("Client >> %s\n", echoBuffer);

        if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");

            if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed");

        fp = fopen("echo_history.log", "a+");
        if(fp == NULL)
            puts("FILE OPEN ERROR");
        
        fprintf(fp, "%s", echoBuffer);

        if(fclose(fp) == EOF)
            printf("FILE CLOSE ERROR");
    }
    close(clntSocket);
}