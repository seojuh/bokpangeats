#include "customer.h"
#include "ui_customer.h"

Customer::Customer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Customer)
    , isAddingOrder(false)
{
    ui->setupUi(this);
    initClnt();
    QHostAddress serverAddress = getMyIP();
    tcpSocket->connectToHost(serverAddress, 25000);
    //시작화면
    ui->stackedWidget->setCurrentIndex(0);
    //광고배너
    QMovie *movie = new QMovie(":/new/prefix1/project/homeui/advertise.gif");
    QMovie *movie2 = new QMovie(":/new/prefix1/project/homeui/advertise2.gif");
    ui->advertise->setMovie(movie);
    ui->advertise2->setMovie(movie2);
    ui->advertise->setScaledContents(true);
    ui->advertise2->setScaledContents(true);
    movie->start();
    movie2->start();
    passwordValid();
    ui->orderview->setColumnWidth(0,350);

}

Customer::~Customer()
{
    delete ui;
}

//클라이언트 소켓생성
void Customer::initClnt()
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &Customer::connectedToServer);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Customer::slot_read);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    tcpSocket->connectToHost(QHostAddress("10.10.20.114"), 25000);
}

//서버연결 확인
void Customer::connectedToServer()
{
    qDebug() << "Connected to server.";
}

//서버연결 오류
void Customer::connectionError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Connection error:" << socketError;
}

QHostAddress Customer::getMyIP()
{
    QHostAddress myAddress;
    QList<QHostAddress> ipAddresseslist = QNetworkInterface::allAddresses();
    for(int i=0; i<ipAddresseslist.size(); ++i)
    {
        if(ipAddresseslist.at(i) != QHostAddress::LocalHost && ipAddresseslist.at(i).toIPv4Address())
        {
            myAddress = ipAddresseslist.at(i);
            break;
        }
    }
    if(myAddress.toString().isEmpty())
    {
        myAddress = QHostAddress(QHostAddress::LocalHost);
    }
    return myAddress;
}

void Customer::on_chickenbutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    message = "1#치킨";
    tcpSocket->write(message.toUtf8());
    acticeRadiobutton();
}



void Customer::slot_read()
{

    if(tcpSocket->bytesAvailable()>=0)
    {
        QString readData = tcpSocket ->readAll();
        processData(readData);
        tcpSocket->flush();

        if(readData.startsWith("6#"))//서버에서 받는거를 조건으로 실행시키기;
        {
            bool ok;
            int orderCount = readData.split("#")[1].toInt(&ok);

            if(ok)
            {
                applyDiscount(orderCount);
            }
        }

        if(readData.startsWith("7#"))
        {
            SearchResult(readData);
        }
    }
}

