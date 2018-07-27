
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "work.h"
#include "pub.h"

#define HEAD "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length:%u\r\n\r\n"

/*

#define HEAD "HTTP/1.0 200 OK\r\n\
Content-Type: %s\r\n\
Transfer-Encoding: chunked\r\n\
Connection: Keep-Alive\r\n\
Accept-Ranges:bytes\r\n\
Content-Length:%u\r\n\r\n"
*/
#define TAIL "\r\n\r\n"

//#define EXEC "s?wd="
#define POST_LEN "Content-Length:"

#define POST_HEAN_END "\r\n\r\n"

void gethttpcommand(const char *sHTTPMsg, char *command) //从http请求中读出GET后面的命令行
{
	int i;
	int istart = 0;
	int iend = 0;
	int msg_len = strlen(sHTTPMsg);
	for (i = 0; i < msg_len; i++)
	{
		if ((sHTTPMsg[i] == ' ') && (istart == 0))
		{
			istart = i + 2;
		}
		else
		{
			if (sHTTPMsg[i] == ' ')
			{
				iend = i;
				break;
			}
		}
	}
	int len = iend - istart;
	if (istart > msg_len || len <= 0 || len >= 2048)
		return;
	strncpy(command, &sHTTPMsg[istart], len);
}


size_t getcgicontent(const char *cgi, char **buf) //得到cgi文件输出内容
{
	
	char cmd[2048] = { 0 };
	httpstr2stdstr(cgi, cmd);//将get请求中的转义字符转化为实际字符
	
	
	//strcpy(cmd, cgi);
	int i = 0;
	while (cmd[i++])
	{
		if (cmd[i] == '?' || cmd[i] == '&')
		{
			cmd[i] = ' ';
		}
	}
	
	char path[2048] = { 0 };
	sprintf(path, "./%s", cmd);
	FILE *fd = popen(path, "r"); //
	if (fd != NULL)
	{
		*buf = malloc(BUFSIZE); //固定分配8k空间
		memset(*buf, 0, BUFSIZE);
		char tmp[2048] = { 0 };
		fgets(tmp, sizeof(tmp), fd);
		if (strncmp(tmp, "content-type", strlen("content-type")) != 0)
		{
			strcat(*buf, tmp);
		}
		while (!feof(fd))
		{
			memset(tmp, 0, sizeof(tmp));
			fgets(tmp, sizeof(tmp), fd);
			strcat(*buf, tmp);
		}
		pclose(fd);
		return strlen(*buf);//返回实际字符长度
	}
	else
	{
		printf("open %s failed %s\n", cmd, strerror(errno));
		return 0;
	}
}


size_t make_http_content(const char *command, char **buf) //根据get提供的文件名，生成静态http reponse消息内容
{
	char *contentbuf = NULL;
	unsigned int icontentlen = 0;
	int is_error = 0;//0为没有错误，1为发生错误

//GET请求后面为空，或者为myhttp makefile 得到默认页面内容图
	if (command[0] == 0 || strncmp(command, "myhttp", 6) == 0 || strncmp(command, "makefile", 8) == 0) 
	{
		icontentlen = getfilecontent("default.html", &contentbuf);
	}
	else
	{
		if (strstr(command, ".cgi"))//get请求中有.cgi
		{
			icontentlen = getcgicontent(command, &contentbuf);
		}
		else//GET请求后面为client要得到的文件名
		{
			char path[2048] = { 0 };
			char tmp[2046] = { 0 };
			strncpy(tmp, command, sizeof(tmp) - 1);
			sprintf(path, "./%s", tmp);//
			icontentlen = getfilecontent(path, &contentbuf);//动态设置http请求内容,query为条件，buf为动态内容
		}

		if (icontentlen == 0)//发生错误，显示错误页
		{
			is_error = 1;
			icontentlen = getfilecontent("error.html", &contentbuf);
		}
	}

	if (icontentlen > 0)
	{
		char headbuf[1024];
		memset(headbuf, 0, sizeof(headbuf));
		if (is_error)
			sprintf(headbuf, HEAD, getfiletype("error.html"), icontentlen);
		else
			sprintf(headbuf, HEAD, getfiletype(command), icontentlen); //设置消息头
		size_t iheadlen = strlen(headbuf);//得到消息头长度
		size_t isumlen = iheadlen + icontentlen;//得到消息总长度
		*buf = malloc(isumlen);//根据消息总长度，动态分配内存
		char *tmp = *buf;
		memcpy(tmp, headbuf, iheadlen); //安装消息头
		memcpy(&tmp[iheadlen], contentbuf, icontentlen); //安装消息体
		printf("headbuf:\n%s", headbuf);
		if (contentbuf)
			free(contentbuf);
		return isumlen;//返回消息总长度
	}
	else
	{
		return 0;
	}
}

