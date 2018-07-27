
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>
#include "work.h"
#include "pub.h"

void setdaemon()
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0)
	{
		printf("fork failed %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
	{
		exit(EXIT_SUCCESS);
	}

	if ((sid = setsid()) < 0)
	{
		printf("setsid failed %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 if (chdir("/") < 0)
	 {
	 printf("chdir failed %s\n", strerror(errno));
	 exit(EXIT_FAILURE);
	 }
	 umask(0);
	 close(STDIN_FILENO);
	 close(STDOUT_FILENO);
	 close(STDERR_FILENO);
	 */
}

void catch_Signal(int Sign)
{
	switch (Sign)
	{
	case SIGINT:
		printf("signal SIGINT\n");
		break;
	case SIGPIPE:
		printf("signal SIGPIPE\n");
	case SIGALRM:
		printf("signal SIGALRM\n");
		break;	
	case SIGTERM:
		printf("signal SIGTERM\n");
		printf("myhttp end\n");
		writelog("signal SIGTERM");
		writelog("myhttp end");
		exit(0);
		break;
	default:
		break;

	}
}

int signal1(int signo, void (*func)(int))
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	return sigaction(signo, &act, &oact);
}

int hex2dec(const char hex) //将16进制的字符转化为十进制的整数，例如：'a'转化为整数10,‘B’转化为整数11
{
	switch (hex)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'a':
		return 10;
	case 'A':
		return 10;
	case 'b':
		return 11;
	case 'B':
		return 11;
	case 'c':
		return 12;
	case 'C':
		return 12;
	case 'd':
		return 13;
	case 'D':
		return 13;
	case 'e':
		return 14;
	case 'E':
		return 14;
	case 'f':
		return 15;
	case 'F':
		return 15;
	default:
		return -1;
	}
}


//将两位16进制的字符串转化为十进制的unsigned char，例如：'10'转化为16,‘1A'转化为17
unsigned char hexstr2dec(const char *hex)
{
	return hex2dec(hex[0]) * 16 + hex2dec(hex[1]);
}

//将HTTP GET请求中的转义符号转化为标准字符,注意，空格被转义为'+'号
void httpstr2stdstr(const char *httpstr, char *stdstr)
{
	int index = 0;
	int i;
	for (i = 0; i < strlen(httpstr); i++)
	{
		if (httpstr[i] == '%')
		{
			stdstr[index] = hexstr2dec(&httpstr[i + 1]);
			i += 2;
		}
		else
		{
			stdstr[index] = httpstr[i];
		}
		index++;
	}
}

const char *getfilename(const char *path)//从完整路径中得到文件名
{
	int i;
	for(i = strlen(path); i >= 0; i--)
	{
		if (path[i] == '\\' || path[i] == '/')
			return &path[i + 1];
	}
	return path;
}

void get_file_ext(const char *filename, char *extname)//得到文件扩展名
{
	int len = strlen(filename);
	int i;
	for (i = len - 1; i >=0; i--)
	{
		if (filename[i] == '.')
		{
			strncpy(extname, &filename[i + 1], 1023);
			break;
		}
	}
}

const char *getfiletype(const char *filename) //根据扩展名返回文件类型描述
{
	////////////得到文件扩展名///////////////////
	
	char sExt[1024] = { 0 };
	get_file_ext(filename, sExt);

	////////根据扩展名返回相应描述///////////////////

	if (strncmp(sExt, "bmp", 3) == 0)
		return "image/bmp";

	if (strncmp(sExt, "gif", 3) == 0)
		return "image/gif";

	if (strncmp(sExt, "ico", 3) == 0)
		return "image/x-icon";

	if (strncmp(sExt, "jpg", 3) == 0)
		return "image/jpeg";
		
	if (strncmp(sExt, "png", 3) == 0)
		return "image/png";	
		
		
	if (strncmp(sExt, "mp3", 3) == 0)
		return "audio/mp3";	

	if (strncmp(sExt, "avi", 3) == 0)
		return "video/avi";
		
	if (strncmp(sExt, "mpg", 3) == 0)
		return "video/mpg";
		
		if (strncmp(sExt, "mp4", 3) == 0)
		return "video/mpeg4";
	
		if (strncmp(sExt, "wma", 3) == 0)
		return "audio/x-ms-wma";

	if (strncmp(sExt, "wmv", 3) == 0)
		return "video/x-ms-wmv";
		
	if (strncmp(sExt, "rm", 2) == 0)
		return "video/x-ms-rm";
		
		if (strncmp(sExt, "rmvb", 4) == 0)
		return "video/x-ms-rmvb";	
		
		
			if (strncmp(sExt, "rar", 3) == 0)
		return "application/x-rar-compressed";

	if (strncmp(sExt, "pdf", 3) == 0)
		return "application/pdf";
	
	if (strncmp(sExt, "zip", 3) == 0)
		return "application/zip";
		
	//if (strncmp(sExt, "ppt", 3) == 0)
	//	return "application/vnd.ms-powerpoint";	
	if (strncmp(sExt, "ppt", 3) == 0 || strncmp(sExt, "pptx", 4) == 0)
		return "application/x-ppt";		

	if (strncmp(sExt, "xls", 3) == 0 || strncmp(sExt, "xlsx", 4) == 0)
		return "application/ms-excel";

	if (strncmp(sExt, "doc", 3) == 0 || strncmp(sExt, "docx", 4) == 0)
		return "application/msword";
		
	if (strncmp(sExt, "dll", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "exe", 3) == 0)
		return "application/x-msdownload";	
				

	if (strncmp(sExt, "css", 3) == 0)
		return "text/css";
		
	if (strncmp(sExt, "dtd", 3) == 0)
		return "text/xml";
		
	if (strncmp(sExt, "xml", 3) == 0)
		return "text/xml";	
		
	if (strncmp(sExt, "js", 2) == 0)
		return "text/javascript";		
		
	if (strncmp(sExt, "txt", 3) == 0)
		return "text/html";		
		
	if (strncmp(sExt, "shtml", 4) == 0)
		return "text/html";	
		
	if (strncmp(sExt, "xhtml", 4) == 0)
		return "text/html";	
		
	if (strncmp(sExt, "htm", 3) == 0)
		return "text/html";	
		
	if (strncmp(sExt, "html", 4) == 0)
		return "text/html";	
		
	if (strncmp(sExt, "cgi", 3) == 0)
		return "text/html";	
		
	if (sExt[0] == 0 || sExt[0] == ' ')
		return "text/html";	

	return "application/x-msdownload";
}



size_t getfilecontent(const char *filename, char **buf) //得到文件内容
{
	int ishtml = 0;
	if (strcmp(getfiletype(filename), "text/html") == 0)//filename是一个html文件
		ishtml = 1;
	
	
	
	struct stat t;
	memset(&t, 0, sizeof(t));
	FILE *fd = fopen(filename, "rb"); //从只读方式打开参数filename指定的文件

	if (fd != NULL)
	{
		stat(filename, &t);
		int rc = t.st_size;
		if (ishtml)
		{
			rc += 4;
			*buf = malloc(rc); //根据文件大小，动态分配内存buf
			fread(*buf, t.st_size, 1, fd); //将文件读取到buf
			(*buf)[rc - 4] = '\r';//如果是html文件，将缓冲区最后4个字节设置为\r\n\r\n
			(*buf)[rc - 3] = '\n';
			(*buf)[rc - 2] = '\r';
			(*buf)[rc - 1] = '\n';
		}
		else
		{
			*buf = malloc(t.st_size); //根据文件大小，动态分配内存buf
			fread(*buf, t.st_size, 1, fd); //将文件读取到buf
		}
		fclose(fd);
		return rc;
	} else
	{
		printf("open %s failed %s\n", filename, strerror(errno));
		return 0;
	}
}

int socket_create(int port) //根据参数port，建立server端socket
{
	int st = socket(AF_INET, SOCK_STREAM, 0); //建立TCP的socket描述符
	if (st == -1)
	{
		printf("socket failed %s\n", strerror(errno));
		return 0;
	}
	int on = 1;
	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		return 0;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("bind failed %s\n", strerror(errno));
		return 0;
	}
	if (listen(st, 200) == -1)
	{
		printf("listen failed %s\n", strerror(errno));
		return 0;
	}
	printf("listen %d success\n", port);
	return st; //返回listen的socket描述符
}

