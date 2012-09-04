#include "tools.h"
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <QDir>
#include <QDebug>

using namespace ReceiptsConstants;
using namespace Tools;

enum { WarnDebugMessage = true };

ReceiptsTools::ReceiptsTools(){}
ReceiptsTools::~ReceiptsTools(){}
QHash<int,QString> ReceiptsTools::hashOfItems()
{
        QHash<int,QString> treeviewRows;
        treeviewRows.insert(THESAURUS_ITEM,QObject::tr("Your favorite lists"));
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
    
QHash<QString,QString> ReceiptsTools::flagsHash()
{
        QHash<QString,QString> hash;     
        //hash.insert("FR",Core::ICore::instance()->theme()->iconFullPath("flags/FR.png",Core::ITheme::SmallIcon));
        QString flagsDirPath;
        QStringList listPath = Core::ICore::instance()->theme()->iconFullPath("flags/FR.png",Core::ITheme::SmallIcon)
                       .split(QDir::separator());
        listPath.removeLast();
        flagsDirPath = listPath.join(QDir::separator());
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " iconfullpath =" << flagsDirPath;        
        QDir dirFlags(flagsDirPath);
        QStringList listOfFlags;
        listOfFlags = dirFlags.entryList();
        foreach(QString flag,listOfFlags){
            QStringList listOfStr;
            listOfStr = flag.split(".");
            QString country;
            QString type;
            if (listOfStr.size()>1)
            {
            	  country = listOfStr[0];
            	  type = listOfStr[1];
                }
            hash.insert(country,dirFlags.absolutePath()+QDir::separator()+country+"."+type);
            }                
        return hash;
    }


