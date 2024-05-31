//MAINWINDOW.H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include "MessageType.h"
#include "authorization.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QByteArray Data;
    QString user;
    Authorization* auth;
    void SendToServer(QString str);
    quint16 nextBlockSize;

public slots:
    void slotReadyRead();
    void receiveMessage(const QString &login, const QString &password, MessageType &mtype);

signals:
    void sendMessage(const QString &message);
    void regDone();
    void regError();
    void auDone();
    void auError(QString str);
};
#endif // MAINWINDOW_H
