#include "customer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Customer w;
    w.show();
    return a.exec();
}
