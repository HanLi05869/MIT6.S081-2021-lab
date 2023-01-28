#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void ChildFunc(int* pi)
{
	int num;
	if(read(pi[0],&num,sizeof(int)) < sizeof(int))
		exit(0);
	printf("prime %d\n",num);

	int p[2];
	if(pipe(p) < 0)
	{
		printf("create pipe error\n");
		exit(1);
	}
	int pid;
	if((pid = fork())<0)
	{
		printf("fork error!\n");
		exit(1);
	}
	if(pid == 0)
	{
		//child
		close(pi[0]);
		close(p[1]);
		ChildFunc(p);
	}
	else
	{
		//parent
		close(p[0]);
		int n1;
		while((read(pi[0],&n1,sizeof(int))) == sizeof(int)){
			if(n1 % num != 0){
				if(write(p[1], &n1, sizeof(int))!= sizeof(int)){
					printf("error\n");
					exit(1);
				}
			}
		}
		close(p[1]);
		close(pi[0]);
		int statue;
		wait(&statue);
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	int p[2];
	if(pipe(p)<0){
		printf("error\n");
		exit(1);
	}
	int pid;
	if((pid = fork())<0)
	{
		exit(1);
	}
	if(pid ==0){
		//child
		close(p[1]);
		ChildFunc(p);
		exit(0);
	}
	else
	{
		//father
		close(p[0]);
		for(int i=2;i<=35;++i){
			if(write(p[1], &i, sizeof(int))!=sizeof(int)){
				printf("error\n");
				exit(1);
			}
		}
		close(p[1]);
		int statue;
		wait(&statue);
		exit(0);
	}
}
