#include <QHostAddress>
#include <QtNetwork>

#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    connectServer();

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(ui->login, SIGNAL(clicked()), this, SLOT(login()));

    connect(ui->btn_Accept, SIGNAL(clicked()), this, SLOT(Accept()));
    connect(ui->btn_Refuse, SIGNAL(clicked()), this, SLOT(Refuse()));
}

Widget::~Widget()
{
    delete ui;
}

// 서버와 연결
void Widget::connectServer()
{
    // tcpSocket 클라이언트 소켓 생성
    tcpSocket = new QTcpSocket(this);

    QHostAddress serverAddress = getMyIP();
    tcpSocket->connectToHost(serverAddress, 25000);
}

void Widget::readMessage()
{
    if(tcpSocket->bytesAvailable() >= 0)
    {
        readData = tcpSocket->readAll();
    }
    read = readData.split("#");
    // qDebug() << read[0];

    // 로그인
    if(read[0] == "fail")
    {
        QMessageBox::critical(this, "알림", "매장 정보가 없습니다.");
        ui->storeID->clear();
        ui->storePW->clear();
    }
    else if(read[0] == "success")
    {
        ui->stackedWidget->setCurrentIndex(1);
    }

    // 주문 정보 들어옴
    else if(read[0] == "4")
    {
        qDebug() << read;
        QString showList;
        showList.append("\n주문번호: " + read[1] + "\n");
        showList.append("주소: " + read[2] + "\n");
        showList.append("전화번호: " + read[3] + "\n주문내역\n");
        for(int i = 4; i < read.size(); i+=6)
        {
            showList.append("메뉴: " + read[i]+ "    ");
            showList.append(read[i+1]+"개\n");
        }
        showList.append("총 결제금액: " + read[6] + "\n");

        // orderList.append(read[1]);
        ui->listWidget->addItem(showList);
    }
}

// 수락 버튼
void Widget::Accept()
{
    QString listText;
    listText = ui->listWidget->currentItem()->text();

    // 주문번호 확인하기
    int index = listText.indexOf("주소:");
    QString text = listText.left(index-1);
    text.remove(0, 7);

    text.prepend("5#수락#");
    text.append("#");

    qDebug() << text;

    // 삭제
    ui->listWidget->takeItem(ui->listWidget->currentRow());

    // 주문결과 보내기
    tcpSocket->write(text.toUtf8());
}

// 거절 버튼
void Widget::Refuse()
{
    QString listText;
    listText = ui->listWidget->currentItem()->text();

    // 주문번호 확인하기
    int index = listText.indexOf("주소:");
    QString text = listText.left(index-1);
    text.remove(0, 7);

    text.prepend("5#거절#");

    // 삭제
    ui->listWidget->takeItem(ui->listWidget->currentRow());

    // 주문결과 보내기
    tcpSocket->write(text.toUtf8());
}

// 연결
void Widget::login()
{
    QString storeData("0#" + ui->storeID->text()+ "#" +ui->storePW->text());
    tcpSocket->write(storeData.toUtf8());
}

QHostAddress Widget::getMyIP()
{
    QHostAddress myAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            myAddress = ipAddressesList.at(i);
            break;
        }
    }
    if (myAddress.toString().isEmpty())
        myAddress = QHostAddress(QHostAddress::LocalHost);

    return myAddress;
}
