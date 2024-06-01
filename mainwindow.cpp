//MAINWINDOW.CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"

//Конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket,&QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    socket->connectToHost("127.0.0.1", 2323);
    auth = new Authorization();
    connect(this, &MainWindow::sendMessage, auth, &Authorization::receiveMessage);
    connect(auth, &Authorization::sendMessage, this, &MainWindow::receiveMessage);
    connect(this, &MainWindow::auError, auth, &Authorization::AuthError);
    auth->show();
    nextBlockSize = 0;
}

//Деструктор
MainWindow::~MainWindow()
{
    delete ui;
}

//Отправка сообщения на сервер
void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << MessageType::Message << QTime::currentTime() << str << user;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
    ui->lineEdit->clear();
}

//Запрос на получение контактов
void MainWindow::GetContacts()
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << MessageType::GetContacts;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

//Получение запроса от окна авторизации, и перессылка на сервер
void MainWindow::receiveMessage(const QString &login, const QString &password, MessageType &mtype)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << mtype << login << password;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

//Чтение запроса от сервера
void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }

            MessageType type;
            in >> type;

            switch (type)
            {

            case MessageType::AuthDone:
            {

                in >> user;
                ui->usernameLabel->setText(user);
                auth->close();
                GetContacts();
                show();
                break;
            }
            case MessageType::AuthError:
            {
                QString str;
                in >> str;
                emit auError(str);
                break;
            }
            case MessageType::JoinChat:
            {
                ui->textBrowser->clear();
                QStringList messages;
                in >> messages;
                foreach (const QString &message, messages) {
                    ui->textBrowser->append(message);
                }
                break;
            }
            case MessageType::GetContacts:
            {
                QStringList usernames;
                in >> usernames;
                ui->contactsList->clear();
                foreach (const QString &username, usernames) {
                    ui->contactsList->addItem(new QListWidgetItem(username));
                }
                break;
            }

            }
            nextBlockSize = 0;
        }
    }
}

//Отправление сообщения
void MainWindow::on_pushButton_2_clicked()
{
    SendToServer(ui->lineEdit->text());
}
void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer(ui->lineEdit->text());
}

//При нажатии на контакт
void MainWindow::on_contactsList_itemClicked(QListWidgetItem *item)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QStringList usernames;
    usernames.append(user);
    usernames.append(item->text());
    out << quint16(0) << MessageType::JoinChat << usernames;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

