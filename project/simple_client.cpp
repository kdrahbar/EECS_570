#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    struct hostent *he;
    struct in_addr ipv4addr;
    struct hostent *server;

    char buffer[256];
    // if (argc < 3) {
    //    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    //    exit(0);
    // }

    // portno = atoi(argv[2]);
    portno = 5555;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    inet_pton(AF_INET, "104.197.119.249", &ipv4addr);
    server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    // server = gethostbyname(argv[1]);
    // server = gethostbyname("104.197.119.249");
    if (server == NULL) {
        error("ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	char* addr = "104.197.119.249";

    //bcopy((char *)server->h_addr, 
    bcopy((char *)addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         strlen(addr));
         //server->h_length);

    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    
    FILE * pFile;

    pFile = fopen ("sample_message.txt" , "r");
    if (pFile == NULL) perror ("Error opening file");
    
    bzero(buffer,256);
    fgets(buffer,255, pFile);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}
