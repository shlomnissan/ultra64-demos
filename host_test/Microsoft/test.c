#include <stdio.h>
#include <stdlib.h>
#include <ultrahost.h>
#include <string.h>

#define BUF_SIZE 0x100000

char buf[BUF_SIZE];
char buf_bak[BUF_SIZE];

#ifdef WIN32
#define GLOAD_FILE	"../game.n64"
#else
#define GLOAD_FILE	"rom"
#endif

int main(void)
{
    int i,hfd;

    printf("  HostIO Test Program\n");

    if((hfd=uhOpenGame("PARTNER-N64"))<0){
	printf("\nError uhOpenGame\n");
	exit(-1);
    }

    if(uhGload(hfd,GLOAD_FILE)!=0){
	printf("\nCan't %s",GLOAD_FILE);
	exit(-1);
    };

    for(i=0;i<BUF_SIZE;++i){
	buf[i]=buf_bak[i]=rand();
    }

    printf("\n	WriteRamrom");

    if(uhWriteRamrom(hfd,(void *)0x1234,buf,BUF_SIZE)!=BUF_SIZE){
	printf("\nError uhWriteRamrom\n");
	exit(-1);
    }

    memset(buf,0,BUF_SIZE);

    printf("\n	ReadRamrom");

    if(uhReadRamrom(hfd,(void *)0x1234,buf,BUF_SIZE)!=BUF_SIZE){
	printf("\nError uhReadRamrom\n");
	exit(-1);
    }

    for(i=0;i<BUF_SIZE;++i){
	if(buf[i]!=buf_bak[i]){
	    printf("\nCompare error uhWriteRamrom,uhReadRamrom\n");
	}
    }

    printf("\n	WriteGame");

    if(uhWriteGame(hfd,buf,BUF_SIZE)!=BUF_SIZE){
	printf("\nError uhWriteGame\n");
	exit(-1);
    }

    memset(buf,0,BUF_SIZE);

    printf("\n	ReadGame");

    if(uhReadGame(hfd,buf,BUF_SIZE)!=BUF_SIZE){
	printf("\nError uhReadGame\n");
	exit(-1);
    }

    for(i=0;i<BUF_SIZE;++i){
	if(buf[i]!=buf_bak[i]){
	    printf("\nCompare error uhWriteGame,uhReadGame\n");
	}
    }

    printf("\n	Loop Test(100)");

    for(i=0;i<100;++i){
	if(uhWriteGame(hfd,buf,10)!=10){
	    printf("\nError uhWriteGame\n");
	    exit(-1);
	}
	if(uhReadGame(hfd,buf,10)!=10){
	    printf("\nError uhReadGame\n");
	    exit(-1);
	}
    }

    uhCloseGame(hfd);

    printf("\n\n  All test OK\n");

    return 0;
}
