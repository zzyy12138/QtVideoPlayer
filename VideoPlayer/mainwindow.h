#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QString>
#include <QListWidget>
#include <QTimer>
#include <QTime>
#include "setdialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;//QVideoWidget
    bool flag;

    QTimer *timer;
    qint64 stime;
    QString totaltime, currenttime;
    double_t speed;


private slots:
    //播放
    void play();
    //播放状态
    void mediaStateChanged(QMediaPlayer::State state);
    //改变播放位置
    void positionChanged(qint64 position);
    //获取播放位置
    void setPosition(int position);
    //播放长度
    void durationChanged(qint64 position);
    //获取服务器的文件列表
    void getFile();
    //创建listwidget条目item  添加条目名称
    void createItem(QString item);


    void on_toolButton_clicked();
    void on_horizontalSlider_sliderMoved(int position);
    void on_toolButton_2_clicked();
    void on_toolButton_3_clicked();
    void on_voice_valueChanged(int value);
    void on_toolButton_4_clicked();

    void timerUpDate();

    void SpeedDown();
    void SpeedUp();
    void SpeedNormal();

signals:
    void setdlgshow();
};

#endif // MAINWINDOW_H
