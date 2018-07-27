

#ifndef WORK_H_
#define WORK_H_

size_t getcgicontent(const char *cgi, char **buf); //得到cgi文件输出内容
void gethttpcommand(const char *sHTTPMsg, char *command); //从http请求中读出GET后面的命令行
size_t make_http_content(const char *command, char **buf); //根据get提供的文件名，生成静态http reponse消息内容
void *socket_contr(void *arg);

#endif /* WORK_H_ */
