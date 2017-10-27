#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXPENDING 5
#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);
int HandleTCPClient(int clntSocket);

int main(int argc,char *argv[])
{
  struct sockaddr_in server, client;
  struct stat obj;
  int sock1, sock2;
  char buf[100], command[5], filename[20];
  int k, i, size, len, c;
  int filehandle;

  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if(sock1 == -1)
  {
      printf("Socket creation failed");
      exit(1);
  }

  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(5000);

  k = bind(sock1,(struct sockaddr*)&server,sizeof(server));
  if(k == -1)
  {
    printf("Binding error");
    exit(1);
  }

  k = listen(sock1,1);
  if(k == -1)
    {
      printf("Listen failed");
      exit(1);
    }

  len = sizeof(client);
  sock2 = accept(sock1,(struct sockaddr*)&client, &len);

  printf("client ip : 127.0.0.1\n");
  //printf("client ip : %d\n", inet_ntoa(client.sin_addr));
  printf("port : %d\n", client.sin_port);

  //echo chatting
  HandleTCPClient(sock2); 

  //FTP Service
  i = 1;
  while(1)
  {
      recv(sock2, buf, 100, 0);
      sscanf(buf, "%s", command);
      if(!strcmp(command, "ls"))
      {
        system("ls >temps.txt");
        i = 0;
        stat("temps.txt",&obj);
        size = obj.st_size;
        send(sock2, &size, sizeof(int),0);
        filehandle = open("temps.txt", O_RDONLY);
        sendfile(sock2,filehandle,NULL,size);
      }

      else if(!strcmp(command,"get"))
      {
        sscanf(buf, "%s%s", filename, filename);
        stat(filename, &obj);
        filehandle = open(filename, O_RDONLY);
        size = obj.st_size;
        if(filehandle == -1)
            size = 0;
        send(sock2, &size, sizeof(int), 0);
        if(size)
        sendfile(sock2, filehandle, NULL, size);
          
      }

      else if(!strcmp(command, "put"))
      {
        int c = 0, len;
        char *f;
        sscanf(buf+strlen(command), "%s", filename);
        recv(sock2, &size, sizeof(int), 0);
        i = 1;
        while(1)
        {
            filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
            if(filehandle == -1)
            {
              sprintf(filename + strlen(filename), "%d", i);
            }   
            else
              break;
        }
        f = malloc(size);
        recv(sock2, f, size, 0);
        c = write(filehandle, f, size);
        close(filehandle);
        send(sock2, &c, sizeof(int), 0);
      }

      else if(!strcmp(command, "pwd"))
      {
        system("pwd>temp.txt");
        i = 0;
        FILE*f = fopen("temp.txt","r");
        while(!feof(f))
        buf[i++] = fgetc(f);
        buf[i-1] = '\0';
        fclose(f);
        send(sock2, buf, 100, 0);
      }

      else if(!strcmp(command, "exit"))
      {
        printf("FTP server quitting..\n");
        i = 1;
        send(sock2, &i, sizeof(int), 0);
        HandleTCPClient(sock2);
       
      }
  }//while
  return 0;
}
