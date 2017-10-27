#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);

int EchoChattingHandle(int sock){
    char str[1024];
    char *echoString = str;
    char echoBuffer[RCVBUFSIZE]; 
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;
    int flag = 1;

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

	if(strcmp(echoString, "FT")==0){
	   printf("ECHO FTP SERVICE!!!\n");
	   return 0;
	}
        else
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
