#include "widget.h"
#include "ui_widget.h"

#define PATH "C:/Users/iot/Documents/bokpangeats/Bokpangeats_DATABASE"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // DB 열기
    bokpangDB = QSqlDatabase::addDatabase("QSQLITE");
    bokpangDB.setDatabaseName(PATH);
    if(!bokpangDB.open())
    {
        qDebug() << bokpangDB.lastError();
        return;
    }

    InitServer();

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slot_socketConnection()));
}

Widget::~Widget()
{
    delete ui;
}

// 초기화
void Widget::InitServer()
{
    QHostAddress hostAddress = getMyIp();
    qDebug() << hostAddress;
    int port = 25000;

    // tcpServer 서버 소켓 생성
    tcpServer = new QTcpServer(this);

    // 대기
    if(!tcpServer->listen(hostAddress, port))
    {
        qDebug() << tr("TCP Server") << tr("서버를 시작할 수 없습니다. 에러메세지 : %1.").arg(tcpServer->errorString());
        close();
        return; // 실패시 종료
    }
}

// 소켓 연결
void Widget::slot_socketConnection()
{
    // QList<QString> clntNum;
    QString count;

    // 연결 소켓 생성
    tcpSocket = tcpServer->nextPendingConnection();

    // qset_clntSKTList.insert(tcpSocket);
    // count = QString::number(qset_clntSKTList.size()-1);

    // // 소켓, 소켓 할당번호
    // qmap_userList[tcpSocket] = count;
    // tcpSocket->write(count.toUtf8());

    ui->access_user->append("소켓 연결 성공");

    // 메시지 읽기
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_read()));

    // 접속 종료 시
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(store_disconnected()));
}