void Customer::processData(const QString &data)
{
    list = data.split("#");

    qDebug()<<list;


    if(list[0]== "1")
    {
        //이름 짝수, 사진 홀수
        QPixmap pixmap(list[2]);
        QPixmap pixmap2(list[4]);
        QPixmap pixmap3(list[6]);
        QIcon icon(pixmap);
        QIcon icon2(pixmap2);
        QIcon icon3(pixmap3);
        //버튼 사이즈 조절
        QSize buttonSize = ui->storepicture->size();
        pixmap = pixmap.scaled(buttonSize, Qt::KeepAspectRatio);
        //매장선택 정보read
        ui->storename->setText(list[1]);
        ui->storename_2->setText(list[3]);
        ui->storename_3->setText(list[5]);
        ui->storepicture->setIcon(icon);
        ui->storepicture_2->setIcon(icon2);
        ui->storepicture_3->setIcon(icon3);
        ui->storepicture->setIconSize(buttonSize);
        ui->storepicture_2->setIconSize(buttonSize);
        ui->storepicture_3->setIconSize(buttonSize);
    }

    //주문하기 버튼 정보 read
    else if(list[0]=="2")
    {
        QString imagepath = list[8];
        QString imagepath2= list[12];
        QString imagepath3= list[16];
        QString imagepath4= list[20];
        QString imagepath5= list[24];
        QPixmap menuPixmap(imagepath);
        QPixmap menuPixmap2(imagepath2);
        QPixmap menuPixmap3(imagepath3);
        QPixmap menuPixmap4(imagepath4);
        QPixmap menuPixmap5(imagepath5);
        QSize buttonSize = menuPixmap.size();
        menuPixmap = menuPixmap.scaled(buttonSize, Qt::KeepAspectRatio);
        ui->orderstore->setPixmap(QPixmap(list[2]));
        ui->orderstore->setScaledContents(true);
        ui->ordertitle->setText(list[1]);
        ui->deliveryprice->setText(list[3]);
        ui->minprice->setText(list[4]);
        ui->menuname->setText(list[5]);
        ui->menuname_2->setText(list[9]);
        ui->menuname_3->setText(list[13]);
        ui->menuname_4->setText(list[17]);
        ui->menuname_5->setText(list[21]);
        ui->menudecription->setText(list[6]);
        ui->menudecription_2->setText(list[10]);
        ui->menudecription_3->setText(list[14]);
        ui->menudecription_4->setText(list[18]);
        ui->menudecription_5->setText(list[22]);
        ui->menuprice->setText(list[7]);
        ui->menuprice_2->setText(list[11]);
        ui->menuprice_3->setText(list[15]);
        ui->menuprice_4->setText(list[19]);
        ui->menuprice_5->setText(list[23]);
        ui->menupicture->setIcon(QIcon(menuPixmap));
        ui->menupicture_2->setIcon(QIcon(menuPixmap2));
        ui->menupicture_3->setIcon(QIcon(menuPixmap3));
        ui->menupicture_4->setIcon(QIcon(menuPixmap4));
        ui->menupicture_5->setIcon(QIcon(menuPixmap5));
        ui->menupicture->setIconSize(buttonSize);
        ui->menupicture_2->setIconSize(buttonSize);
        ui->menupicture_3->setIconSize(buttonSize);
        ui->menupicture_4->setIconSize(buttonSize);
        ui->menupicture_5->setIconSize(buttonSize);
    }
    //메뉴 담기
    else if(list[0]=="3")
    {
        ui->one_optionselect->setText(list[1]);
        ui->two_optionselect->setText(list[6]);
        ui->one_option1->setText(list[2]);
        ui->one_option2->setText(list[4]);
        ui->one_optionprice1->setText(list[3]);
        ui->one_optionprice2->setText(list[5]);
        ui->two_option1->setText(list[7]);
        ui->two_option_2->setText(list[9]);
        ui->two_option_3->setText(list[11]);
        ui->two_option_4->setText(list[13]);
        ui->two_optionprice1->setText(list[8]);
        ui->two_optionprice2->setText(list[10]);
        ui->two_optionprice3->setText(list[12]);
        ui->two_optionprice4->setText(list[14]);
    }

    else if(list[0]=="7")
    {
        SearchResult(list[3]);
    }

    else if(list[0]=="5")
    {
        if(list[1]=="수락")
        {
            ui->orderchange->setText("주문 승인");
        }
        else if(list[1]=="거절")
        {
            ui->orderchange->setText("주문 거절");
        }
    }

    if(list[0] == "12")
    {
        ui->logedit->append("[" + list[1] + "]: " + list[2]);
        ui->logedit->setAlignment(Qt::AlignLeft);
    }

}

void Customer::on_pizzabutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    message = "1#피자";
    tcpSocket->write(message.toUtf8());
    acticeRadiobutton();
}


void Customer::on_dongasbutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    message = "1#돈까스";
    tcpSocket->write(message.toUtf8());
    acticeRadiobutton();
}


void Customer::on_bunsikbutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    message = "1#분식";
    tcpSocket->write(message.toUtf8());
    acticeRadiobutton();
}


void Customer::on_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Customer::on_storepicture_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    QString storename = ui->storename->text();
    QString message = "2#"+storename;
    tcpSocket->write(message.toUtf8());
}


