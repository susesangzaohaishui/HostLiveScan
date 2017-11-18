#include <QCoreApplication>
#include "host_scan.h"
#include "random.hpp"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    host_init("host_list.xml",NULL,NULL);
    return a.exec();
}