// 데이터 읽어오기
void Widget::slot_read()
{
    QSqlQuery qry;

    QTcpSocket *clntSocket = (QTcpSocket*)sender();
    qDebug() << clntSocket;

    // 읽어온 메시지 저장
    QString readMessage = clntSocket->readAll();
    qDebug() << readMessage;

    // 메시지 나누기
    splitMessage = readMessage.split("#");

    if(splitMessage[0] == "0")          // 매장 클라이언트 구분
    {
        QString pw, login_result;
        qry.prepare("SELECT storePW FROM STORE_DATA WHERE storeID = :id");
        qry.bindValue(":id", splitMessage[1]);
        if(qry.exec()&&qry.next())
            pw = qry.value(0).toString();

        if(pw == splitMessage[2])
        {
            QString storeName;
            login_result = "success";

            qry.prepare("SELECT storeName FROM STORE_LIST WHERE storeID = :ID");
            qry.bindValue(":ID", splitMessage[1]);
            if(qry.exec()&&qry.next())
                storeName = qry.value(0).toString();
            ui->access_store->append("[" + storeName + "] 로그인");
        }
        else
            login_result = "fail";

        storeList[clntSocket] = splitMessage[1];

        // 특정 매장 소켓에 전송
        QTcpSocket *storeSocket = storeList.key(splitMessage[1]);
        storeSocket->write(login_result.toUtf8());

        qDebug() << storeList;

        // ui->access->append(qmap_userList.value(sendSocket)+"연결됨");
    }
    else if(splitMessage[0] == "1")     // 고객클라이언트_카테고리 선택 시
    {
        sendMessage.append("1#");

        qry.prepare("SELECT storeName, storePhoto FROM STORE_LIST WHERE storeCategory = :send");
        qry.bindValue(":send", splitMessage[1]);
        qry.exec();
        while(qry.next())
        {
            sendMessage.append(qry.value(0).toString());
            sendMessage.append("#");
            sendMessage.append(qry.value(1).toString());
            sendMessage.append("#");
        }
        qDebug() << sendMessage;
        clntSocket->write(sendMessage.toUtf8());
    }
    else if(splitMessage[0] == "2")     // 고객클라이언트_메뉴
    {
        sendMessage.append("2#");

        QString id;
        qry.prepare("SELECT storeID FROM STORE_LIST WHERE storeName = :name");
        qry.bindValue(":name", splitMessage[1]);
        if(qry.exec()&&qry.next())
        {
            id = qry.value(0).toString();
        }

        // 가게 이름, 가게 사진, 배달금액, 최소금액
        qry.prepare("SELECT storeName, storePhoto, deliveryFee, minAmount FROM STORE_LIST WHERE storeID = :ID");
        qry.bindValue(":ID", id);
        if(qry.exec()&&qry.next())
        {
            for(int i = 0; i < 4; i++)
            {
                sendMessage.append(qry.value(i).toString());
                sendMessage.append("#");
            }
        }
        qDebug() << sendMessage;

        // 메뉴 이름, 메뉴 설명, 메뉴 가격, 메뉴 사진
        qry.prepare("SELECT menuName, menuDescription, menuPrice, menuPhoto FROM STORE_MENU WHERE storeID = :ID");
        qry.bindValue(":ID", id);
        qry.exec();
        while(qry.next())
        {
            for(int i = 0; i < 4; i++)
            {
                sendMessage.append(qry.value(i).toString());
                sendMessage.append("#");
            }
        }
        qDebug() << sendMessage;
        clntSocket->write(sendMessage.toUtf8());
    }
    else if(splitMessage[0] == "3")     // 고객클라이언트_메뉴 옵션
    {
        sendMessage.append("3#");

        QString title;
        qry.prepare("SELECT optionTitle, optionName, addPrice FROM MENU_OPTION WHERE menuName = :name");
        qry.bindValue(":name", splitMessage[1]);
        qry.exec();
        while(qry.next())
        {
            if(title != qry.value(0).toString())
            {
                title = (qry.value(0).toString());
                sendMessage.append(qry.value(0).toString());
                sendMessage.append("#");
            }
            sendMessage.append(qry.value(1).toString());
            sendMessage.append("#");
            sendMessage.append(qry.value(2).toString());
            sendMessage.append("#");
        }
        clntSocket->write(sendMessage.toUtf8());
    }
    else if(splitMessage[0] == "4")     // 주문내역
    {
        sendMessage.append("4#");
        storeClnt();
    }
    else if(splitMessage[0] == "5")     // 주문내역 수락/거절
    {
        sendMessage.append("5#");

        // 주문 수락/거절 DB update
        qry.prepare("UPDATE USER_DATA SET orderResult = :result WHERE orderID = :orderID");
        qry.bindValue(":result", splitMessage[1]);
        qry.bindValue(":orderID", splitMessage[2]);
        qry.exec();

        QString result;
        qry.prepare("SELECT userPhone, orderResult FROM USER_DATA WHERE orderID = :id");
        qry.bindValue(":id", splitMessage[2]);
        if(qry.exec()&&qry.next())
        {
            result = qry.value(0).toString();
            sendMessage.append(qry.value(1).toString() + "#");
        }

        qDebug() << "결과: " << sendMessage;

        // 전화번호에 해당하는 소켓에게 메시지 전송하기
        // 유저에게 5#수락# / 5#거절#
        QTcpSocket *phoneSocket = userList.key(result);
        phoneSocket -> write(sendMessage.toUtf8());
    }
    else if(splitMessage[0] == "6")     // 전화번호 저장
    {
        sendMessage.append("6#");

        // 유저 전화번호로 특정 소켓 저장하기
        userList[clntSocket] = splitMessage[1];

        qDebug() << "userPhone: " << splitMessage[1];

        // 주문내역 보내주기
        // qry.prepare("SELECT orderResult FROM USER_DATA WHERE userPhone = :phone");
        // qry.bindValue(":phone", splitMessage[2]);
        // if(qry.exec()&&qry.next())
        // {
        //     sendMessage.append(qry.value(0).toString()+"#");
        // }

        qry.prepare("SELECT COUNT(*) FROM USER_DATA WHERE userPhone = :phone AND orderResult = '수락'");
        qry.bindValue(":phone", splitMessage[1]);
        if(qry.exec()&&qry.next())
        {
            sendMessage.append(qry.value(0).toString());
        }
        qDebug() << sendMessage;
        clntSocket->write(sendMessage.toUtf8());
    }
    else if(splitMessage[0] == "7")     // 검색어
    {
        sendMessage.append("7#");

        qry.exec("SELECT DISTINCT STORE_LIST.storeName FROM STORE_LIST INNER JOIN STORE_MENU ON STORE_LIST.storeID = STORE_MENU.storeID WHERE menuName LIKE '%"+splitMessage[1]+"%';");
        while(qry.next())
        {
            sendMessage.append(qry.value(0).toString());
            sendMessage.append("#");
        }
        qDebug() << sendMessage;
        clntSocket->write(sendMessage.toUtf8());
    }
    // else if(splitMessage[0] == "8")     // 주문내역 버튼
    // {
        // QString orderID;
        // // 고객 정보
        // qry.prepare("SELECT orderID, userAddress, userPhone, deliveryFee, total, payMethod, orderResult FROM USER_DATA WHERE userPhone = :phone");
        // qry.bindValue(":phone", splitMessage[1]);
        // if(qry.exec()&&qry.next())
        // {
        //     orderID = qry.value(0).toString();

        //     for(int i = 1; i<7; i++)
        //     {
        //         sendMessage.append(qry.value(i).toString() + "#");
        //     }
        // }

        // qDebug() << "order id: " << orderID << "\nsendMessage: " << sendMessage;

        // // 주문정보 유무
        // if(sendMessage.isEmpty())       // 주문정보 없음
        // {
        //     sendMessage = "null8#";
        // }
        // else                            // 주문정보 있음
        // {
        //     sendMessage.prepend("8#");

        //     qry.prepare("SELECT menuName FROM USER_ORDER WHERE orderID = :id");
        //     qry.bindValue(":id", "16154");
        //     qry.exec();
        //     while(qry.next())
        //     {
        //         sendMessage.append(qry.value(0).toString() + "\n");
        //     }
        //     sendMessage.append("#");
        // }

        // qDebug() << sendMessage;
        // clntSocket ->write(sendMessage.toUtf8());
    // }
    // else if(splitMessage[0] == "8")
    // {
    //     QString orderID;

    //     // 고객 정보
    //     qry.prepare("SELECT orderID, userAddress, userPhone, deliveryFee, total, payMethod, orderResult FROM USER_DATA WHERE userPhone = :phone");
    //     qry.bindValue(":phone", splitMessage[1]);
    //     if(qry.exec()&&qry.next())
    //     {
    //         orderID = qry.value(0).toString();

    //         for(int i = 1; i<7; i++)
    //         {
    //             sendMessage.append(qry.value(i).toString() + "#");
    //         }
    //     }

    //     qDebug() << "order id: " << orderID << "\nsendMessage: " << sendMessage;

    //     // 주문정보 유무
    //     if(sendMessage.isEmpty())       // 주문정보 없음
    //     {
    //         sendMessage = "null8#";
    //     }
    //     else                            // 주문정보 있음
    //     {
    //         sendMessage.prepend("8#");

    //         qry.prepare("SELECT menuName FROM USER_ORDER WHERE orderID = :id");
    //         qry.bindValue(":id", "16154");
    //         qry.exec();
    //         while(qry.next())
    //         {
    //             sendMessage.append(qry.value(0).toString() + "\n");
    //         }
    //         sendMessage.append("#");
    //     }

    //     qDebug() << sendMessage;
    //     clntSocket ->write(sendMessage.toUtf8());
    // }
    else if(splitMessage[0] == "9")     // 고객센터 클라이언트 소켓번호 저장
    {
        // (고객센터->서버) 9#
        service = clntSocket;
        qDebug() << service;
    }
    else if(splitMessage[0] == "10")        // 문의하기의 전화번호 입력 버튼
    {
        int haveKey = 0;

        QMap<QTcpSocket *, QString>::iterator i;
        for(i = serviceList.begin(); i != serviceList.end(); i++)
        {
            if(clntSocket == i.key())
                haveKey++;
        }

        if(haveKey == 0)
        {
            serviceList[clntSocket] = QString::number(count);
            count++;
        }
        qDebug() << serviceList;
    }
    else if(splitMessage[0] == "11")         // 문의하기 메세지 전송
    {
        // (고객->서버) 11#메시지#
        sendMessage.append(serviceList.value(clntSocket) + "#" + splitMessage[1] + "#");

        qDebug() << sendMessage;

        // (서버->고객센터) 고객번호#메시지# 보내기
        service->write(sendMessage.toUtf8());
    }
    else if(splitMessage[0] == "12")
    {
        // (고객센터->서버) 12#고객번호#메시지
        sendMessage.append("12#상담사#" + splitMessage[2] + "#");

        QTcpSocket *clnt = serviceList.key(splitMessage[1]);

        clnt->write(sendMessage.toUtf8());
    }
    sendMessage.clear();
}

