#ifndef TOOLS_H
#define TOOLS_H	

#include "constants.h"

#include <QHash>
#include <QString>

namespace Tools
{
class ReceiptsTools : public QObject{
    public:
        ReceiptsTools();
        ~ReceiptsTools();
        QHash<int,QString> hashOfItems();
        QString getStringFromRows(int item);
        int getEnumItemFromRow(const QString itemText);

    };
};



#endif