void Customer::on_storepicture_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    QString storename = ui->storename_2->text();
    QString message = "2#"+storename;
    tcpSocket->write(message.toUtf8());
}


void Customer::on_storepicture_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    QString storename = ui->storename_3->text();
    QString message = "2#"+storename;
    tcpSocket->write(message.toUtf8());
}

//매장돌아가기
void Customer::on_back_2_clicked()
{
    if(isAddingOrder)
    {
        QMessageBox::warning(this,"경고","추가 주문 중에는 다른매장을 선택할 수 없습니다.");
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
        tcpSocket->write(message.toUtf8());
    }
}

//메뉴 선택 클릭 매서드
void Customer::on_menupicture_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    originalMenuName = ui->menuname->text();
    QString menuname = ui->menuname->text();
    QString price = ui->menuprice->text();
    QString message = "3#"+menuname;
    tcpSocket->write(message.toUtf8());
    QPixmap pixmap = ui->menupicture->icon().pixmap(ui->menupicture->iconSize());
    ui->optionmenupicture->setPixmap(pixmap);
    ui->optionmenupicture->setScaledContents(true);
    ui->optionmenuname->setText(menuname);
    ui->optionmenuprice->setText(price);
    updateOrderAndTotalAmounts();
    acticeRadiobutton();
}


void Customer::on_menupicture_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    originalMenuName = ui->menuname_2->text();
    QString menuname = ui->menuname_2->text();
    QString price = ui->menuprice_2->text();
    QString message = "3#"+menuname;
    tcpSocket->write(message.toUtf8());
    QPixmap pixmap = ui->menupicture_2->icon().pixmap(ui->menupicture_2->iconSize());
    ui->optionmenupicture->setPixmap(pixmap);
    ui->optionmenupicture->setScaledContents(true);
    ui->optionmenuname->setText(menuname);
    ui->optionmenuprice->setText(price);
    updateOrderAndTotalAmounts();
    acticeRadiobutton();
}


void Customer::on_menupicture_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    originalMenuName = ui->menuname_3->text();
    QString menuname = ui->menuname_3->text();
    QString price = ui->menuprice_3->text();
    QString message = "3#"+menuname;
    tcpSocket->write(message.toUtf8());
    QPixmap pixmap = ui->menupicture_3->icon().pixmap(ui->menupicture_3->iconSize());
    ui->optionmenupicture->setPixmap(pixmap);
    ui->optionmenupicture->setScaledContents(true);
    ui->optionmenuname->setText(menuname);
    ui->optionmenuprice->setText(price);
    updateOrderAndTotalAmounts();
    acticeRadiobutton();
}


void Customer::on_menupicture_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    originalMenuName = ui->menuname_4->text();
    QString menuname = ui->menuname_4->text();
    QString price = ui->menuprice_4->text();
    QString message = "3#"+menuname;
    tcpSocket->write(message.toUtf8());
    QPixmap pixmap = ui->menupicture_4->icon().pixmap(ui->menupicture_4->iconSize());
    ui->optionmenupicture->setPixmap(pixmap);
    ui->optionmenupicture->setScaledContents(true);
    ui->optionmenuname->setText(menuname);
    ui->optionmenuprice->setText(price);
    updateOrderAndTotalAmounts();
    acticeRadiobutton();
}


void Customer::on_menupicture_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    originalMenuName = ui->menuname_5->text();
    QString menuname = ui->menuname_5->text();
    QString price = ui->menuprice_5->text();
    QString message = "3#"+menuname;
    tcpSocket->write(message.toUtf8());
    QPixmap pixmap = ui->menupicture_5->icon().pixmap(ui->menupicture_5->iconSize());
    ui->optionmenupicture->setPixmap(pixmap);
    ui->optionmenupicture->setScaledContents(true);
    ui->optionmenuname->setText(menuname);
    ui->optionmenuprice->setText(price);
    updateOrderAndTotalAmounts();
    acticeRadiobutton();
}