// 주문내역 송수신
void Widget::storeClnt()
{
    QSqlQuery qry;

    // 주문번호 랜덤난수
    QList<int> orderNum;
    unsigned int randomOrder;
    bool randNum = true;
    QString orderID;

    QString appendMessage;


    //// 주문내역 판단
    // 주문내역 붙이기
    for(int i = 1; i < splitMessage.size()-1; i++)
    {
        appendMessage.append(splitMessage[i]);
        appendMessage.append("#");
    }
    qDebug() << "appendMessage: " << appendMessage;

    splitMessage = appendMessage.split("^^");

    // 알맞게 자르기
    QList<QString> firstSplit;
    QList<QString> secondSplit;

    firstSplit = splitMessage[0].split("#");    // 주소, 전화번호, 매장명, (주문~)
    secondSplit = splitMessage[1].split("#");   // 주문 금액, 배달비, 총 결제금액, 결제방식

    qDebug() << "firstSplit   " << firstSplit;
    qDebug() << "secondSplit   " << secondSplit;


    //// 주문번호 생성
    // 주문번호 중복 제거
    qry.prepare("SELECT orderID FROM USER_DATA;");
    qry.exec();
    while(qry.next())
    {
        orderNum.append(qry.value(0).toInt());
    }
    // 주문번호 랜덤난수
    while(randNum)
    {
        randomOrder = QRandomGenerator::global()->bounded(99999);
        for (int i = 0; i < orderNum.size(); i++)
        {
            if(randomOrder == orderNum[i])
            {
                randNum = true;
                break;
            }
            else
            {
                randNum = false;
            }
        }
    }

    orderID = QString::number(randomOrder);
    qDebug() << "orderNum: " << orderNum << "\norderID: " << orderID;


    //// 매장ID 찾아오기
    QString storeID;
    // QString fee;
    // qry.prepare("SELECT storeID, deliveryFee FROM STORE_LIST WHERE storeName = :name");
    qry.prepare("SELECT storeID FROM STORE_LIST WHERE storeName = :name");
    qry.bindValue(":name", firstSplit[2]);
    if(qry.exec()&&qry.next())
    {
        storeID = qry.value(0).toString();
        // fee = qry.value(1).toString();
    }

    //// DB 저장
    // 주문 정보
    qry.prepare("INSERT INTO USER_DATA VALUES(:storeID, :orderID, :address, :phone, :delivery, :payment, :total, :result, :method)");
    qry.bindValue(":storeID", storeID);         // 매장ID
    qry.bindValue(":orderID", orderID);         // 주문번호
    qry.bindValue(":address", firstSplit[0]);   // 고객 주소
    qry.bindValue(":phone", firstSplit[1]);     // 고객 전화번호
    qry.bindValue(":delivery", secondSplit[1]); // 배달비
    qry.bindValue(":payment", secondSplit[0]);  // 주문금액
    qry.bindValue(":total", secondSplit[2]);    // 총 결제 금액
    qry.bindValue(":result", "주문대기");
    qry.bindValue(":method", secondSplit[3]);   // 결제방법
    qry.exec();

    int menu = 3;
    int count = 4;

    // 주문 내역
    for(int i = 0; i<(firstSplit.size()/4); i++)
    {
        qry.prepare("INSERT INTO USER_ORDER VALUES(:orderID, :menuName, :count)");
        qry.bindValue(":orderID", orderID);
        if(i == 0)
        {
            qry.bindValue(":menuName", firstSplit[menu]);
            qry.bindValue(":count", firstSplit[count]);
        }
        else
        {
            qry.bindValue(":menuName", firstSplit[menu+2]);
            qry.bindValue(":count", firstSplit[count+2]);
        }
        qry.exec();
    }

    // qry.prepare("INSERT INTO USER_ORDER VALUES(:orderID, :menuName, :count)");
    // qry.bindValue(":orderID", orderID);
    // qry.bindValue(":menuName", firstSplit[menu]);
    // qry.bindValue(":count", firstSplit[count]);
    // qry.exec();

    // for(int i = 5; i<firstSplit.size()-2; i+=2)
    // {
    //     qry.prepare("INSERT INTO USER_ORDER VALUES(:orderID, :menuName, :count)");
    //     qry.bindValue(":orderID", orderID);
    //     qry.bindValue(":menuName", firstSplit[menu+2]);
    //     qry.bindValue(":count", firstSplit[count+2]);
    //     qry.exec();
    // }

    // 매장 클라이언트에 데이터 송신
    qry.prepare("SELECT USER_DATA.orderID, USER_DATA.userAddress, USER_DATA.userPhone, USER_ORDER.menuName, USER_ORDER.count, USER_DATA.total FROM USER_DATA INNER JOIN USER_ORDER ON USER_DATA.orderID = USER_ORDER.orderID WHERE USER_DATA.storeID = :id AND USER_DATA.orderID = :orderID");
    qry.bindValue(":id", storeID);
    qry.bindValue(":orderID", orderID);
    qry.exec();
    while(qry.next())
    {
        for(int i = 0; i < 6; i++)
        {
            sendMessage.append(qry.value(i).toString());
            sendMessage.append("#");
        }
    }

    qDebug() << "여기까지 옴";

    QTcpSocket *storeSocket = storeList.key(storeID);
    storeSocket->write(sendMessage.toUtf8());

    sendMessage.clear();
}

