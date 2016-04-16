#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>


#define PORT 5001

#define MaxBufferLength 1024 // set the size of data you want to recieve from Server


void error(const char *msg)
{
    perror(msg);
    exit(2);
}



int main()
{
  int sockFd, newsockfd, bytesRead= 1, bytesSent;

  char buffer[MaxBufferLength];

  socklen_t clilen;

  struct sockaddr_in server, client, cli_addr;


  sockFd = socket(AF_INET, SOCK_STREAM, 0);

  bzero((char *) &server, sizeof(server));
  server.sin_port= htons(PORT);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  //server.sin_addr.s_addr = INADDR_ANY;

  if(sockFd < 0)
      printf("Unable to open socket\n");

  if (bind(sockFd, (struct sockaddr *) &server,
            sizeof(server)) < 0)  
            error("ERROR on binding");
  
   listen(sockFd,5);
   clilen = sizeof(cli_addr);
   newsockfd = accept(sockFd, 
               (struct sockaddr *) &cli_addr, 
               &clilen);
  
   if (newsockfd < 0)  
        error("ERROR on accept");


  //int connectionSocket = connect(sockFd, (struct sockaddr *) &server, sizeof(struct sockaddr) );

  //if(connectionSocket < 0)
  //    perror("connection not established\n");


  int fd = open("helloworlds.txt",O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);  

  if(fd == -1)
      perror("couldn't openf iel");

  while(bytesRead > 0)
  {           

      bytesRead = recv(newsockfd, buffer, MaxBufferLength, 0);

      if(bytesRead == 0)
      {

          break;
      }

      printf("bytes read %d\n", bytesRead);

      printf("receivnig data\n");

      bytesSent = write(fd, buffer, bytesRead);


      printf("bytes written %d\n", bytesSent);

      if(bytesSent < 0)
          perror("Failed to send a message");

  }


  close(fd);

  close(sockFd);

  return 0;

}