void send_msg(int sock, const char *content, size_t contentsize)
{
	int rc = 0;
	while (1)
	{
		rc = send(sock, &content[rc], contentsize, 0);
		if (rc >= contentsize)
			break;
		if (rc <= 0)
			break;
		contentsize -= rc;
	}
}

int getpostcontent(const char *sHTTPMsg, char **content) //得到post的长度和内容
{
	const char *s = strstr(sHTTPMsg, POST_LEN);
	if (s)
	{
		int end = 0;
		while (s[end] != '\n' && s[end] != '\0')
		{
			end++;
		}
		char len[100] = { 0 };
		strncpy(len, &s[strlen(POST_LEN)], end);
		int i_len = atoi(len);
		s = strstr(s, "\r\n\r\n");
		if (s)
		{
			*content = malloc(i_len);
			memcpy(*content, &s[4], i_len);
			return i_len;
		}
	}
	return 0;
}


int get_post_head_len(const char *sHTTPMsg) //得到post消息head的长度
{
	int index = 0;
	while (sHTTPMsg[index + 3] != '\0')
	{
		if (sHTTPMsg[index] == '\r' && sHTTPMsg[index + 1] == '\n' &&
			sHTTPMsg[index + 2] == '\r' && sHTTPMsg[index + 3] == '\n')
			break;
		index++;
	}
	return index + 4;
}

int get_post_body_len(const char *sHTTPMsg) //得到post消息body的长度
{
	const char *s = strstr(sHTTPMsg, POST_LEN);
	if (s)
	{

		int end = 0;
		while (s[end] != '\n' && s[end] != '\0')
		{
			end++;
		}
		char len[100] = { 0 };
		strncpy(len, &s[strlen(POST_LEN)], end);

		int i_len = atoi(len);
		return i_len;
	}
	return 0;
}

int save_post_file(const char *post_body, int bufsize)//解析post消息体，保存上传的文件
{
	int first_line_end = 0;//指向buf第一行的指针的结束位置
	int second_line_end = 0;//指向buf第二行的指针的结束位置
	int third_line_end = 0;//指向buf第三行的指针的结束位置
	int content_line_end = 0;//指向buf文件内容指针的结束位置

	int i;
	for (i = 0; i < bufsize; i++)//找到第一个\r\n
	{
		if (post_body[i] == '\r' && post_body[i + 1] == '\n')
		{
			first_line_end = i;
			break;
		}
	}

	for (i = first_line_end + 2; i < bufsize; i++)//找到第二个\r\n
	{
		if (post_body[i] == '\r' && post_body[i + 1] == '\n')
		{
			second_line_end = i;
			break;
		}
	}

	for (i = second_line_end + 2; i < bufsize; i++)//找到第三个\r\n
	{
		if (post_body[i] == '\r' && post_body[i + 1] == '\n')
		{
			third_line_end = i;
			break;
		}
	}
	char line[1024] = { 0 };

	memset(line, 0, sizeof(line));
	memcpy(line, &post_body[second_line_end + 2], third_line_end - second_line_end - 2);//截取出第三行
	printf("----------------------log----------------------\n%s\n", line);
	printf("----------------------log----------------------\n");
	memset(line, 0, sizeof(line));
	memcpy(line, &post_body[first_line_end + 2], second_line_end - first_line_end - 2);//截取出第二行

	line[strlen(line) - 1] = '\0';//去掉最后的"号
	char filename[1024] = { 0 };
	for (i = strlen(line) - 1; i >= 0; i--)
	{
		if (line[i] == '\"')
		{
			strcpy(filename, &line[i + 1]);
			break;
		}
	}

	content_line_end = bufsize - first_line_end;

	const char *tmp_filename = getfilename(filename);
	FILE *p = fopen(tmp_filename, "r");
	if (p)//上传文件已经存在
	{
		fclose(p);
		return 0;
	}

	p = fopen(tmp_filename, "wb");
	if (p == NULL)
		return 0;

	fwrite(&post_body[third_line_end + 4], content_line_end - third_line_end - 10, 1, p);
	fclose(p);
	char ext[1024] = { 0 };
	get_file_ext(filename, ext);
	if (strcmp(ext, "cgi") == 0)//如果是cgi文件，那么设置可执行权限
		chmod(tmp_filename, 0775);

	return 1;
}

