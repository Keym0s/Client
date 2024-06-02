#ifndef NEWCHAT_H
#define NEWCHAT_H

#include <QDialog>

namespace Ui {
class NewChat;
}

class NewChat : public QDialog
{
    Q_OBJECT

public:
    explicit NewChat(QWidget *parent = nullptr);
    ~NewChat();

private:
    Ui::NewChat *ui;

public slots:
    void receiveUsers(QStringList users);
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
    void createNewChat(QStringList usernames, const QString chatname);
};

#endif // NEWCHAT_H
