#include "myhttp.h"

#include <QHostAddress>
#include <QString>
#include <string.h>

//http的表头文件
#define REQ "GET /filelist.cgi HTTP/1.1\r\n\
Accept: text/html, application/xhtml+xml, */*\r\n\
Accept-Language: zh-CN\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko\r\n\
Accept-Encoding: gzip, deflate\r\n\
Host: %s\r\n\
Connection: Keep-Alive\r\n"

myhttp::myhttp()
{

}

QString myhttp::getFileList(QString URL, int port)
{
    QHostAddress serverIP;
    serverIP.setAddress(URL);
    //连接到server端
    connectToHost(serverIP,port);
    //等待连接
    waitForConnected();

    //获取http格式化的请求
    char buf[1024];
    memset(buf,0,sizeof(buf));

    //将格式化的字符串放在buf中   格式 http表头文件+ url地址
    sprintf(buf,REQ,URL.toStdString().data());
    //阻塞等待TCP写状态OK
    waitForBytesWritten();
    //将HTTP请求发送到server端
    write(buf,strlen(buf));
    //阻塞等待数据传输到本地
    waitForReadyRead();

    //保存从server端接收到的数据
    QString content;
    while(bytesAvailable()>0)
    {
        memset(buf,0,sizeof(buf));
        read(buf,sizeof(buf));
        content+=buf;
    }
    //等待阻塞断开完成
    //waitForDisconnected();
    //完成之后关闭socket
    close();

    //处理content数据\r\n 分割content
    QStringList ls = content.split("\r\n");

    return ls[ls.count()-1];

}


myhttp::~myhttp()
{

}
