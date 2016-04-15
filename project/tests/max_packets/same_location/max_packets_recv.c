#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <time.h>

#if defined(__dietlibc__)
#include <strings.h>
#endif

struct errmsg {
	char *msg;
	int size;
	int len;
};

const int zero = 0;
const int one = 1;

static int mtu = 1500;
static int count = 1;
static char *address = "";
char *payload;

/* converts str in the form [<ipv4>|<ipv6>|<hostname>]:port to struct sockaddr_storage.
 * Returns < 0 with err set in case of error.
 */
int addr_to_ss(char *str, struct sockaddr_storage *ss, struct errmsg *err)
{
	char *range;

	/* look for the addr/port delimiter, it's the last colon. */
	if ((range = strrchr(str, ':')) == NULL) {
		err->len = snprintf(err->msg, err->size, "Missing port number: '%s'\n", str);
		return -1;
	}	    

	*range++ = 0;

	memset(ss, 0, sizeof(*ss));

	if (strrchr(str, ':') != NULL) {
		/* IPv6 address contains ':' */
		ss->ss_family = AF_INET6;
		((struct sockaddr_in6 *)ss)->sin6_port = htons(atoi(range));

		if (!inet_pton(ss->ss_family, str, &((struct sockaddr_in6 *)ss)->sin6_addr)) {
			err->len = snprintf(err->msg, err->size, "Invalid server address: '%s'\n", str);
			return -1;
		}
	}
	else {
		ss->ss_family = AF_INET;
		((struct sockaddr_in *)ss)->sin_port = htons(atoi(range));

		if (*str == '*' || *str == '\0') { /* INADDR_ANY */
			((struct sockaddr_in *)ss)->sin_addr.s_addr = INADDR_ANY;
			return 0;
		}

		if (!inet_pton(ss->ss_family, str, &((struct sockaddr_in *)ss)->sin_addr)) {
			struct hostent *he = gethostbyname(str);

			if (he == NULL) {
				err->len = snprintf(err->msg, err->size, "Invalid server name: '%s'\n", str);
				return -1;
			}
			((struct sockaddr_in *)ss)->sin_addr = *(struct in_addr *) *(he->h_addr_list);
		}
	}

	return 0;
}

/*
 * returns the difference, in microseconds, between tv1 and tv2, which must
 * be ordered.
 */
static inline long long tv_diff(struct timeval *tv1, struct timeval *tv2)
{
        long long ret;
  
        ret = (long long)(tv2->tv_sec - tv1->tv_sec) * 1000000LL;
	ret += tv2->tv_usec - tv1->tv_usec;
        return ret;
}

int main(int argc, char **argv)
{
	int fd;
	struct sockaddr_storage ss;
	struct errmsg err;
	char *prog = *argv;
	struct timeval last;

	err.len = 0;
	err.size = 100;
	err.msg = malloc(err.size);

	--argc; ++argv;

	while (argc && **argv == '-') {
		if (strcmp(*argv, "-l") == 0) {
			address = *++argv;
			argc--;
		}
		else
			break;
		argc--;
		argv++;
	}

	if (argc > 0 || !*address) {
		fprintf(stderr, "usage: %s [ -l address ]\n", prog);
		exit(1);
	}

	payload = malloc(mtu);
	if (!payload) {
		perror("malloc");
		exit(1);
	}

	memset(payload, 'A', mtu);

	if (addr_to_ss(address, &ss, &err) < 0) {
		fprintf(stderr, "%s\n", err.msg);
		exit(1);
	}

	if ((fd = socket(ss.ss_family, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) == -1) {
		perror("setsockopt(SO_REUSEADDR)");
		exit(1);
	}

	if (bind(fd, (struct sockaddr *)&ss, ss.ss_family == AF_INET6 ?
		 sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in)) == -1) {
		perror("bind");
		exit(1);
	}
	
	/* the socket is ready */
	/* and now accept and process all incoming sessions */
	count = -1;
	gettimeofday(&last, NULL);
	while (1) {
		struct sockaddr from;
		socklen_t fromlen = sizeof(from);
		long long diff;
	
		if (recvfrom(fd, payload, mtu, MSG_NOSIGNAL, &from, &fromlen) > 0)
			count++;

		if ((count & 0xfffff) == 0) {
			struct timeval now;

			gettimeofday(&now, NULL);
			diff = tv_diff(&last, &now);
			last = now;
			if (count)
				printf("1048576 packets in %lld us = %1.3f Mpps\n", diff, 1048576.0 / diff);
		}
	}
}