//수량 더하기 빼기 매서드
void Customer::on_plusbutton_clicked()
{
    changeQuantity(1);
}
void Customer::on_minusbutton_clicked()
{
    changeQuantity(-1);
}

void Customer::changeQuantity(int delta)
{
    int quantity = ui->optionmenunum->text().toInt() + delta;
    quantity = qMax(1, quantity);
    ui->optionmenunum->setText(QString::number(quantity));

    updateTotalPrice();
}

//전체가격 업데이트 매서드
void Customer::updateTotalPrice()
{
    int quantity = ui->optionmenunum->text().toInt();
    double pricePerItem = ui->menuprice->text().toDouble();
    double totalPrice = quantity * pricePerItem;

    if (ui->radioButton->isChecked())
    {
        totalPrice += 0;
    }
    if (ui->radioButton_2->isChecked())
    {
        totalPrice += ui->one_optionprice2->text().toDouble();
    }
    if (ui->radioButton_3->isChecked())
    {
        totalPrice += ui->two_optionprice1->text().toDouble();
    }
    if(ui->radioButton_4->isChecked())
    {
        totalPrice += ui->two_optionprice2->text().toDouble();
    }
    if(ui->radioButton_5->isChecked())
    {
        totalPrice+= ui->two_optionprice3->text().toDouble();
    }
    if(ui->radioButton_6->isChecked())
    {
        totalPrice+= ui->two_optionprice4->text().toDouble();
    }
    ui->optionmenuprice->setText(QString::number(totalPrice));
}
//1옵션 선택
void Customer::on_radioButton_clicked()
{
    QString optionName = ui->one_option1->text();
    addOptionToMenuName(optionName);
    QMessageBox::information(this,"","옵션: "+ optionName +" 선택되었습니다.");
    currentOption1 = ui->one_option1->text();
    updateOptionDisplay();
    updateTotalPrice();
}

void Customer::on_radioButton_2_clicked()
{
    QString optionName = ui->one_option2->text();
    addOptionToMenuName(optionName);
    QMessageBox::information(this,"","옵션: "+ optionName +" 선택되었습니다.");
    currentOption1 = ui->one_option2->text();
    updateOptionDisplay();
    updateTotalPrice();
}

//2옵션 선택
void Customer::on_radioButton_3_clicked()
{
    QString optionName = ui->two_option1->text();
    addOptionToMenuName(optionName);
    QMessageBox::information(this,"","옵션: "+ optionName +" 선택되었습니다.");
    currentOption2 = ui->two_option1->text();
    updateOptionDisplay();
    updateTotalPrice();

}

void Customer::on_radioButton_4_clicked()
{
    QString optionName = ui->two_option_2->text();
    addOptionToMenuName(optionName);
    QMessageBox::information(this,"","옵션: "+ optionName +" 선택되었습니다.");
    currentOption2 = ui->two_option_2->text();
    updateOptionDisplay();
    updateTotalPrice();

}

void Customer::on_radioButton_5_clicked()
{
    QString optionName = ui->two_option_3->text();
    addOptionToMenuName(optionName);
    QMessageBox::information(this,"","옵션: "+ optionName +" 선택되었습니다.");
    currentOption2 = ui->two_option_3->text();
    updateOptionDisplay();
    updateTotalPrice();

}

void Customer::on_radioButton_6_clicked()
{
    QString optionName = ui->two_option_4->text();
    addOptionToMenuName(optionName);
    QMessageBox::information(this,"","옵션: "+ optionName +" 선택되었습니다.");
    currentOption2 = ui->two_option_4->text();
    updateOptionDisplay();
    updateTotalPrice();

}
//메뉴 옵션 이름 더하기 매서드
void Customer::addOptionToMenuName(const QString& optionName)
{
    QString currentMenuName = originalMenuName;
    currentMenuName += " (" + optionName + ")";
    ui->optionmenuname->setText(currentMenuName);
}

