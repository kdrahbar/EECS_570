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
#include <time.h>


void error(const char *msg)
{
    perror(msg);
    exit(2);
}


int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}


int main(int argc, char *argv[])
{
    // Timing Variables
    struct timespec start, end; 

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    struct hostent *he;
    struct in_addr ipv4addr;
    struct hostent *server;

    
    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);

    int day = aTime->tm_mday;
    int month = aTime->tm_mon + 1;
    int year = aTime->tm_year + 1900;
    int hour=aTime->tm_hour;

    char * portno_str = argv[1];
//    printf("starting on %s\n",portno_str);
    portno = atoi(portno_str);
    clock_gettime(CLOCK_REALTIME, &start); /*mark start time */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    // inet_pton(AF_INET, "10.128.0.2", &ipv4addr);
    inet_pton(AF_INET, "10.128.0.3", &ipv4addr);
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
    
    int iterator = atoi(argv[2]); 
    int x = 1000000;
    int *si = &x;
    int i = 0;
    char buffer[x];
    bzero(buffer,x);
    fgets(buffer,x, pFile);
    for(i = 0; i < iterator; i++){
    //    bzero(buffer,10000);
    //    fgets(buffer,10000, pFile);    
        sendall(sockfd, buffer, si);
    }
    
    close(sockfd);
    clock_gettime(CLOCK_REALTIME, &end); /* mark the end time */ 

    uint64_t roundtrip_time;
    //roundtrip_time=end.tv_sec - start.tv_sec;   
 
    std::ofstream logging;
   logging.open("client_timings.txt", std::ios_base::app);

	if (iterator < 100){    
    roundtrip_time=end.tv_nsec - start.tv_nsec;   
logging << x*iterator << " " << roundtrip_time << " ns" << std::endl;
}
	else{

    roundtrip_time=end.tv_sec - start.tv_sec;   
logging << x*iterator << " " << roundtrip_time << " s" << std::endl;
	}
    printf("%llu seconds\n", (long long unsigned int) roundtrip_time);

    
    return 0;
}
