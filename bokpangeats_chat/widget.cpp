#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connectServ();

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
}

Widget::~Widget()
{
    delete ui;
}

// 서버와 연결
void Widget::connectServ()
{
    // tcpSocket 클라이언트 소켓 생성
    tcpSocket = new QTcpSocket(this);
    QHostAddress serverAddress = getMyIP();
    tcpSocket->connectToHost(serverAddress, 25000);

    QString send = "9#";
    tcpSocket->write(send.toUtf8());
}

void Widget::readMessage()
{
    if(tcpSocket->bytesAvailable() >= 0)
    {
        readData = tcpSocket->readAll();
    }
    read = readData.split("#");

    if(read[0] == "userList")
    {
        ui->comboBox->clear();
        for(int i = 1; i < read.size()-1; i++)
            ui->comboBox->addItem(read[i]);
    }
    else
    {
        // 콤보박스에 번호 저장
        if(!userList.isEmpty())
        {
            int count = 0;
            for(int i = 0; i < userList.size(); i++)
            {
                if(read[0] == userList[i])
                {
                    count++;
                }
            }
            if(count == 0)
            {
                ui->comboBox->addItem(read[0]);
                userList.append(read[0]);
            }
        }
        else
        {
            ui->comboBox->addItem(read[0]);
            userList.append(read[0]);
        }

        ui->textEdit->append("[" + read[0] + "에게 받은 메시지]: " + read[1]);
        ui->textEdit->setAlignment(Qt::AlignLeft);
    }
}

// 메시지 보내기
void Widget::sendMessage()
{
    QString user = ui->comboBox->currentText();
    QString send = ui->lineEdit->text();
    QString sendText = "12#" + user + "#" + send + "#";

    ui->textEdit->append("[" + user + "에게 보내는 메시지]: " + send);
    ui->textEdit->setAlignment(Qt::AlignRight);
    tcpSocket->write(sendText.toUtf8());

    ui->lineEdit->clear();
}

// IP 주소 찾기
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