void sockaddr_toa(const struct sockaddr_in *addr, char *IPAddr) //将struct sockaddr_in转化为IP地址字符串
{
	unsigned char *p = (unsigned char *) &(addr->sin_addr.s_addr);
	sprintf(IPAddr, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
}

void socket_accept(int st)
{
	pthread_t thr_d;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //设置线程为可分离状态
	int client_st = 0;
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	while (1) //循环执行accept
	{
		memset(&client_addr, 0, sizeof(client_addr));
		//accept函数阻塞，直到有client端连接到达，或者accept错误返回
		client_st = accept(st, (struct sockaddr *) &client_addr, &len);

		if (client_st == -1 && errno != 4)
		{
			printf("accept failed %d: %s\n", errno, strerror(errno));
			break; //accept错误，循环break
		} else
		{
			char sIP[32];
			memset(sIP, 0, sizeof(sIP));
			sockaddr_toa(&client_addr, sIP);
			char text[128] = { 0 };
			sprintf(text, "accept by %s", sIP);
			printf("%s\n", text);
			writelog(text);
			int *tmp = malloc(sizeof(int));
			*tmp = client_st;
			//将来自client端的socket做为参数，启动一个可分离线程
			pthread_create(&thr_d, &attr, socket_contr, tmp);
		}
	}
	pthread_attr_destroy(&attr);//释放线程参数
}

void writelog(const char *log) //向my.log文件中写日志
{
	time_t tDate;
	struct tm *eventTime;
	time(&tDate);//得到系统时间
	eventTime = localtime(&tDate);//将时间转化为struct tm结构
	int iYear = eventTime->tm_year + 1900;//得到年
	int iMon = eventTime->tm_mon + 1;//得到月
	int iDay = eventTime->tm_mday;//得到天
	int iHour = eventTime->tm_hour;//得到小时
	int iMin = eventTime->tm_min;//得到分钟
	int iSec = eventTime->tm_sec;//秒
	char sDate[16] = { 0 };
	sprintf(sDate, "%04d-%02d-%02d", iYear, iMon, iDay);//将字符串格式化为yyyy-mm-dd格式
	char sTime[16] = { 0 };
	sprintf(sTime, "%02d:%02d:%02d", iHour, iMin, iSec);//将字符串格式化为hh:mi:se格式
	char s[1024] = { 0 };
	char tmp[1000] = { 0 };
	strncpy(tmp, log, 999);
	sprintf(s, "%s %s %s\n", sDate, sTime, tmp);//生成一个字符串，格式为yyyy-mm-dd hh:mi:se log内容
	const char *slogfile = "my.log";
	FILE *fd = fopen(slogfile, "a+");//以追加方式打开my.log文件
	if (fd == NULL)
	{
		printf("open %s failed %s\n", slogfile, strerror(errno));
	} else
	{
		fputs(s, fd);//往my.log中插入一行
		fclose(fd);
	}
}