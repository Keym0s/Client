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
    newchat = new NewChat();
    newchat->setModal(true); // Устанавливаем диалог модальным
    connect(this, &MainWindow::sendMessage, auth, &Authorization::receiveMessage);
    connect(auth, &Authorization::sendMessage, this, &MainWindow::receiveMessage);
    connect(this, &MainWindow::auError, auth, &Authorization::AuthError);
    connect(this, &MainWindow::membersForNewChat, newchat, &NewChat::receiveUsers);
    connect(newchat, &NewChat::createNewChat, this, &MainWindow::createNewChat);
    auth->show();
    ui->createChatButton->setVisible(false);
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
    out << quint16(0) << MessageType::Message << str;
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

//Запрос на получение чатов
void MainWindow::GetChats()
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << MessageType::GetChats;
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

//Создание нового чата
void MainWindow::createNewChat(QStringList usernames, const QString chatname)
{
    Data.clear();
    usernames.append(user);
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << MessageType::JoinGroupChat << usernames << chatname;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
    GetChats();
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
                ui->membersList->clear();
                QStringList messages, members;
                in >> messages >> members;
                foreach (const QString &message, messages) {
                    ui->textBrowser->append(message);
                }
                ui->membersList->addItems(members);
                break;
            }
            case MessageType::GetContacts:
            {
                QStringList usernames;
                in >> usernames;
                if (ui->ContactsButton->isChecked() == true){
                    ui->contactsList->clear();
                    foreach (const QString &username, usernames) {
                        ui->contactsList->addItem(new QListWidgetItem(username));
                    }
                }
                else
                {
                    emit membersForNewChat(usernames);
                }
                break;
            }
            case MessageType::GetChats:
            {
                QStringList chats;
                in >> chats;
                ui->contactsList->clear();
                foreach (const QString &chat, chats) {
                    ui->contactsList->addItem(new QListWidgetItem(chat));
                }
                break;
            }
            case MessageType::Message:
            {
                QString message;
                in >> message;
                ui->textBrowser->append(message);
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

//При нажатии на item
void MainWindow::on_contactsList_itemClicked(QListWidgetItem *item)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    if (ui->ContactsButton->isChecked() == true)
    {
        QStringList usernames;
        usernames.append(user);
        usernames.append(item->text());
        out << quint16(0) << MessageType::JoinChat << usernames;
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        socket->write(Data);
    }
    else
    {
        QString chat;
        chat = item->text();
        out << quint16(0) << MessageType::ShowGroupChat << chat;
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        socket->write(Data);
    }
}

//При нажатии на "Чаты"
void MainWindow::on_ChatsButton_clicked()
{
    GetChats();
    ui->textBrowser->clear();
    ui->membersList->clear();
    ui->contactsList->clear();
    ui->createChatButton->setVisible(true);
}

//При нажатии на "Контакты"
void MainWindow::on_ContactsButton_clicked()
{
    GetContacts();
    ui->textBrowser->clear();
    ui->membersList->clear();
    ui->createChatButton->setVisible(false);
}

//Кнопка создать чат
void MainWindow::on_createChatButton_clicked()
{
    newchat->show();
    GetContacts();
}

