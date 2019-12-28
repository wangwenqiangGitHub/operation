#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *arg[]={"env",NULL};
	char *envp[]={"PATH=/tmp","USER=sunq",NULL};
        if(fork()==0){
                if(("bin/env","env",NULL,envp)<0)
                        perror("execve error!");
        }
}

