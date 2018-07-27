#include "setdialog.h"
#include "ui_setdialog.h"

SetDialog::SetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

SetDialog::~SetDialog()
{
    delete ui;
}


void SetDialog::SetDlgShow()
{
    this->show();
}

void SetDialog::on_btn_down_clicked()
{
    emit speedDown();
}

void SetDialog::on_btn_up_clicked()
{
    emit speedUp();
}

void SetDialog::on_btn_normal_clicked()
{
    emit speedNormal();
}
