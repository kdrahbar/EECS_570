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
    // if (argc < 3) {
    //    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    //    exit(0);
    // }

    // portno = atoi(argv[2]);

    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);

    int day = aTime->tm_mday;
    int month = aTime->tm_mon + 1; // Month is 0 – 11, add 1 to get a jan-dec 1-12 concept
    int year = aTime->tm_year + 1900; // Year is # years since 1900
    int hour=aTime->tm_hour;

    // portno = 5555;
    char * portno_str = argv[1];
    printf("starting on %s\n",portno_str);
    portno = atoi(portno_str);
    start = std::clock();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    inet_pton(AF_INET, "10.128.0.2", &ipv4addr);
    server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    // server = gethostbyname(argv[1]);
    // server = gethostbyname("104.197.119.249");
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

    pFile = fopen ("sample_message.txt" , "r");
    if (pFile == NULL) perror ("Error opening file");
    
    bzero(buffer,256);
    fgets(buffer,255, pFile);
    // n = write(sockfd,buffer,strlen(buffer));
    n = write(sockfd,portno_str,strlen(portno_str));
    
    if (n < 0) 
         error("ERROR writing to socket");
    
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    
    //printf("%s\n",buffer);
    close(sockfd);
    double rountrip_time = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
    
    std::ofstream logging;
    logging.open("client_timings.txt", std::ios_base::app);

    logging << hour << " " << rountrip_time << " ms" << std::endl;

    
    return 0;
}
