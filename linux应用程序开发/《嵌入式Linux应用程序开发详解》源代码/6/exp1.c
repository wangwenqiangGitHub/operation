#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lock_set(int fd,int type);

int main(void)
{
	int fd,nwrite,nread,len;
	char *buff="Hello\n";
	char buf_r[100];
	buf_r[100] = '\0';
	len=strlen(buff);
	fd=open("hello",O_RDWR | O_CREAT, 0666);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	lock_set(fd, F_WRLCK);
	if((nwrite=write(fd,buff,len))==len){
		printf("write success\n");
	}
	getchar();
	lock_set(fd, F_UNLCK);
	getchar();
	lock_set(fd, F_RDLCK);
	lseek(fd,0,SEEK_SET);
	if((nread=read(fd,buf_r,len))==len){
		printf("read:%s\n",buf_r);
	}
	getchar();
	lock_set(fd, F_UNLCK);
	getchar();
	close(fd);
	exit(0);
}

void lock_set(int fd, int type)
{
        struct flock lock;
        lock.l_whence = SEEK_SET;//¸³ֵlock½ṹÌ
        lock.l_start = 0;
        lock.l_len =0;
        while(1){
                lock.l_type = type;
                if((fcntl(fd, F_SETLK, &lock)) == 0){
                if( lock.l_type == F_RDLCK )
                        printf("read lock set by %d\n",getpid());
                else if( lock.l_type == F_WRLCK )
                        printf("write lock set by %d\n",getpid());
                else if( lock.l_type == F_UNLCK )
                        printf("release lock by %d\n",getpid());
                return;
                }
                fcntl(fd, F_GETLK,&lock);
                if(lock.l_type != F_UNLCK){
                        if( lock.l_type == F_RDLCK )
                                printf("read lock already set by %d\n",lock.l_pid);
                        else if( lock.l_type == F_WRLCK )
                                printf("write lock already set by %d\n",lock.l_pid);
                        getchar();
                }
        }
}