void Customer::updateOptionDisplay()
{
    QString menuName = originalMenuName;

    if (!currentOption1.isEmpty()) {
        menuName += " (" + currentOption1;
        if (!currentOption2.isEmpty()) {
            menuName += ", " + currentOption2;
        }
        menuName += ")";
    } else if (!currentOption2.isEmpty()) {
        menuName += " (" + currentOption2 + ")";
    }

    ui->optionmenuname->setText(menuName);
}

//메뉴 추가 매서드
void Customer::on_addmenu_clicked()
{
    QString menuName = ui->optionmenuname->text();
    int quantity = ui->optionmenunum->text().toInt();
    double price = ui->optionmenuprice->text().toDouble();
    QString storeName = ui->ordertitle->text();

    int newRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRow);

    ui->tableWidget->setItem(newRow, 0, new QTableWidgetItem(menuName));
    ui->tableWidget->setItem(newRow, 1, new QTableWidgetItem(QString::number(quantity)));
    ui->tableWidget->setItem(newRow, 2, new QTableWidgetItem(QString::number(price)));

    ui->basketstorename->setText(storeName);
    updateOrderAndTotalAmounts();

    ui->stackedWidget->setCurrentIndex(4);
}

//주문버튼 누를시 서버로 전송
void Customer::on_orderbutton_clicked()
{
    QString homeAddress = ui->home->text();
    QString phoneNumber = ui->phone->text();

    if (!ui->cashkakao->isChecked() && !ui->cashnaver->isChecked() && !ui->cashtoss->isChecked())
    {
        QMessageBox::warning(this, "결제 방법 선택", "결제 방법을 선택해주세요.");
        return;
    }
    if(homeAddress.isEmpty() || phoneNumber.isEmpty())
    {
        QMessageBox::warning(this, "입력 오류", "주소와 전화번호를 입력하지 않으면 주문할 수 없습니다.");
        return;
    }

    updateOrderAndTotalAmounts();
    QString orderDetails;
    orderDetails +="4#";//서버에 보낼 주문정보 구별 앞자리
    orderDetails += ui->home->text() + "#";
    orderDetails += ui->phone->text() + "#";
    orderDetails += ui->basketstorename->text() +"#";
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {

        orderDetails += ui->tableWidget->item(row, 0)->text() + "#"; // 메뉴 이름
        orderDetails += ui->tableWidget->item(row, 1)->text() + "#"; // 수량
        // orderDetails += ui->tableWidget->item(row, 2)->text(); // 가격
        if(row!=ui->tableWidget->rowCount()-2)
        {
            orderDetails +="#";
        }
    }
    orderDetails += "^^";
    orderDetails += ui->basketprice->text() +"#";
    orderDetails += ui->basketdelivery->text() + "#";
    orderDetails += ui->baskettotalprice->text()+"#";
    if(ui->cashkakao->isChecked())
    {
        orderDetails += "KakaoPay#";
    }
    else if(ui->cashnaver->isChecked())
    {
        orderDetails += "NaverPay#";
    }
    else if(ui->cashtoss->isChecked())
    {
        orderDetails += "TossPay#";
    }
    qDebug() << orderDetails;

    tcpSocket->write(orderDetails.toUtf8());

    int startRow = ui->orderview->rowCount(); // 기존 행의 개수를 가져옴

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        ui->orderview->insertRow(startRow + row); // 새 행을 추가
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
        {
            if (ui->tableWidget->item(row, col)) { // null 체크
                ui->orderview->setItem(startRow + row, col, new QTableWidgetItem(*ui->tableWidget->item(row, col)));
            }
        }
    }

    if (!homeAddress.isEmpty())
    {
        ui->orderview->insertRow(startRow);
        ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("고객님 주소: " + homeAddress));
    }
    if (!phoneNumber.isEmpty()) {
        ui->orderview->insertRow(startRow);
        ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("고객님 전화번호: " + phoneNumber));
    }
    ui->orderview->insertRow(startRow);
    ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("주문하신 매장 이름: " + ui->basketstorename->text()));
    ui->orderview->insertRow(startRow);
    ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("배달비: " + ui->deliveryprice->text()));
    ui->orderview->insertRow(startRow);
    // ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("메뉴 가격: "+ui->optionmenuprice->text()));
    // ui->orderview->insertRow(startRow);
    ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("총 주문 금액: " + ui->baskettotalprice->text()));
    QString paymentMethod = ui->cashkakao->isChecked() ? "KakaoPay" : ui->cashnaver->isChecked() ? "NaverPay" : "TossPay";
    ui->orderview->insertRow(startRow);
    ui->orderview->setItem(startRow++, 0, new QTableWidgetItem("고객님 결제 수단: " + paymentMethod));

    // 주문 완료 알림
    QMessageBox::information(this,"","주문이 완료되었습니다.");

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->baskettotalprice->clear();
    ui->stackedWidget->setCurrentIndex(0);
    resetOrderForm();
    clearOrderInfo();
    ui->phone->setText("");
    isAddingOrder = false;
    ui->orderchange->setText("주문 대기");
    ui->orderchange->setVisible(true);
    ui->phoneright->setEnabled(true);
    ui->searchedit->clear();
    ui->phoneline->clear();
}

