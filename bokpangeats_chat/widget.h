#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QHostAddress getMyIP();
    QTcpSocket *tcpSocket;

    QList<QString> read;
    QString readData;

    QList<QString> userList;

    void connectServ();        // 서버랑 연결

private slots:
    void readMessage();
    void sendMessage();

};
#endif // WIDGET_H
