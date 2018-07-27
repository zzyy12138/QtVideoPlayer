
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "pub.h"

int allow_postfile = 0;//是否允许上传文件，0不允许，1允许

int main(int arg, char *args[])
{	
	/*
	char s[100] = "aaa.cgbbb.gi";
	char *tmp = strstr(s, ".cgi");
	printf("%s\n", tmp);
	return 0;
	*/
	if (arg < 2)//如果没有参数，main函数返回
	{
		printf("usage:%s port\n", args[0]);
		return EXIT_FAILURE;
	}

	int iport = atoi(args[1]);//将第一个参数转化为整数
	if (iport == 0)
	{
		printf("port %d is invalid\n", iport);
		return EXIT_FAILURE;
	}

	if (arg >= 3)
		allow_postfile = atoi(args[2]);
	
	int st = socket_create(iport);//建立listen socket
	if (st == 0)
		return EXIT_FAILURE;
	
	printf("myhttp begin\n");
	writelog("myhttp begin");
	setdaemon();//设置进程为daemon状态
	signal1(SIGINT, catch_Signal);	//捕捉SIGINT信号	
	signal1(SIGPIPE, catch_Signal);
	signal1(SIGALRM, catch_Signal);
	signal1(SIGTERM, catch_Signal);
	socket_accept(st);
	close(st);
	printf("myhttp end\n");
	writelog("myhttp end");
	return EXIT_SUCCESS;
}

