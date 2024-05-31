//AUTHORIZATION.H
#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include "MessageType.h"
#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class Authorization;
}

class Authorization : public QDialog
{
    Q_OBJECT

public:
    explicit Authorization(QWidget *parent = nullptr);
    ~Authorization();


private:
    Ui::Authorization *ui;
    void sendData(const QString &login, const QString &password);

private slots:
    void on_LogInButton_clicked();

    void on_registrationButton_toggled(bool checked);

public slots:
    void receiveMessage(const QString &message);

    void AuthError(QString str);

signals:
    void sendMessage(const QString &login, const QString &password, MessageType &mtype);
};

#endif // AUTHORIZATION_H
