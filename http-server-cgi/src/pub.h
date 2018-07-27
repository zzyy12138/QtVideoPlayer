

#ifndef PUB_H_
#define PUB_H_

//8192是8k
#define BUFSIZE 8192

void setdaemon();
void catch_Signal(int Sign);
int signal1(int signo, void (*func)(int));
const char *getfilename(const char *path);//从完整路径中得到文件名
void get_file_ext(const char *filename, char *extname);//得到文件扩展名
const char *getfiletype(const char *filename); //根据扩展名返回文件类型描述
size_t getfilecontent(const char *filename, char **buf); //得到文件内容
int socket_create(int port);
void socket_accept(int st);

//将HTTP GET请求中的转义符号转化为标准字符,注意，空格被转义为'+'号
void httpstr2stdstr(const char *httpstr, char *stdstr);

void writelog(const char *log); //向my.log文件中写日志

#endif /* PUB_H_ */
