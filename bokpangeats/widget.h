#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QTcpServer>
#include <QtSql>

#include <QRandomGenerator>

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

    QSqlDatabase bokpangDB;

    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    QString readMessage;            // 읽어올 메시지
    QString sendMessage;            // 보낼 메시지
    QList<QString> splitMessage;    // 메시지 쪼개서 저장

    int count = 1;                  // 문의하기 고객명

    // 소켓 구분
    // QSet<QTcpSocket *> set_cList;
    QTcpSocket *service;                        // 고객센터 클라이언트 저장

    QMap<QTcpSocket *, QString> storeList;      // 매장 클라이언트 저장
    QMap<QTcpSocket *, QString> userList;       // 유저 클라이언트 저장

    QSet<QTcpSocket *> dul;                     // 고객센터의 유저 클라이언트 중복 제거
    QMap<QTcpSocket *, QString> serviceList;    // 고객센터에 문의하는 유저 클라이언트 저장

    QHostAddress getMyIp();     // 아이피 주소
    void InitServer();          // 서버 초기화
    void storeClnt();           // 매장 클라

private slots:

    void slot_socketConnection();   // 소켓 연결
    void slot_read();               // 데이터 읽어오기

    void store_disconnected();      // 클라이언트의 연결종료

};
#endif // WIDGET_H
