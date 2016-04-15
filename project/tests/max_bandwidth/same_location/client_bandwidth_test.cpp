#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <ctime>
#include <fstream>

void error(const char *msg)
{
    perror(msg);
    exit(2);
}

int main(int argc, char *argv[])
{
    std::clock_t    start;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    struct hostent *he;
    struct in_addr ipv4addr;
    struct hostent *server;

    char buffer[256];
    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);

    int day = aTime->tm_mday;
    int month = aTime->tm_mon + 1;
    int year = aTime->tm_year + 1900;
    int hour=aTime->tm_hour;

    char * portno_str = argv[1];
    printf("starting on %s\n",portno_str);
    portno = atoi(portno_str);
    start = std::clock();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    inet_pton(AF_INET, "10.128.0.2", &ipv4addr);
    server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    if (server == NULL) {
        error("ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    
    FILE * pFile;

    pFile = fopen ("1MB_rand.txt" , "r");
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
    
    close(sockfd);
    double rountrip_time = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
    
    std::ofstream logging;
    logging.open("client_timings.txt", std::ios_base::app);

    logging << hour << " " << rountrip_time << " ms" << std::endl;

    
    return 0;
}
