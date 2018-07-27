#ifndef SETDIALOG_H
#define SETDIALOG_H

#include <QDialog>

namespace Ui {
class SetDialog;
}

class SetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetDialog(QWidget *parent = 0);
    ~SetDialog();

private:
    Ui::SetDialog *ui;

private slots:
    void SetDlgShow();

    void on_btn_down_clicked();

    void on_btn_up_clicked();

    void on_btn_normal_clicked();

signals:
    void speedDown();
    void speedUp();
    void speedNormal();

};

#endif // SETDIALOG_H