void *socket_contr(void *arg)//线程入口函数
{
	printf("thread is begin\n");
	int st = *(int *)arg;//得到来自client端的socket
	free((int *)arg);

	char *buf = malloc(BUFSIZE);
	memset(buf, 0, BUFSIZE);

	ssize_t rc = recv(st, buf, BUFSIZE, 0);//接收来自client端socket的消息

	if (rc <= 0)
	{
		printf("recv failed %s\n", strerror(errno));
	}
	else
	{
		printf("recv:\n%s", buf);
		writelog(buf);
		if (strncmp(buf, "GET", 3) == 0)//处理GET请求
		{
			char command[2048] = { 0 };
			char tmp[2048] = { 0 };
			gethttpcommand(buf, tmp); //得到http 请求中 GET后面的字符串			
			httpstr2stdstr(tmp, command);//将get请求中的转义字符转换为原始字符

			char *content = NULL;
			size_t ilen = make_http_content(command, &content);//根据用户在GET中的请求，生成相应的回复内容
			if (ilen > 0)
			{
				//send(st, content, ilen, 0);//将回复的内容发送给client端socket
				send_msg(st, content, ilen);
				free(content);
			}
		}

		if (strncmp(buf, "POST", 4) == 0)//处理POST请求
		{
			extern int allow_postfile;

			if (strstr(buf, "multipart") != NULL)//上传文件
			{
				if (allow_postfile)//可以上传文件
				{
					int body_len = get_post_body_len(buf);
					int head_len = get_post_head_len(buf);
					int len = head_len + body_len;

					buf = realloc(buf, len);
					while (rc < len)
					{
						int aa = recv(st, &buf[rc], len - rc, 0);
						rc += aa;
					}

					if (save_post_file(&buf[head_len], body_len))//解析post消息体，保存上传的文件
					{
						char *content = NULL;
						size_t ilen = make_http_content("success.html", &content);//执行成功，返回成功页
						if (ilen > 0)
						{
							send_msg(st, content, ilen);
							free(content);
						}
					}
					else
					{
						char *content = NULL;
						size_t ilen = make_http_content("error.html", &content);//执行失败，返回出错页
						if (ilen > 0)
						{
							send_msg(st, content, ilen);
							free(content);
						}
					}
				}
			}
			else
			{
				char command[2048] = { 0 };
				gethttpcommand(buf, command); //得到http 请求中 POST后面的字符串

				char *post = NULL;
				if (getpostcontent(buf, &post) > 0)
				{

					strcat(command, "?");
					strcat(command, post);
					free(post);
				}
				char *content = NULL;
				size_t ilen = make_http_content(command, &content);//根据用户在POST中的请求，生成相应的回复内容
				if (ilen > 0)
				{
					//send(st, content, ilen, 0);//将回复的内容发送给client端socket
					send_msg(st, content, ilen);
					free(content);
				}
			}
		}
	}
	free(buf);

	close(st);//关闭client端socket
	printf("thread_is end\n");
	return NULL;
}

