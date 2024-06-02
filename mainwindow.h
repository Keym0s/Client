//MAINWINDOW.H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <QStringListModel>
#include <QListWidgetItem>
#include "MessageType.h"
#include "authorization.h"
#include "newchat.h"

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

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_contactsList_itemClicked(QListWidgetItem *item);

    void on_ChatsButton_clicked();

    void on_ContactsButton_clicked();

    void on_createChatButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QByteArray Data;
    QString user;
    Authorization* auth;
    NewChat* newchat;
    int currentChatID;
    void SendToServer(QString str);
    void GetContacts();
    void GetChats();
    quint16 nextBlockSize;

public slots:
    void slotReadyRead();
    void receiveMessage(const QString &login, const QString &password, MessageType &mtype);
    void createNewChat(QStringList usernames, const QString chatname);

signals:
    void sendMessage(const QString &message);
    void auDone();
    void auError(QString str);
    void membersForNewChat(QStringList users);
};
#endif // MAINWINDOW_H
