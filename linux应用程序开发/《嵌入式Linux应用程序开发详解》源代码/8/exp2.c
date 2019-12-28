#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define BUFSZ 2048
int main()
{
	int shmid,i,fd,nwrite,nread;
	char *shmadd;
	char buf[5];
	buf[5] = '\0';
	if((shmid=shmget(IPC_PRIVATE,BUFSZ,0666))<0){
		perror("shmget");
		exit(1);
	}
	else
		printf("created shared-memory: %d\n",shmid);
	if((shmadd=shmat(shmid,0,0))<(char *)0){
		perror("shmat");
		exit(1);
	}
	else
		printf("attached shared-memory\n");
	shmadd="Hello";
	if((fd = open("share",O_CREAT | O_RDWR,0666))<0){
		perror("open");
		exit(1);
	}
	else
		printf("open success!\n");
	if((nwrite=write(fd,shmadd,5))<0){
		perror("write");
		exit(1);
	}
	else
		printf("write success!\n");
	lseek( fd, 0, SEEK_SET );
	if((nread=read(fd,buf,5))<0){
		perror("read");
		exit(1);
	}
	else
		printf("read %d form file:%s\n",nread,buf);
	if((shmdt(shmadd))<0){
		perror("shmdt");
		exit(1);
	}
	else
		printf("deleted shared-memory\n");
	exit(0);
}

