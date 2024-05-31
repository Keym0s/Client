//AUTHORIZATION.CPP
#include "authorization.h"
#include "ui_authorization.h"

//Конструктор
Authorization::Authorization(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Authorization)
{
    ui->setupUi(this);
    ui->passwordAccept->setVisible(false);
    ui->passwordAccept_label->setVisible(false);
    ui->status_label->setStyleSheet("QLabel { color : red; }");
}

//Деструктор
Authorization::~Authorization()
{
    delete ui;
}

//Передать запрос другой форме, чтобы она отправила на сервер
void Authorization::sendData(const QString &login, const QString &password)
{
    MessageType m;
    //Если регистрация
    if (ui->registrationButton->isChecked() == true)
    {
        m = MessageType::Register;
    }
    //Если обычный вход
    else
    {
        m = MessageType::AuthData;
    }
    emit sendMessage(login, password, m);
}

void Authorization::on_LogInButton_clicked()
{

    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();
    sendData(login, password);
}

void Authorization::on_registrationButton_toggled(bool checked)
{
    if (checked == true)
    {
        ui->LogInButton->setText("Регистрация");
        ui->passwordAccept->setVisible(true);
        ui->passwordAccept_label->setVisible(true);
    }
    else
    {
        ui->LogInButton->setText("Войти");
        ui->passwordAccept->setVisible(false);
        ui->passwordAccept_label->setVisible(false);
    }
}

void Authorization::receiveMessage(const QString &message)
{

}

void Authorization::AuthError(QString str)
{
    ui->status_label->setText(str);
}


