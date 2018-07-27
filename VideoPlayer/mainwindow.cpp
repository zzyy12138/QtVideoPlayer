#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>
#include "myhttp.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置主界面背景
    QPixmap pixmap(":/images/background.jpg");
    QPalette palette;
    palette.setBrush(backgroundRole(),QBrush(pixmap));
    setPalette(palette);

    //创建mediaPlayer
    mediaPlayer = new QMediaPlayer(this);

    //设置mediaPlayer 的 QVideoWidget 播放窗口
    mediaPlayer->setVideoOutput(ui->widget);
    //设置默认音量
    ui->voice->setValue(50);
    mediaPlayer->setVolume(ui->voice->value());

    //默认播放速度
    speed=1;
    mediaPlayer->setPlaybackRate(speed);

    //设置播放属性
    //1、监听播放信号变化函数
    connect(mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(mediaStateChanged(QMediaPlayer::State)));
    //2、播放进度信号变化函数
    connect(mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    //3、播放长度信号变化
    connect(mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));
    //4、设置播放错误关联
    connect(mediaPlayer,SIGNAL(error(QMediaPlayer::Error)),this,SLOT(handleError()));

    //播放测试
    //mediaPlayer->setMedia(QUrl::fromLocalFile("E:/01-1.avi"));

    //设置播放按钮
    ui->toolButton->setToolTip("播放");
    ui->toolButton->setAutoRaise(true);
    ui->toolButton->setIcon(QPixmap(":/images/play.png"));

    //设置打开文件按钮
    ui->toolButton_2->setToolTip("打开文件");
    ui->toolButton_2->setAutoRaise(true);
    ui->toolButton_2->setIcon(QPixmap(":images/openfile.png"));

    //设置静音按钮
    ui->toolButton_3->setToolTip("静音");
    ui->toolButton_3->setAutoRaise(true);
    ui->toolButton_3->setIcon(QPixmap(":images/voice.png"));

    //设置设置按钮
    ui->toolButton_4->setToolTip("设置");
    ui->toolButton_4->setAutoRaise(true);
    ui->toolButton_4->setIcon(QPixmap(":images/set.png"));

    ui->lab_nowtime->setText("00:00/00:00");




    //获取文件列表
    getFile();

    //设置listwidget 处理事件  相应item doubleClicked事件
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,[=](QListWidgetItem * item)
    {
        //获取播放的url地址，根据地址打开播放的文件
        QUrl url;
        url.setUrl("http://10.17.176.148:80/mv/"+item->text());
        //设置mediaplayer 的播放内容
        mediaPlayer->setMedia(url);
        play();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::play()
{
    qint64 minute;
    int second;
    qint64 time;
    time = mediaPlayer->duration()/1000;        //得到总共的秒数
    minute = time/60;                     //获得分
    second = time%60;                        //获得秒
    totaltime = QString::number(minute) + ":" + QString::number(second);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    timer->start(100);
   //获取播放状态
    switch (mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer->pause();
        break;
    default:
        mediaPlayer->play();
        break;
    }
}

void MainWindow::on_toolButton_clicked()
{
    play();
    //需要设置按钮暂停样式  测试播放内容
    //ui->toolButton->setToolTip("暂停");
    //ui->toolButton->setIcon(QPixmap(":/images/pause.png"));
}


void MainWindow::mediaStateChanged(QMediaPlayer::State state)
{
    //mediaplay播放过程中动态调整播放进度
    switch (state) {
    case QMediaPlayer::PlayingState:
        ui->listWidget->hide();
        ui->toolButton->setToolTip("暂停");
        ui->toolButton->setIcon(QPixmap(":/images/pause.png"));

        break;
    default:
        ui->listWidget->show();
        ui->toolButton->setToolTip("播放");
        ui->toolButton->setIcon(QPixmap(":/images/play.png"));
        break;
    }
}
void MainWindow::setPosition(int position)
{
    //获取mediaplayer进度条调整位置
    mediaPlayer->setPosition(position);
}
void MainWindow::durationChanged(qint64 duration)
{
    //设置进度条的范围
    ui->horizontalSlider->setRange(0,duration);

    //ui->lab_allTime->setText(tr("%1").arg(duration));
}

void MainWindow::positionChanged(qint64 position)
{
    //改变播放位置
    ui->horizontalSlider->setValue(position);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    setPosition(position);
}

void MainWindow::on_toolButton_2_clicked()
{
    QString filename = QFileDialog::getOpenFileName();
    //设置播放内容
    mediaPlayer->setMedia(QUrl::fromLocalFile(filename));

    ui->toolButton->setAutoRaise(true);
}

//通过http请求获取服务端server中  mv目录下的文件列表
void MainWindow::getFile()
{
    myhttp http;
    QString content = http.getFileList("10.17.176.148",80);
    qDebug()<< content;

    //获取到的服务器资源列表
    //将资源列表中的内容 变成listwidget中的条目  item

    QStringList ls = content.split("\n");
    for(int i=0;i<ls.count();i++)
    {
        //qDebug()<<ls[i];
        //如果ls[i]不为空 将ls[i]添加到item中
        if(!ls[i].isEmpty())
        {
            createItem(ls[i]);
        }
    }

}

void MainWindow::createItem(QString item)
{
    //创建item条目
    QListWidgetItem * item1 = new QListWidgetItem(item);
    //item1->setBackgroundColor();
    //设置item水平居中对齐
    //item1->setTextAlignment(Qt::AlignHCenter);
    //添加到界面中
    ui->listWidget->addItem(item1);
}

void MainWindow::on_toolButton_3_clicked()
{
    if(flag)
    {
        mediaPlayer->setVolume(false);
        ui->toolButton_3->setIcon(QPixmap(":images/voice2.png"));
        flag=false;
    }
    else
    {
        mediaPlayer->setVolume(ui->voice->value());
        ui->toolButton_3->setIcon(QPixmap(":images/voice.png"));
        flag=true;
    }
}

void MainWindow::on_voice_valueChanged(int value)
{
    mediaPlayer->setVolume(ui->voice->value());
}

void MainWindow::on_toolButton_4_clicked()
{
    emit setdlgshow();
}

void MainWindow::timerUpDate()
{
    stime=mediaPlayer->position();
    qint64 time=stime/1000;
    qint64 minute = time / 60;
    int second = time % 60;
    currenttime = QString::number(minute) + ":" + QString::number(second);
    QString Time = currenttime + '/' + totaltime;
    ui->lab_nowtime->setText(Time);

}
void MainWindow::SpeedDown()
{
    speed=speed/2;
    mediaPlayer->setPlaybackRate(speed);
}
void MainWindow::SpeedUp()
{
    speed=speed*2;
    mediaPlayer->setPlaybackRate(speed);
}
void MainWindow::SpeedNormal()
{
    mediaPlayer->setPlaybackRate(1);

}