// 접속 종료
void Widget::store_disconnected()
{
    QSqlQuery qry;
    QString disStore;

    // 접속 끊은 소켓 찾기
    QTcpSocket *disSocket = (QTcpSocket*)sender();
    qDebug() << "disSocket: " << disSocket;

    // 접속 끊은 매장 이름 찾기
    QString disID = storeList.value(disSocket);
    // qDebug() << "disID: " << disID;

    // 접속 끊은 유저 찾기
    QString user = serviceList.value(disSocket);
    qDebug() << "serviceList: " << serviceList;

    qry.prepare("SELECT storeName FROM STORE_LIST WHERE storeID = :ID");
    qry.bindValue(":ID", disID);
    if(qry.exec()&&qry.next())
        disStore = qry.value(0).toString();
    // qDebug() << "disStore" << disStore;

    // 접속 끊긴 소켓 제거
    if(disStore.isEmpty())
        ui->disconnect_user->append("유저 접속 종료");
    else
    {
        ui->access_store->append("[" + disStore + "] 로그아웃");
    }

    storeList.remove(disSocket);        // 매장 삭제
    serviceList.remove(disSocket);      // 채팅 삭제


    // 접속 중인 유저만 보내기
    QString sendM;
    if(!user.isEmpty())
    {
        sendM = ("userList#");
        QMap<QTcpSocket *, QString>::iterator i;
        for(i = serviceList.begin(); i != serviceList.end(); i++)
        {
            sendM.append(QString("%1").arg(i.value()) + "#");
        }

        qDebug() << sendM;

        service->write(sendM.toUtf8());
    }
}

QHostAddress Widget::getMyIp()
{
    QHostAddress myAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // localhost(127.0.0.1) 가 아닌 것을 사용
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            //qDebug() << ipAddressesList.at(i);
            myAddress = ipAddressesList.at(i);
            break;
        }
    }

    // 인터넷이 연결되어 있지 않다면, localhost(127.0.0.1) 사용
    if (myAddress.toString().isEmpty())
        myAddress = QHostAddress(QHostAddress::LocalHost);

    return myAddress;
}
