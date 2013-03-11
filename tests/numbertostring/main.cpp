#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QStringList>

#include "../../libs/utils/numbertostring.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qWarning();

    QList<int> list;
    list << 5 << 14 << 19 << 39 << 149 << 2942;
    for(int i = 0 ; i < list.count(); ++i)
        qWarning() << list.at(i) << Utils::integerToHumanReadableString(list.at(i));

    QList<double> dlist;
    dlist << 5.8 << 24.7183 << 69.12 << 1732874.871 << 149.67 << 2192.263;
    for(int i = 0 ; i < dlist.count(); ++i)
        qWarning() << QString::number(dlist.at(i), 'f', 6) << Utils::doubleToHumanReadableString(dlist.at(i));

    return 0;
    //return a.exec();
}
