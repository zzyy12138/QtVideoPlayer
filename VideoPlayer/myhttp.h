#ifndef MYHTTP_H
#define MYHTTP_H


#include <QTcpSocket>

class myhttp:public QTcpSocket
{
public:
    myhttp();
    //获取网络中文件列表  url  地址  port  端口
    QString getFileList(QString URL,int port);
    ~myhttp();
};

#endif // MYHTTP_H
