#include "newchat.h"
#include "ui_newchat.h"

//Конструктор
NewChat::NewChat(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewChat)
{
    ui->setupUi(this);
}

//Деструктор
NewChat::~NewChat()
{
    delete ui;
}

//Получаем список пользователей
void NewChat::receiveUsers(QStringList users)
{
    ui->chatLine->clear();
    ui->listWidget->clear();
    //Добавление пользователей из QStringList в QListWidget с флажками
    foreach (const QString &user, users) {
        QListWidgetItem *item = new QListWidgetItem(user);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // Делаем элемент отмечаемым
        item->setCheckState(Qt::Unchecked); // По умолчанию флажок не установлен
        ui->listWidget->addItem(item); // Добавляем элемент в listWidget
    }
}

void NewChat::on_buttonBox_accepted()
{
    QStringList usernames;
    QString chatname;
    for(int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if(item->checkState() == Qt::Checked) {
            usernames.append(item->text());
        }
    }
    chatname = ui->chatLine->text();
    emit createNewChat(usernames, chatname);
    this->close();
}


void NewChat::on_buttonBox_rejected()
{
    this->close();
}

