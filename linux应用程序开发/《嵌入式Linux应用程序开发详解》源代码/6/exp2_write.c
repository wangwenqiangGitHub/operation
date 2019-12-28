#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "serial.h"

int main(void) 
{
	int fd;
	int nwrite,i;
	char buff[]="Hello\n";
	if((fd=open_port(fd,1))<0){ 
		perror("open_port error");
		return;
	}
	if((i=set_opt(fd,115200,8,'N',1))<0){ 
		perror("set_opt error");
		return;
	}
	printf("fd=%d\n",fd);
	nwrite=write(fd,buff,8);
	printf("nwrite=%d\n",nwrite);
	close(fd);
	return;
}

