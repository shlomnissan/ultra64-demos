/**************************************************************************
 *                                                                        *
 *               Copyright (C) 1995, Silicon Graphics, Inc.               *
 *                                                                        *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright  law.  They  may not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *                                                                        *
 *************************************************************************/

/*
 * This demo shows how a typical tool will use the hostio routines. There
 * are two parts, this host indy application and the game application which
 * is in game.c. 
 *
 * The host application sends a command block which tells what to do, and
 * in the case of data transfer tells how many bytes to transfer. After the
 * command block has been sent, the game either prints something, or the
 * host and game transfer data.
 *
 * Before doing anything, we open the DEV_U64_DATA device, then fork a process
 * that will launch gload. Then prompt the user to tell us what to do. Because
 * the shell is shared by this app, and gload, printf's can come out of order. 
 * When we do a transfer in this app, we are just sending garbage buffers, but 
 * in a real app, you would send your data.
 *
 */



#include <stdlib.h>
#ifdef WIN32
#define DEV_U64_DATA	"PARTNER-N64"
#else
#include <sys/u64gio.h> 
#endif
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <ultrahost.h>
#ifndef WIN32 /* { */
#include <unistd.h>
#endif /* } */
#include "hostio.h"

int  fd;

void HandlePrint(void);
void HandleSend(void);
void HandleGet(void);

char  ReadBuf[MY_MAX_BLOCK_SIZE];
char  SendBuf[MY_MAX_BLOCK_SIZE];

#ifdef WIN32
void rev(unsigned *data)
{
    unsigned char c0,c1;
    union {
	unsigned long ld;
	unsigned char ch[4];
    } d;

    d.ld=*data;
    c0=d.ch[0];
    c1=d.ch[1];
    d.ch[0]=d.ch[3];
    d.ch[1]=d.ch[2];
    d.ch[3]=c0;
    d.ch[2]=c1;
    *data=d.ld;
}
#else
#define rev(x)
#endif

int main(int argc, char **argv)
{
#ifndef WIN32
    pid_t	pid;
#endif
    int		done = 0;
    char        cmdStr[64];


    if ((fd = uhOpenGame(DEV_U64_DATA)) == -1) 
    {
	fprintf(stderr, "host: Unable to open %s : %s\n", 
	DEV_U64_DATA, sys_errlist[errno]);
    	return(1);
    }

#ifdef WIN32
    if(uhGload(fd,"../game.n64")!=0){
	fprintf(stderr, "host: gload failed\n");
	return(-1);
    }
#else
    if ((pid = fork()) == -1) 
    {
	fprintf(stderr, "host: fork: %s\n", sys_errlist[errno]);
	return(-1);
    } 
    else if (pid == 0) 
    {
	(void)execl("/usr/sbin/gload", "/usr/sbin/gload", 0);
	fprintf(stderr, "host: execl(\"gload\") failed\n");
	return(-1);
    }
#endif

    while(!done)
    {
	printf("host: Type S for send, G for get, P for print, Q for quit\n");
	
	scanf("%s",cmdStr);
	
	switch(cmdStr[0])
	{
	    case 'P':
	    case 'p':
		HandlePrint();
		break;
	    case 'S':
	    case 's':
		HandleSend();
		break;
	    case 'G':
	    case 'g':
		HandleGet();
		break;
	    case 'Q':
	    case 'q':
		done = 1;
		break;
	    default:
		printf("host: Sorry don't understand: %s\n",cmdStr);
	}
    }
    return(0);
}

void HandlePrint(void)
{
    cmdBlk      cb;

    cb.command = CMD_PRINT_SOMETHING;

    rev((unsigned *)&cb.command);

    if (uhWriteGame(fd, &cb, sizeof(cmdBlk)) == -1) 
    {
	fprintf(stderr, "host: uhWriteGame: %s\n", sys_errlist[errno]);
	return;
    }
    
}

void HandleSend(void)
{
    cmdBlk      cb;
    char        sizeStr[64];
    int         blkSize;

    printf("host: Send how many bytes?  (1 - %d) ", MY_MAX_BLOCK_SIZE);

    scanf("%s",sizeStr);
    blkSize = atoi(sizeStr);

    if(blkSize < 1 || blkSize > MY_MAX_BLOCK_SIZE)
    {
	printf("host: Sorry, block size is out of range %d bytes\n");
	blkSize++;
	return;
    }
    
    cb.command = CMD_WRITE_GAME;
    cb.blkSize = blkSize;

    rev((unsigned *)&cb.command);
    rev((unsigned *)&cb.blkSize);

    if (uhWriteGame(fd, &cb, sizeof(cmdBlk)) == -1) 
    {
	fprintf(stderr, "host: uhWriteGame: %s\n", sys_errlist[errno]);
	return;
    }
    if (uhWriteGame(fd, SendBuf, blkSize) == -1) 
    {
	fprintf(stderr, "host: uhWriteGame: %s\n", sys_errlist[errno]);
	return;
    }

    printf("host: Sent %d bytes\n",blkSize);

}

void HandleGet(void)
{
    cmdBlk      cb;
    char        sizeStr[64];
    int         blkSize;

    printf("host: Get how many bytes?  (1 - %d) ", MY_MAX_BLOCK_SIZE);

    scanf("%s",sizeStr);
    blkSize = atoi(sizeStr);

    if(blkSize < 1 || blkSize > MY_MAX_BLOCK_SIZE)
    {
	printf("host: Sorry, block size is out of range %d bytes\n");
	blkSize++;
	return;
    }
    
    cb.command = CMD_READ_GAME;
    cb.blkSize = blkSize;

    rev((unsigned *)&cb.command);
    rev((unsigned *)&cb.blkSize);

    if (uhWriteGame(fd, &cb, sizeof(cmdBlk)) == -1) 
    {
	fprintf(stderr, "host: uhWriteGame: %s\n", sys_errlist[errno]);
	return;
    }
    if (uhReadGame(fd, ReadBuf, blkSize) == -1) 
    {
	fprintf(stderr, "host: uhReadGame: %s\n", sys_errlist[errno]);
	return;
    }

    printf("host: Got %d bytes\n",blkSize);

}
