/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "atcmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QProgressDialog>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/ZARawSources/") + QDir::separator();}
static inline QString atcCsvFile()      {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + QString(Core::Constants::ATC_FILENAME));}

AtcModel *AtcModel::_instance = 0;

namespace DrugsDB {
namespace Internal {
class AtcItem
{
public:
    AtcItem(AtcItem *parent = 0) :
            m_Parent(parent)
    {
    }
    ~AtcItem() { qDeleteAll(m_Children); }

    // Genealogy management
    AtcItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    AtcItem *parent() { return m_Parent; }
    void setParent(AtcItem *parent) { m_Parent = parent; }
    bool addChildren(AtcItem *child)
    {
        if (!m_Children.contains(child))
            m_Children.append(child);
        return true;
    }
    bool insertChild(const int row, AtcItem *child)
    {
        if (row > m_Children.count())
            return false;
        m_Children.insert(row, child);
        return true;
    }
    int childNumber() const
    {
        if (m_Parent)
            return m_Parent->m_Children.indexOf(const_cast<AtcItem *>(this));
        return 0;
    }
    void sortChildren()
    {
        qSort(m_Children.begin(), m_Children.end(), AtcItem::lessThan);
    }

    void setData(const int id, const QVariant &value) {m_Datas.insert(id, value.toString());}

    // Access to data
    QString data(const int id) const {return m_Datas.value(id);}
    QString code() const {return m_Datas.value(AtcModel::ATC_Code);}
    QString english() const {return m_Datas.value(AtcModel::ATC_EnglishLabel);}
    QString french() const {return m_Datas.value(AtcModel::ATC_FrenchLabel);}

    // For sort functions
    static bool lessThan(AtcItem *item1, AtcItem *item2)
    {
        return item1->code() < item2->code();
    }

private:
    AtcItem *m_Parent;
    QList<AtcItem*> m_Children;
    QHash<int, QString> m_Datas;
};

class AtcModelPrivate
{
public:
    AtcModelPrivate(AtcModel *parent) :
            m_Root(0), q(parent)
    {
        m_Root = new AtcItem;
        m_Root->setData(0, "ROOT");
    }

    ~AtcModelPrivate()
    {}

    void getTree()
    {
        qWarning() << "AtcModel::getTree()";
        QFile file(atcCsvFile());
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            Utils::Log::addError("ATCModel", tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file.fileName()),__FILE__, __LINE__);
            return;
        }
        QString content = QString::fromUtf8(file.readAll());
        foreach(const QString &line, content.split("\n")) {
            const QStringList &vals = line.split("\";\"");
            if (vals.count()!=4) {
                qWarning() << vals;
                continue;
            }
            QString tmp;
            AtcItem *item = new AtcItem();
            tmp = vals.at(0).toUpper();
            tmp.remove("\"");
            item->setData(AtcModel::ATC_Code, tmp);
            QString en = vals.at(1).toUpper();
            en.remove("\"");
            item->setData(AtcModel::ATC_EnglishLabel, en);
            tmp = vals.at(2).toUpper();
            tmp.remove("\"");
            if (tmp.isEmpty())
                item->setData(AtcModel::ATC_FrenchLabel, en);
            else
                item->setData(AtcModel::ATC_FrenchLabel, tmp);
            tmp = vals.at(3);
            tmp = tmp.remove("\"").toUpper();
            if (tmp.isEmpty())
                item->setData(AtcModel::ATC_DeutschLabel, en);
            else
                item->setData(AtcModel::ATC_DeutschLabel, tmp);
            m_ItemsList.append(item);
        }

        qWarning() << "get" << m_ItemsList.count() << "ATC";
        qSort(m_ItemsList.begin(), m_ItemsList.end(), AtcItem::lessThan);

        QList<AtcItem *> three, four, five, six;
        AtcItem *last = 0;
        AtcItem *lastOne = 0;
        AtcItem *lastThree = 0;
        AtcItem *lastFour = 0;
        AtcItem *lastFive = 0;
        AtcItem *parent = 0;
        foreach(AtcItem *item, m_ItemsList) {
            switch (item->code().count())
            {
            case 1: parent = m_Root;     lastOne = item; break;
            case 3: parent = lastOne;    lastThree = item; break;
            case 4: parent = lastThree;  lastFour = item;  break;
            case 5: parent = lastFour;   lastFive = item;  break;
            case 7: parent = lastFive;    break;
            }
            // need to be reparented
            if (!parent) {
                qWarning() << item->code() << "no parent";
            } else {
                item->setParent(parent);
                parent->addChildren(item);
            }
            last = item;
            parent = 0;
        }
        q->reset();
    }

    AtcItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            AtcItem *item = static_cast<AtcItem *>(index.internalPointer());
            if (item)
                return item;
        }
        return m_Root;
    }

public:
    AtcItem *m_Root;
    QList<AtcItem *> m_ItemsList;

private:
    AtcModel *q;
};

}  // namespace Internal
}  // namespace DrugsDB

AtcModel::AtcModel(QObject * parent) :
        QAbstractItemModel(parent), d(new AtcModelPrivate(this))
{
    _instance = this;
}

AtcModel::~AtcModel()
{}

/** Initialize ATC Model. Read the ATC CSV file in the global_resources */
bool AtcModel::initialize()
{
    d->getTree();
    return true;
}

QModelIndex AtcModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

    AtcItem *parentItem = d->getItem(parent);
    AtcItem *childItem = 0;
    childItem = parentItem->child(row);

    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex AtcModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AtcItem *childItem = d->getItem(index);
    AtcItem *parentItem = childItem->parent();

    if (parentItem == d->m_Root)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int AtcModel::rowCount(const QModelIndex &parent) const
{
    AtcItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int AtcModel::columnCount(const QModelIndex &) const
{
    return NumberOfColumn;
}

QVariant AtcModel::data(const QModelIndex & item, int role) const
{
    if (!item.isValid())
        return QVariant();

    const AtcItem *it = d->getItem(item);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
    {
        if (item.column() == ATC_CodeAndLabel) {
            return it->data(ATC_Code) + " - " + it->data(ATC_EnglishLabel);
        }
        return it->data(item.column());
    }
    case Qt::ToolTipRole :
    {
        return it->english();
    }
    case Qt::ForegroundRole :
    {
        //            if (it->isTemplate()) {
        //                return QColor(settings()->value(Core::Constants::S_FOREGROUND_TEMPLATES, "#000").toString());
        //            } else {
        //                return QColor(settings()->value(Core::Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
        //            }
        break;
    }
    case Qt::BackgroundRole :
    {
        //            QColor c;
        //            if (it->isTemplate()) {
        //                c = QColor(settings()->value(Core::Constants::S_BACKGROUND_TEMPLATES, "white").toString());
        //            } else {
        //                c = QColor(settings()->value(Core::Constants::S_BACKGROUND_CATEGORIES, "white").toString());
        //            }
        //            if (!Utils::isReleaseCompilation()) {
        //                if (it->isNewlyCreated()) {
        //                    c = QColor(Qt::blue);
        //                } else
        //                    if (it->isModified()) {
        //                    c = QColor(Qt::red);
        //                }
        //            }
        //            if (c.name()=="#ffffff")
        //                return QVariant();
        //            c.setAlpha(125);
        //            return c;
        break;
    }
    }
    return QVariant();
}

Qt::ItemFlags AtcModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QStringList AtcModel::getAtcLabel(const QStringList &codes)
{
    QStringList list;
    bool fr = QLocale().name().left(2).toLower()=="fr";
    foreach(const QString &code, codes) {
        foreach(AtcItem *item, d->m_ItemsList) {
            if (item->code() == code) {
                if (fr)
                    list << item->french();
                else
                    list << item->english();
                break;
            }
        }
    }
    return list;
}

bool AtcModel::insertAtcCodeToDatabase(const QString &connectionName)
{
    // Start transaction
    QSqlDatabase iam = QSqlDatabase::database(connectionName);
    if (!iam.isOpen())
        return false;

    iam.transaction();

    // Clean ATC table from old values
    QString req;
    req = "DELETE FROM `ATC`"
          "WHERE ID>=0";
    DrugsDB::Tools::executeSqlQuery(req, connectionName, __FILE__, __LINE__);

    // Import ATC codes to database
    QFile file(atcCsvFile());
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
        Utils::Log::addError("Tools", QApplication::translate("Tools", "ERROR : can not open file %1, %2").arg(file.fileName(), file.errorString()), __FILE__, __LINE__);
    } else {
        QString content = file.readAll();
        int i = 0;
        const QStringList &list = content.split("\n", QString::SkipEmptyParts);
        QProgressDialog progress("Adding ATC Codes to database", "Abort", 0, list.count(), qApp->activeWindow());
        progress.setWindowModality(Qt::WindowModal);
        foreach(const QString &s, list) {
            req = QString("INSERT INTO `ATC`  (`ID`, `CODE`, `ENGLISH`, `FRENCH`, `DEUTSCH`) "
                          "VALUES (%1, %2) ").arg(i).arg(s);
            DrugsDB::Tools::executeSqlQuery(req, connectionName, __FILE__, __LINE__);
            ++i;
            if (i%50 == 0)
                progress.setValue(i);
        }
    }

    // Commit transaction
    iam.commit();

    return true;
}
