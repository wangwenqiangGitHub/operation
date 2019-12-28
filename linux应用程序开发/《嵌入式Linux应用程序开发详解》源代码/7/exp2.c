#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>
#define MAXFILE 65535
int main(void)
{
	pid_t child1,child2;
	int i;
	child1 = fork();
	if( child1 == -1 ){
		perror("child1 fork");
		exit(1);
	}
	else if( child1 > 0 )
		exit( 0 );
	openlog("exc2_info", LOG_PID, LOG_DAEMON);
	setsid();
	chdir( "/" );
	umask( 0 );
	for( i = 0 ; i < MAXFILE ; i++ )
	{
		close( i );
	}
	child2 = fork();
	if( child2 == -1 ){
		perror("child2 fork");
		exit(1);
	}
	else if( child2 == 0 ){
		syslog( LOG_INFO, " child2 will sleep for 10s ");
		sleep(10);
		syslog( LOG_INFO, " child2 is going to exit! ");
		exit(0);
	}
	else{
		waitpid( child2, NULL, 0);
		syslog( LOG_INFO , " child1 noticed that child2 has exited " );
		closelog();
		while(1){
			sleep(10);
		}	
	}
}

