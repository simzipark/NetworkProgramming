/*
* 과제2
* echo chatting
*/
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket){
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    int flag = 1;
    FILE *fp;


    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    while(recvMsgSize > 0){

        echoBuffer[recvMsgSize] = '\0';
        if(strcmp(echoBuffer, "/quit")==0){
            printf("~Close Client Socket!\n");
            close(clntSocket);
        }    
        else{
            printf("msg<- %s\n", echoBuffer);
        }

        //first data send
        if(flag){
            strcpy(echoBuffer, "hi");
            echoBuffer[strlen(echoBuffer)] = '\0';
            flag = 0;
        }
        else{
            echoBuffer[recvMsgSize] = '\0';
        }

        if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");
        printf("msg-> %s\n", echoBuffer);

            if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed");

        //write log
        fp = fopen("echo_history.log", "a+");
        if(fp == NULL)
            puts("FILE OPEN ERROR");
        
        fprintf(fp, "%s", echoBuffer);

        if(fclose(fp) == EOF)
            printf("FILE CLOSE ERROR");
    }
    close(clntSocket);
}