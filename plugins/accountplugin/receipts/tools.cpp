#include "tools.h"

using namespace ReceiptsConstants;
using namespace Tools;

ReceiptsTools::ReceiptsTools(){}
ReceiptsTools::~ReceiptsTools(){}
QHash<int,QString> ReceiptsTools::hashOfItems()
{
        QHash<int,QString> treeviewRows;
        treeviewRows.insert(THESAURUS_ITEM,QObject::tr("Thesaurus of preestablished billing"));
        treeviewRows.insert(ALL_VALUES_ITEM,QObject::tr("All predefined Values"));
        treeviewRows.insert(PREFERED_VALUE_ITEM,QObject::tr("My Preferred Value"));
        treeviewRows.insert(ROUND_TRIP_ITEM,QObject::tr("Round trip"));
        treeviewRows.insert(FREE_VALUE_ITEM,QObject::tr("Free value"));
        treeviewRows.insert(DEBTOR_ITEM,QObject::tr("Debtor"));
        treeviewRows.insert(SITES_ITEM,QObject::tr("Sites"));
        treeviewRows.insert(DISTANCE_RULES_ITEM,QObject::tr("Distance rules"));
        treeviewRows.insert(BANK_ITEM,QObject::tr("Bank"));
        treeviewRows.insert(RULES_ITEM,QObject::tr("Rules"));
        return treeviewRows;
    }

QString ReceiptsTools::getStringFromRows(int item)
{
        QString displayedItem;
        QHash<int,QString> treeviewRows;
        treeviewRows = hashOfItems();
        displayedItem = treeviewRows.value(item);
        return displayedItem;
    }

int ReceiptsTools::getEnumItemFromRow(const QString itemText)
{
        int enumItem = -1;
        QHash<int,QString> treeviewRows;
        treeviewRows = hashOfItems();
        enumItem = treeviewRows.key(itemText);
        return enumItem;
    }


