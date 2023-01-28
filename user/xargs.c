#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//to get some parameters
#include "kernel/param.h"

#define BUF_SIZE 512
int main(int argc, char* argv[])
{
	if(argc == 1){
		printf("error\n");
		exit(1);
	}

	char* args[MAXARG + 1];
	for(int i=1;i<argc;++i)
		args[i-1]=argv[i];
	char buf[BUF_SIZE];
	char* p = buf;
	// fd == 0 represent stdin
	// fd == 1 represent stdout
	// fd == 2 represent stderr
	while(read(0,p,1)==1)
	{
		if((*p)=='\n')
		{
			*p = '\0';//add \0 to let it become a valid string

			int pid;
			if((pid = fork())==0)
			{
				//child
				//will exec it
				//exec solely one thing
				args[argc-1]=buf;
				exec(argv[1],args);
				
				//normally the code wiil be replaced
				//if execute here, it encounter some problems
				printf("error\n");
				exit(0);		
			}
			else
			{
				int status;
				wait(&status);
				p = buf;
			}
		}
		else
			p++;
	}
	exit(0);
}
