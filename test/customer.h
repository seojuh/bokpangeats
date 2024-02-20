#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QMainWindow>
#include <QMovie>
#include <QPixmap>
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QtNetwork>
#include <QString>
#include <QList>
#include <QMessageBox>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class Customer;
}
QT_END_NAMESPACE

class Customer : public QMainWindow
{
    Q_OBJECT

public:
    Customer(QWidget *parent = nullptr);
    ~Customer();

    void initClnt();
    QTcpSocket *tcpSocket;

private slots:
    void on_chickenbutton_clicked();
    void on_pizzabutton_clicked();
    void on_dongasbutton_clicked();
    void on_bunsikbutton_clicked();
    void slot_read();
    void connectedToServer();
    void connectionError(QAbstractSocket::SocketError socketError);

    void on_back_clicked();

    void on_storepicture_clicked();

    void on_storepicture_2_clicked();

    void on_storepicture_3_clicked();

    void on_back_2_clicked();
    void processData(const QString &data);

    void on_menupicture_clicked();

    void on_menupicture_2_clicked();

    void on_menupicture_3_clicked();

    void on_menupicture_4_clicked();

    void on_menupicture_5_clicked();

    void on_plusbutton_clicked();

    void on_minusbutton_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_6_clicked();

    void on_addmenu_clicked();
    void updateTotalPrice();
    void changeQuantity(int delta);
    void addOptionToMenuName(const QString& optionName);
    void updateOptionDisplay();

    void on_orderbutton_clicked();

    void on_addorder_clicked();
    void updateOrderAndTotalAmounts();
    void resetOrderForm();
    void acticeRadiobutton();
    void clearOrderInfo();
    void passwordValid();

    void on_orderrightbutton_clicked();
    void on_homebutton_2_clicked();

    void on_searchbutton_clicked();

    void on_searchbutton_2_clicked();

    void on_searchpageburtton_clicked();

    void on_inquirybutton_clicked();

    void on_inquirybutton_2_clicked();

    void on_phoneright_clicked();
    void applyDiscount(int orderCount);
    void SearchResult(const QString &results);
    void on_orderlist_clicked(const QModelIndex &index);

    void on_phoneright_2_clicked();

    void on_inqurybutton_clicked();

    void on_homebutton_3_clicked();

    void on_searchbutton_3_clicked();

    void on_orderrightbutton_3_clicked();

private:
    Ui::Customer *ui;
    QHostAddress getMyIP();
    QString message;
    QMap<QString, double>optionPrices;
    QString originalMenuName;
    QString currentOption1;
    QString currentOption2;
    bool isAddingOrder;
    QList<QString> list;

};
#endif // CUSTOMER_H
