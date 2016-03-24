/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <fstream>
#include <iostream>


void error(const char *msg)
{
    perror(msg);
    exit(2);
}

int main(int argc, char *argv[])
{
     std::clock_t start;
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     
     std::ifstream portnum_file;
     std::string server_pnum;
     portnum_file.open("server_portnum.txt", std::ios::in);
     if(getline(portnum_file, server_pnum))
        portno = atoi(server_pnum.c_str());

     // if (argc < 2) {
     //     fprintf(stderr,"ERROR, no port provided\n");
     //     exit(1);
     // }
     
     start = std::clock();

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     // portno = atoi(argv[1]);
     // portno = 5555;

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,256);

     
 	 n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
 
     close(newsockfd);
     close(sockfd);

    std::ofstream port_number_file;
    port_number_file.open("server_portnum.txt");
    int pnum = atoi(buffer) + 1;
    port_number_file << pnum << std::endl;

     return 2; 
}

