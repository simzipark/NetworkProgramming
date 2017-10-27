#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>


#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
int EchoChattingHandle(int sock);

int main(int argc,char *argv[])
{
  struct sockaddr_in server;
  struct stat obj;
  int sock;
  int choice;
  char buf[100], command[5], filename[20], *f;
  int k, size, status;
  int filehandle;
  char str[1024];
  char *echoString = str;
  char echoBuffer[RCVBUFSIZE]; 
  unsigned int echoStringLen;
  int bytesRcvd, totalBytesRcvd;
  int flag = 1;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
	{
		printf("socket creation failed");
		exit(1);
	}

  server.sin_family = AF_INET;
  server.sin_port = htons(5000);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  k = connect(sock,(struct sockaddr*)&server, sizeof(server));

  printf("server ip : 127.0.0.1\n");
  //printf("server ip : %d\n", inet_ntoa(server.sin_addr));
  printf("port : %d\n", server.sin_port);

  //ECHO CHATTING START!!!
  EchoChattingHandle(sock);

  if(k == -1)
	{
		printf("Connect Error");
		exit(1);
	}

  int i = 1;
  while(1)
  {
			printf("Welcome to Socket FT client!\n");
			printf("ftp command [1)put   2)get   3)ls   4)rls   5)exit] -> ");
			scanf("%d", &choice);
			
      switch(choice)
			{
				case 1:	//get : client의 파일을 server에 놓기

					printf("Enter filename to put to server: ");
					scanf("%s", filename);

					filehandle = open(filename, O_RDONLY);
					if(filehandle == -1)
					{
						printf("No such file on the local directory\n\n");
						break;
					}

					strcpy(buf, "put ");
					strcat(buf, filename);
					send(sock, buf, 100, 0);
					stat(filename, &obj);
					size = obj.st_size;
					send(sock, &size, sizeof(int), 0);
					sendfile(sock, filehandle, NULL, size);
					recv(sock, &status, sizeof(int), 0);

					if(status)
						printf("File stored successfully\n");
					else
						printf("File failed to be stored to remote machine\n");
					break;


				case 2:	//put : server에서 client로 파일 가져오기
					printf("filename to get from server -> ");
					scanf("%s", filename);
					strcpy(buf, "get ");
					strcat(buf, filename);
					send(sock, buf, 100, 0);
					recv(sock, &size, sizeof(int), 0);

					if(!size)
					{
						printf("No such file on the remote directory\n\n");
						break;
					}

					printf("\nSending ==> ############\n");
					f = malloc(size);
					recv(sock, f, size, 0);

					while(1)
					{
							filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
							if(filehandle == -1)
							{
								sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client								
							}
							else break;
					}
					/*
					test.txt(7554 bytes) uploading success to 127.0.0.1
					*/
					printf("\n%s(%d bytes) uploading success to 127.0.0.1\n\n", filename, size);

					write(filehandle, f, size, 0);
					close(filehandle);
					strcpy(buf, "cat ");
					strcat(buf, filename);
					system(buf);
					break;


				case 3:	//ls
					strcpy(buf, "ls");
					send(sock, buf, 100, 0);
					recv(sock, &size, sizeof(int), 0);
					f = malloc(size);
					recv(sock, f, size, 0);
					filehandle = creat("temp.txt", O_WRONLY);
					write(filehandle, f, size, 0);
					close(filehandle);
					printf("The remote directory listing is as follows:\n");
					system("cat temp.txt");
					printf("\n");
					break;
					
				case 4:	//rls
					strcpy(buf, "rls");
					send(sock, buf, 100, 0);
					recv(sock, buf, 100, 0);
					printf("The rls of the remote directory is: %s\n", buf);
					break;
					
				case 5:	//exit
					strcpy(buf, "exit");
					send(sock, buf, 100, 0);
					recv(sock, &status, 100, 0);
					if(status)
					{
						printf("Server closed\nQuitting..\n");
						EchoChattingHandle(sock);
						break;
					}
					printf("Server failed to close connection\n");
								

					
			}//switch
  }//while
}
