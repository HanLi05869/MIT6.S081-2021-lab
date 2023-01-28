#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//it is vital to include this
#include "kernel/fs.h"

char* getFirstName(char* path)
{
	char* p;

	for(p=path+strlen(path);p>=path && *p !='/';p--);
	p++;
	return p;
}

void findFile(char* path,char* filename)
{
	char* curname = getFirstName(path);
	int fd = open(path,0);
	if(fd < 0){
		return;
	}
	struct stat st;
	struct dirent de;
	if(fstat(fd,&st)<0){
		close(fd);
		return;
	}
	char buf[512];
	switch(st.type){
		case T_FILE:
			if(strcmp(curname,filename) == 0){
				printf("%s\n",path);
			}
			break;
		case T_DIR:
			if(strcmp(".",curname)==0 || strcmp("..",curname)==0)	return;

			if(strlen(path)+1+DIRSIZ+1>sizeof(buf)){printf("too long\n");break;}
			strcpy(buf,path);
			char* p=buf+strlen(path);
			*p='/';
			p++;
			while((read(fd,&de,sizeof(de)) == sizeof(de))){
				memmove(p,&de.name,DIRSIZ);
				findFile(buf,filename);
			}
			break;
	}
	close(fd);
}

int main(int argc,char* argv[])
{
	if(argc != 3){
		printf("error!\n");
		exit(1);
	}

	if(strcmp(".",argv[1])==0||strcmp("..",argv[1])==0){
		int fd;
		char buf[30];
		strcpy(buf,argv[1]);
		int len = strlen(argv[1]);
		buf[len++]='/';
		if((fd = open(argv[1],0))<0){
			printf("error!\n");
			exit(1);
		}
		struct dirent de;
		while(read(fd, &de, sizeof(de))==sizeof(de)){
			if(strcmp(".",de.name)==0 || strcmp("..",de.name)==0)	continue;
			if(de.inum==0)	continue;
			memmove(buf+len,de.name,DIRSIZ);
			findFile(buf, argv[2]);
		}
		close(fd);
	}
	else{
		findFile(argv[1],argv[2]);
	}
	exit(0);
}
