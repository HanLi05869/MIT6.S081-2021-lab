#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
	//0 is read
	//1 is write
	//p1 is child -> parent
	//p2 is parent -> child
	int p1[2];
	int p2[2];
	pipe(p1);
	pipe(p2);

	int pid;
	if((pid = fork()) < 0){
		printf("fork error\n");
		exit(1);
	}
	
	if(pid == 0)
	{
		//this is child
		close(p1[0]);
		close(p2[1]);
		char buf[1];
		if(read(p2[0], buf, 1)>0){
			printf("%d: received ping\n",getpid());
		}
		write(p1[1], buf, 1);
		close(p1[1]);
		exit(0);
	}
	else
	{
		//this is parent
		close(p2[0]);
		close(p1[1]);
		char buf[1];
		buf[0]='p';
		write(p2[1], buf, 1);
		close(p2[1]);
		if(read(p1[0], buf, 1)>0)
		{
			printf("%d: received pong\n", getpid());
		}
		int statue;
		wait(&statue);
		exit(0);
	}
}
