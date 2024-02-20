#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QtNetwork>
#include <QMessageBox>

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

    QList<QString> read;        // 수신 메시지
    QString readData;

    QList<QString> orderList;   // 주문번호 저장

    void connectServer();        // 서버랑 연결

signals:
    void readyRead();

private slots:
    void readMessage();     // 메시지 읽어옴
    void login();           // 로그인 함수

    void Accept();          // 수락
    void Refuse();          // 거절
};
#endif // WIDGET_H