//추가주문 매서드
void Customer::on_addorder_clicked()
{
    double totalOrderAmount = 0.0;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        int itemQuantity = ui->tableWidget->item(row, 1)->text().toInt();
        double itemPrice = ui->tableWidget->item(row, 2)->text().toDouble();
        totalOrderAmount += itemQuantity * itemPrice;
    }

    double deliveryPrice = ui->deliveryprice->text().toDouble();
    double finalTotalAmount = totalOrderAmount + deliveryPrice;

    ui->basketprice->setText(QString::number(totalOrderAmount));
    ui->baskettotalprice->setText(QString::number(finalTotalAmount));

    ui->stackedWidget->setCurrentIndex(2);
    clearOrderInfo();
    resetOrderForm();
    isAddingOrder = true;
}

//총가격 업데이트 매서드
void Customer::updateOrderAndTotalAmounts()
{
    double totalOrderAmount = 0.0;
    double deliveryPrice = ui->deliveryprice->text().toDouble();

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        int itemQuantity = ui->tableWidget->item(row, 1)->text().toInt();
        double itemPrice = ui->tableWidget->item(row, 2)->text().toDouble();
        totalOrderAmount += itemQuantity * itemPrice;
    }

    double finalTotalAmount = totalOrderAmount + deliveryPrice;
    ui->basketprice->setText(QString::number(totalOrderAmount));
    ui->basketdelivery->setText(QString::number(deliveryPrice));
    ui->baskettotalprice->setText(QString::number(finalTotalAmount));
}

//라디오버튼, 수량 초기화 매서드
void Customer::resetOrderForm()
{
    ui->radioButton->setCheckable(false);
    ui->radioButton_2->setCheckable(false);
    ui->radioButton_3->setCheckable(false);
    ui->radioButton_4->setCheckable(false);
    ui->radioButton_5->setCheckable(false);
    ui->radioButton_6->setCheckable(false);
    ui->cashkakao->setCheckable(false);
    ui->cashnaver->setCheckable(false);
    ui->cashtoss->setCheckable(false);
    ui->optionmenunum->setText("1");
}

void Customer::acticeRadiobutton()
{
    ui->radioButton->setCheckable(true);
    ui->radioButton_2->setCheckable(true);
    ui->radioButton_3->setCheckable(true);
    ui->radioButton_4->setCheckable(true);
    ui->radioButton_5->setCheckable(true);
    ui->radioButton_6->setCheckable(true);
    ui->cashkakao->setCheckable(true);
    ui->cashnaver->setCheckable(true);
    ui->cashtoss->setCheckable(true);
}
//주소, 전화번호 초기화 매서드
void Customer::clearOrderInfo()
{
    ui->home->setText("");
}

