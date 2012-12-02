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
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QProgressDialog>
#include <QStandardItem>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/ZARawSources/") + QDir::separator();}
static inline QString atcCsvFile()      {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + QString(Core::Constants::ATC_FILENAME));}

AtcModel *AtcModel::_instance = 0;
namespace {
const int ENGLISH = Qt::UserRole + 1;
const int FRENCH = ENGLISH + 1;
const int SPANISH = FRENCH + 1;
const int CODE = SPANISH + 1;
}  // anymous namespace

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
        q(parent)
    {
        bold.setBold(true);
    }

    ~AtcModelPrivate()
    {}

    bool getTree()
    {
        // Get ATC CSV file content
        QString content = Utils::readTextFile(atcCsvFile());
        if (content.isEmpty())
            return false;

        // Read line by line
        QStringList lines = content.split("\n");
        lines.sort();
        QString code, tmp, en;
        QList<QStandardItem*> items;
        QStandardItem *lastOne = 0;
        QStandardItem *lastThree = 0;
        QStandardItem *lastFour = 0;
        QStandardItem *lastFive = 0;
        QStandardItem *parent = 0;
        QList<QStandardItem*> cols;
        foreach(const QString &line, lines) {
            cols.clear();
            const QStringList &vals = line.split("\";\"");
            if (vals.count() == 0)
                continue;

            if (vals.count() != 4) {
                if (!vals.at(0).startsWith("--"))
                    LOG_ERROR_FOR(q, "ATC CSV file error at line: " + vals.at(0));
                continue;
            }

            tmp = vals.at(0).toUpper();
            tmp.remove("\"");
            code = tmp;
            QStandardItem *item = new QStandardItem(tmp);
            cols << item;
            QString en = vals.at(1).toUpper();
            en.remove("\"");
            cols <<  new QStandardItem(en);
            tmp = vals.at(2).toUpper();
            tmp.remove("\"");
            if (tmp.isEmpty())
                tmp = en;
            cols <<  new QStandardItem(tmp);
            tmp = vals.at(3);
            tmp = tmp.remove("\"").toUpper();
            if (tmp.isEmpty())
                tmp = en;
            cols << new QStandardItem(tmp);
            cols << new QStandardItem(code + " - " + en);

            // Find the parent item
            switch (code.count())
            {
            case 1: parent = 0; lastOne = item; break;
            case 3: parent = lastOne;    lastThree = item; break;
            case 4: parent = lastThree;  lastFour = item;  break;
            case 5: parent = lastFour;   lastFive = item;  break;
            case 7: parent = lastFive;    break;
            }
            // need to be reparented
            if (!parent) {
                qWarning() << code << en << "no parent";
                parent = q->invisibleRootItem();
            }
            parent->appendRow(cols);
            parent = 0;
        }

//        q->reset();
        return true;
    }

public:
    QFont bold;

private:
    AtcModel *q;
};

}  // namespace Internal
}  // namespace DrugsDB

AtcModel::AtcModel(QObject * parent) :
    QStandardItemModel(parent),
    d(new AtcModelPrivate(this))
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

QVariant AtcModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role==Qt::EditRole) {
        return QStandardItemModel::data(index, role);
    } else if (role == Qt::ToolTipRole) {
        QModelIndex code = this->index(index.row(), ATC_Code, index.parent());
        QModelIndex english = this->index(index.row(), ATC_EnglishLabel, index.parent());
        QModelIndex french = this->index(index.row(), ATC_EnglishLabel, index.parent());
        QModelIndex deutsch = this->index(index.row(), ATC_EnglishLabel, index.parent());
        return QString("<b>ATC: %1</b> <br />"
                       "* en: %2 <br />"
                       "* fr: %3 <br />"
                       "* de: %4")
                .arg(code.data().toString())
                .arg(english.data().toString())
                .arg(french.data().toString())
                .arg(deutsch.data().toString());
    } else if (role==Qt::FontRole) {
        if (index.parent() == QModelIndex())
            return d->bold;
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
//    bool fr = QLocale().name().left(2).toLower()=="fr";
//    foreach(const QString &code, codes) {
//        foreach(AtcItem *item, d->m_ItemsList) {
//            if (item->code() == code) {
//                if (fr)
//                    list << item->french();
//                else
//                    list << item->english();
//                break;
//            }
//        }
//    }
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
