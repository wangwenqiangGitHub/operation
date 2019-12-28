#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
int main()
{
	struct addrinfo hints,*res=NULL;
	int rc;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=PF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	rc=getaddrinfo("127.0.0.1","123",&hints,&res);
	if (rc != 0) {
		perror("getaddrinfo");
		exit(1);
	}
	else
		printf("getaddrinfo success\n");
}