void Customer::passwordValid()
{
    QRegularExpression regex("[0-9]*"); // 숫자만 허용하는 정규 표현식
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);
    ui->phone->setValidator(validator);
}

void Customer::on_orderrightbutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}


void Customer::on_homebutton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Customer::on_searchbutton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}


void Customer::on_searchbutton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

//검색창에서 검색 버튼 매서드
void Customer::on_searchpageburtton_clicked()
{
    QString searchItem = ui->searchedit->text().trimmed();
    if(!searchItem.isEmpty())
    {
        QString message = "7#"+searchItem;//보낼 메시지
        tcpSocket->write(message.toUtf8());
    }
}

//고객센터 연결
void Customer::on_inquirybutton_clicked()
{
    message = "10#";
    tcpSocket->write(message.toUtf8());
    ui->stackedWidget->setCurrentIndex(8);
    ui->logedit->clear();
    ui->inquryedit->clear();
}


void Customer::on_inquirybutton_2_clicked()
{
    message = "10#";
    tcpSocket->write(message.toUtf8());
    ui->stackedWidget->setCurrentIndex(8);
    ui->logedit->clear();
    ui->inquryedit->clear();
}

//휴대전화 확인 매서드
void Customer::on_phoneright_clicked()
{
    QString phoneNumber = ui->phone->text();

    if(phoneNumber.length() == 11)
    {
        QString message = "6#" + phoneNumber; // 내가 보내줘야될 조건의 문장
        tcpSocket->write(message.toUtf8());

        ui->phoneright->setEnabled(false);
    }
    else
    {
        QMessageBox::warning(this,"입력 오류","휴대폰 번호 11자리를 제대로 입력해주세요.");
    }
}

//배달비 할인 추가기능 매서드
void Customer::applyDiscount(int orderCount)
{
    if(orderCount == 0)
    {
        ui->deliveryprice->setText("0");
        ui->basketdelivery->setText("0");
        QMessageBox::information(this,"","처음 주문이시면 배달비가 무료입니다.\n주문하기를 누르시면 적용됩니다.");
    }
    else if(orderCount%10 == 0)
    {
        ui->deliveryprice->setText("0");
        ui->basketdelivery->setText("0");
        QMessageBox::information(this,"","많이 주문해주셔서 감사합니다 배달비를 무료로 적용하겠습니다.\n주문하기를 누르시면 적용됩니다.");
    }
}

//검색 시 리스트뷰에 출력되도록 하는 매서드
void Customer::SearchResult(const QString &results)
{
    QString dataWithoutPrefix = results.mid(2);
    QStringList items = dataWithoutPrefix.split("#");

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(items);
    ui->orderlist->setModel(model);
}

void Customer::on_orderlist_clicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }


    QString selectedItem = index.data(Qt::DisplayRole).toString();
    if (selectedItem.isEmpty())
    {
        return;
    }

    QString message = "2#" + selectedItem;
    tcpSocket->write(message.toUtf8());

    ui->stackedWidget->setCurrentIndex(2);

    ui->searchedit->clear();
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->orderlist->model());
    if (model)
    {
        model->setStringList(QStringList());
    }
}

void Customer::on_phoneright_2_clicked()
{
    QString phoneNumber = ui->phoneline->text();

    if(phoneNumber.length() == 11)
    {
        QString message = "8#" + phoneNumber; // 서버로 보낼 메시지
        tcpSocket->write(message.toUtf8());
        ui->stackedWidget->setCurrentIndex(5);
    }
    else
    {
        QMessageBox::warning(this,"입력 오류","휴대폰 번호 11자리를 제대로 입력해주세요.");
    }
}


void Customer::on_inqurybutton_clicked()
{
    message = "11#" + ui->inquryedit->text() + "#";
    ui->logedit->append(ui->inquryedit->text());
    ui->logedit->setAlignment(Qt::AlignRight);
    tcpSocket->write(message.toUtf8());
    ui->inquryedit->clear();
}


void Customer::on_homebutton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Customer::on_searchbutton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}


void Customer::on_orderrightbutton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}

