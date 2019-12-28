/*process.c*/
#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>

int main()
{
	printf("The PID of this process is %d\n",getpid());
	printf("The PPID of this process is %d\n",getppid());
}

