/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "afssapsintegrator.h"
//#include "atcmodel.h"
#include <drugsdb/drugsdbcore.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QColor>
#include <QIcon>
#include <QDesktopServices>
#include <QMenu>
#include <QUrl>
#include <QClipboard>
#include <QSortFilterProxyModel>
#include <QMimeData>

#include "ui_afssapslinkerwidget.h"
#include "ui_afssapstreewidget.h"
#include "ui_afssapsintegratorwidget.h"

#define MODEL_PREFETCH 100
const char * const MIMETYPE = "application/freemedforms.molecule.name";

using namespace DrugsDB;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::DrugsDBCore *dbCore() {return DrugsDB::DrugsDBCore::instance();}

static inline QString workingPath()        {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/AfssapsIntegratorSources/") + QDir::separator();}
//static inline QString iamDatabaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

static inline QString afssapsMolLinkFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_MOLECULE_LINK_FILENAME);}
static inline QString afssapsTreeXmlFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_CLASSTREE_FILENAME);}
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}

QWidget *AfssapsIntegratorPage::createPage(QWidget *parent)
{
    return new AfssapsIntegratorWidget(parent);
}

QWidget *AfssapsClassTreePage::createPage(QWidget *parent)
{
    return new AfssapsTreeWidget(parent);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////    MODEL    //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
namespace DrugsDB {

class DomItem
{
public:
    DomItem(QDomNode &node, int row, DomItem *parent = 0)
    {
        domNode = node;
        // Record the item's location within its parent.
        rowNumber = row;
        parentItem = parent;
    }

    ~DomItem()
    {
        QHash<int,DomItem*>::iterator it;
        for (it = childItems.begin(); it != childItems.end(); ++it)
            delete it.value();
    }

    QDomNode node() const
    {
        return domNode;
    }

    DomItem *parent()
    {
        return parentItem;
    }

    void refreshChildren()
    {
        childItems.clear();
    }

    DomItem *child(int i)
    {
        if (childItems.contains(i))
            return childItems[i];

        if (i >= 0 && i < domNode.childNodes().count()) {
            QDomNode childNode = domNode.childNodes().item(i);
            DomItem *childItem = new DomItem(childNode, i, this);
            childItems[i] = childItem;
            return childItem;
        }
        return 0;
    }

    int row()
    {
        return rowNumber;
    }

private:
    QDomNode domNode;
    QHash<int,DomItem*> childItems;
    DomItem *parentItem;
    int rowNumber;
};

class AfssapsLinkerModelPrivate
{
public:
    AfssapsLinkerModelPrivate(AfssapsLinkerModel *parent) :
            m_RootItem(0), m_FetchedRows(0), q(parent)
    {
        QFile file(afssapsMolLinkFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                Utils::Log::addError(q, QApplication::translate("AfssapsLinkerModel","Can not read XML file content %1").arg(file.fileName()), __FILE__, __LINE__);
                Utils::Log::addError(q, QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error), __FILE__, __LINE__);
            } else {
                Utils::Log::addMessage(q, QApplication::translate("AfssapsLinkerModel","Reading file: %1").arg(file.fileName()));
            }
            file.close();
            m_RootNode = domDocument.firstChildElement("AfssapsLinkerModel");
            m_RootItem = new DomItem(m_RootNode, 0);
        } else {
            Utils::Log::addError(q, QApplication::translate("AfssapsLinkerModel","Can not open XML file %1").arg(file.fileName()), __FILE__, __LINE__);
        }

    }

public:
    QDomDocument domDocument;
    QDomNode m_RootNode;
    DomItem *m_RootItem;
    QString reviewer, actualDbUid;
    int m_FetchedRows;

private:
    AfssapsLinkerModel *q;
};

}  // End namespace DrugsDB

AfssapsLinkerModel *AfssapsLinkerModel::m_Instance = 0;
AfssapsLinkerModel *AfssapsLinkerModel::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new AfssapsLinkerModel(parent);
    return m_Instance;
}

AfssapsLinkerModel::AfssapsLinkerModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    setObjectName("AfssapsLinkerModel");
    d = new AfssapsLinkerModelPrivate(this);
}

AfssapsLinkerModel::~AfssapsLinkerModel()
{
    if (d)
        delete d;
    d = 0;
}

bool AfssapsLinkerModel::saveModel()
{
    QFile file(afssapsMolLinkFile());
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write(d->domDocument.toString(2).toUtf8());
        file.close();
        return true;
    }
    return false;
}

void AfssapsLinkerModel::selectModel(const int type)
{
    QString tag;
    switch (type) {
    case Model_Labels: tag = "Labels"; break;
    case Model_Tree: tag = "Tree"; break;
    default: return;
    }
    QDomElement el = d->m_RootNode.firstChildElement(tag);
    if (!el.isNull()) {
        qWarning() << "FOUND";
        delete d->m_RootItem;
        d->m_RootItem = new DomItem(el, 0);
        d->m_FetchedRows = 0;
        reset();
    }
}

void AfssapsLinkerModel::setActualReviewer(const QString &name)
{
    d->reviewer = name;
}

bool AfssapsLinkerModel::canFetchMore(const QModelIndex &parent) const
{
    int nbItemRows = 0;
    DomItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (parentItem)
        nbItemRows = parentItem->node().childNodes().count();

//        qWarning() << Q_FUNC_INFO << (d->m_FetchedRows < nbItemRows) << parent << nbItemRows << d->m_FetchedRows;

    return (d->m_FetchedRows < nbItemRows);
}

void AfssapsLinkerModel::fetchMore(const QModelIndex &parent)
{
    int nbItemRows = 0;
    DomItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (parentItem)
        nbItemRows = parentItem->node().childNodes().count();

    int remainder = nbItemRows - d->m_FetchedRows;
    int itemsToFetch = qMin(MODEL_PREFETCH, remainder);

//    qWarning() << Q_FUNC_INFO << parent << d->m_FetchedRows << itemsToFetch;

    beginInsertRows(parent, d->m_FetchedRows, d->m_FetchedRows + itemsToFetch);
    d->m_FetchedRows += itemsToFetch;
    endInsertRows();
}

QModelIndex AfssapsLinkerModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (!parentItem)
        return QModelIndex();

    DomItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex AfssapsLinkerModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == d->m_RootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int AfssapsLinkerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    return d->m_FetchedRows;
}

int AfssapsLinkerModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant AfssapsLinkerModel::data(const QModelIndex &index, int role) const
{
    // <Labels>
    //   <Label fr="" category="" atcCodes="" en="" de="" es="" review="" reviewer="" references="" comments="" dateofreview="" autoFound="" />
    // </Labels>
    // <Tree>
    // </Tree>
    if (!index.isValid())
        return QVariant();

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case FancyButton:  // used for the sorting
            if (attributeMap.namedItem("review").nodeValue() == "true")
                return "a";
            return "b";
        case AfssapsName:
            return attributeMap.namedItem("fr").nodeValue();
        case AffapsCategory:
            return attributeMap.namedItem("category").nodeValue();
        case AtcCodes:
            return attributeMap.namedItem("atcCodes").nodeValue();
        case En_Label:
            return attributeMap.namedItem("en").nodeValue();
        case De_Label:
            return attributeMap.namedItem("de").nodeValue();
        case Es_Label:
            return attributeMap.namedItem("es").nodeValue();
        case Review:
            return attributeMap.namedItem("review").nodeValue();
        case Reviewer:
            return attributeMap.namedItem("reviewer").nodeValue();
        case References:
            return attributeMap.namedItem("references").nodeValue();
        case Comments:
            return attributeMap.namedItem("comment").nodeValue();
        case Date:
            return attributeMap.namedItem("dateofreview").nodeValue();
        case AutoFoundAtcs:
            return attributeMap.namedItem("autoFound").nodeValue();
        case WarnDuplicates:
            return attributeMap.namedItem("warnDuplication").nodeValue();
        default:
            return QVariant();
        }
    } else if (role == Qt::CheckStateRole) {
        if (index.column()==Review) {
            if (attributeMap.namedItem("review").nodeValue() == "true")
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    } else if (role == Qt::ToolTipRole) {
        if (index.column() == En_Label || index.column() == De_Label || index.column() == Es_Label) {
            // If (molecule or class) molecule --> Unable language editing
            DomItem *item = static_cast<DomItem*>(index.internalPointer());
            QDomNode node = item->node();
            QDomNamedNodeMap attributeMap = node.attributes();
            if (attributeMap.namedItem("category").nodeValue()!="class") {
                return tr("Language can only be set to CLASS, not to molecules");
            }
        }
        //        QString tmp;
        //        tmp += attributeMap.namedItem("name").nodeValue();
        //        QStringList codes = attributeMap.namedItem("AtcCode").nodeValue().split(",");
        //        tmp += "\n  " + codes.join("\n  ");
        //        tmp += "\n  " + dbCore()->atcModel()->getAtcLabel(codes).join("\n  ");
        //        return tmp;
        return QVariant();
    } else if (role==Qt::ForegroundRole) {
        if (attributeMap.namedItem("category").nodeValue()=="class") {
            return QColor("darkblue");
        } else if (!attributeMap.namedItem("autoFound").nodeValue().isEmpty()) {
            return QColor("lightgray");
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column()==FancyButton) {
            if (attributeMap.namedItem("review").nodeValue() == "true")
                return theme()->icon(Core::Constants::ICONOK);
            else
                return theme()->icon(Core::Constants::ICONHELP);
        }
    }
    return QVariant();
}

bool AfssapsLinkerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role==Qt::EditRole || role==Qt::DisplayRole) {
        switch (index.column()) {
        case AtcCodes:
            attributeMap.namedItem("atcCodes").setNodeValue(value.toString());
            break;
        case En_Label:
            attributeMap.namedItem("en").setNodeValue(value.toString());
            break;
        case De_Label:
            attributeMap.namedItem("de").setNodeValue(value.toString());
            break;
        case Es_Label:
            attributeMap.namedItem("es").setNodeValue(value.toString());
            break;
        case References:
            attributeMap.namedItem("references").setNodeValue(value.toString());
            break;
        case Comments:
            attributeMap.namedItem("comment").setNodeValue(value.toString());
            break;
        case Review:
            attributeMap.namedItem("review").setNodeValue(value.toString());
            break;
        case AutoFoundAtcs:
            attributeMap.namedItem("autoFound").setNodeValue(value.toString());
            break;
        case WarnDuplicates:
            attributeMap.namedItem("warnDuplication").setNodeValue(value.toString());
            break;
        default:
            return false;
        }
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
        return true;
    } else if (role==Qt::CheckStateRole && index.column()==Review) {
        if (value.toInt() == Qt::Checked) {
            attributeMap.namedItem("review").setNodeValue("true");
        } else {
            attributeMap.namedItem("review").setNodeValue("false");
        }
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
        return true;
    }

    return false;
}

Qt::ItemFlags AfssapsLinkerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column()==AfssapsName)
        f |= Qt::ItemIsDragEnabled  | Qt::ItemIsDropEnabled;

    if (index.column() == AfssapsName || index.column() == AffapsCategory || index.column() == Date || index.column() == FancyButton)
        return f;

    if (index.column() == Review) {
        f |= Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
        return f;
    }

    if (index.column()==AtcCodes) {
        f |= Qt::ItemIsEditable;
        return f;
    }

    if (index.column() == En_Label || index.column() == De_Label || index.column() == Es_Label) {
        // If (molecule or class) molecule --> Unable language editing
        DomItem *item = static_cast<DomItem*>(index.internalPointer());
        QDomNode node = item->node();
        QDomNamedNodeMap attributeMap = node.attributes();
        if (attributeMap.namedItem("category").nodeValue()=="class") {
            f |= Qt::ItemIsEditable;
        }
    }

    return f;
}

QVariant AfssapsLinkerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case AfssapsName:
            return tr("Fr Label");
        case AffapsCategory:
            return tr("Category");
        case AtcCodes:
            return tr("ATC codes");
        case En_Label:
            return tr("En Label");
        case De_Label:
            return tr("De Label");
        case Es_Label:
            return tr("Es Label");
        case Review:
            return tr("Review state");
        case Reviewer:
            return tr("Reviewer");
        case References:
            return tr("References");
        case Comments:
            return tr("Comments");
        case Date:
            return tr("Date of review");
        case AutoFoundAtcs:
            return tr("AutoFound");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

bool AfssapsLinkerModel::addUnreviewedMolecules(const QString &dbUid, const QStringList &molecules)
{
    Q_UNUSED(dbUid);
    QDomElement el = d->m_RootNode.firstChildElement("Labels");
    if (el.isNull())
        return false;

    QStringList known;
    for(int i=0; i < rowCount(); ++i) {
        known << index(i, AfssapsName).data().toString();
    }
    known.removeDuplicates();
    known.removeAll("");

    foreach(const QString &mol, molecules) {
        if (mol.isEmpty())
            continue;
        if (known.contains(mol, Qt::CaseInsensitive))
            continue;
        QDomElement newmol = d->domDocument.createElement("Label");
        newmol.setAttribute("fr", mol);
        newmol.setAttribute("category", "molecule");
        newmol.setAttribute("review", "false");
        newmol.setAttribute("reviewer", QString());
        newmol.setAttribute("references", QString());
        newmol.setAttribute("comment", QString());
        newmol.setAttribute("dateofreview", QString());
        el.appendChild(newmol);
    }

    reset();
    return true;
}

bool AfssapsLinkerModel::addAutoFoundMolecules(const QMultiHash<QString, QString> &mol_atc, bool removeFromModel)
{
    Q_UNUSED(removeFromModel);
    int nb = 0;
    foreach(const QString &mol, mol_atc.keys()) {
        QDomNode n = d->m_RootItem->node().firstChild();

        while (!n.isNull()) {
            QDomNamedNodeMap attributeMap = n.attributes();
            if (attributeMap.namedItem("affsaps").nodeValue() == mol) {
                //                 if (removeFromModel) {
                //                     const QStringList &list = mol_atc.values(mol);
                //                     n.toElement().setAttribute("autofound", list.join(","));
                //                     QDomNode rem = d->m_RootItem->node().removeChild(n);
                //                 } else {
                const QStringList &list = mol_atc.values(mol);
                n.toElement().setAttribute("autofound", list.join(","));
                //                 }
                ++nb;
                break;
            }
            n = n.nextSibling();
        }
    }
    return true;
}

QStringList AfssapsLinkerModel::mimeTypes() const
{
    QStringList types;
    types << MIMETYPE;
    return types;
}

QMimeData *AfssapsLinkerModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            if (index.column()==AfssapsName) {
                QString text = data(index, Qt::DisplayRole).toString();
//                text += ":";
//                text += index(index.row(), AfssapsLinkerModel::Review).data().toString();
                stream << text;
            }
        }
    }

    mimeData->setData(MIMETYPE, encodedData);
    return mimeData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////    TREEMODEL    ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
namespace DrugsDB {

class AfssapsClassTreeModelPrivate
{
public:
    AfssapsClassTreeModelPrivate(AfssapsClassTreeModel *parent) :
        m_RootItem(0),
        m_FetchedRows(0),
        q(parent)
    {
        QFile file(afssapsTreeXmlFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                Utils::Log::addError(q, QApplication::translate("AfssapsClassTreeModelPrivate","Can not read XML file content %1").arg(file.fileName()), __FILE__, __LINE__);
                Utils::Log::addError(q, QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error), __FILE__, __LINE__);
            } else {
                Utils::Log::addMessage(q, QApplication::translate("AfssapsClassTreeModelPrivate","Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            Utils::Log::addError(q, QApplication::translate("AfssapsClassTreeModelPrivate","Can not open XML file %1").arg(file.fileName()), __FILE__, __LINE__);
        }

        m_RootNode = domDocument.firstChildElement("AfssapsTree");

        m_RootItem = new DomItem(m_RootNode, 0);
    }

public:
    QDomDocument domDocument;
    QDomNode m_RootNode;
    DomItem *m_RootItem;
    QString reviewer, actualDbUid;
    int m_FetchedRows;
    QMultiHash<QString, QString> m_BuggyIncludes; // K= class; V= INNs

private:
    AfssapsClassTreeModel *q;
};

}  // End namespace DrugsDB


AfssapsClassTreeModel *AfssapsClassTreeModel::m_Instance = 0;
AfssapsClassTreeModel *AfssapsClassTreeModel::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new AfssapsClassTreeModel(parent);
    return m_Instance;
}

AfssapsClassTreeModel::AfssapsClassTreeModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    setObjectName("AfssapsClassTreeModel");
    d = new AfssapsClassTreeModelPrivate(this);
}

AfssapsClassTreeModel::~AfssapsClassTreeModel()
{
    if (d)
        delete d;
    d = 0;
}

bool AfssapsClassTreeModel::saveModel()
{
    QFile file(afssapsTreeXmlFile());
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write(d->domDocument.toString(2).toUtf8());
        file.close();
        return true;
    }
    return false;
}

void AfssapsClassTreeModel::setActualReviewer(const QString &name)
{
    d->reviewer = name;
}

QModelIndex AfssapsClassTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    DomItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex AfssapsClassTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == d->m_RootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int AfssapsClassTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomItem *item = 0;

    if (!parent.isValid())
        item = d->m_RootItem;
    else
        item = static_cast<DomItem*>(parent.internalPointer());

    return item->node().childNodes().count();
}

int AfssapsClassTreeModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant AfssapsClassTreeModel::data(const QModelIndex &index, int role) const
{
    // <Class name="" review="" reviewer="" dateofreview="">
    //   <Molecule name="">
    //    <Source link="" />
    //  </Molecule>
    // </Class>
    if (!index.isValid())
        return QVariant();

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case Name:
            if (node.toElement().tagName()=="Source")
                return attributeMap.namedItem("link").nodeValue();
            return attributeMap.namedItem("name").nodeValue();
        case Review:
            return attributeMap.namedItem("review").nodeValue();
        case Reviewer:
            return attributeMap.namedItem("reviewer").nodeValue();
        case Date:
            return attributeMap.namedItem("dateofreview").nodeValue();
        default:
            return QVariant();
        }
    } else if (role == Qt::CheckStateRole) {
        if (index.column()==Review) {
            if (attributeMap.namedItem("review").nodeValue() == "true")
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    } else if (role == Qt::ToolTipRole) {
        QString tmp;
        //        tmp += attributeMap.namedItem("name").nodeValue();
        //        QStringList codes = attributeMap.namedItem("AtcCode").nodeValue().split(",");
        //        tmp += "\n  " + codes.join("\n  ");
        //        tmp += "\n  " + AtcModel::instance()->getAtcLabel(codes).join("\n  ");
        return tmp;
    } else if (role==Qt::ForegroundRole) {
        if (!attributeMap.namedItem("autoFound").nodeValue().isEmpty()) {
            return QColor("lightgray");
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column()==Name && node.toElement().tagName()=="Class") {
            if (attributeMap.namedItem("review").nodeValue() == "true")
                return QIcon(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/pixmap/16x16/ok.png");
            else
                return QIcon(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/pixmap/16x16/help.png");
        }
    } else if (role==Qt::BackgroundRole) {
        if (node.toElement().tagName()=="Class") {
            if (d->m_BuggyIncludes.uniqueKeys().contains(node.attributes().namedItem("name").nodeValue())) {
                QColor c("red");
                c.setAlpha(125);
                return c;
            }
        }
        if (node.toElement().tagName()=="Molecule") {
            const QString &cl = node.parentNode().attributes().namedItem("name").nodeValue();
            if (d->m_BuggyIncludes.values(cl).contains(attributeMap.namedItem("name").nodeValue())) {
                QColor c("red");
                c.setAlpha(125);
                return c;
            }
        }
    }
    return QVariant();
}

bool AfssapsClassTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role==Qt::EditRole) {
        switch (index.column()) {
        case Name:
            if (node.toElement().tagName()=="Class")
                return false;
            attributeMap.namedItem("name").setNodeValue(value.toString());
            break;
        case Review:
            attributeMap.namedItem("review").setNodeValue(value.toString());
            break;
        default:
            return false;
        }
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
        return true;
    } else if (role==Qt::CheckStateRole && index.column()==Review) {
        if (value.toInt() == Qt::Checked) {
            attributeMap.namedItem("review").setNodeValue("true");
        } else {
            attributeMap.namedItem("review").setNodeValue("false");
        }
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
        return true;
    }

    return false;
}

Qt::ItemFlags AfssapsClassTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();

    if (node.isComment()) {
        return f;
    }

    if (node.toElement().tagName() == "Class")
        f |= Qt::ItemIsDropEnabled;

    f |= Qt::ItemIsEditable;

    if (index.column() == Name || index.column() == Date)
        return f;

    if (index.column() == Review)
        f |= Qt::ItemIsUserCheckable;

    return f;
}

QVariant AfssapsClassTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Name");
        case Review:
            return tr("Review state");
        case Reviewer:
            return tr("Reviewer");
        case Date:
            return tr("Date of review");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QStringList AfssapsClassTreeModel::mimeTypes() const
{
    QStringList types;
    types << MIMETYPE;
    return types;
}

Qt::DropActions AfssapsClassTreeModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

bool AfssapsClassTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat(MIMETYPE))
        return false;

    if (column > 0)
        return false;

    int beginRow;
    if (row != -1)
        beginRow = row;
    else
        beginRow = 0;

    QByteArray encodedData = data->data(MIMETYPE);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    int rows = 0;

    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        newItems << text;
        ++rows;
    }

    insertRows(beginRow, rows, parent);

    foreach (QString text, newItems) {
        QModelIndex idx = index(beginRow, Name, parent);
        setData(idx, text);
        beginRow++;
    }

    return true;
}

bool AfssapsClassTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    DomItem *item = static_cast<DomItem*>(parent.internalPointer());
    if (!item)
        item = d->m_RootItem;
    QDomNode node = item->node();
    if (node.toElement().tagName()!="Class") {
        qWarning() << "WRONG INSERT --> != CLASS";
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    QDomNode afterMe = node.childNodes().at(row);
    for(int i = 0; i < count; ++i) {
        QDomElement insertMe = d->domDocument.createElement("Molecule");
        insertMe.setAttribute("name", "");
        if (i == 0)
            afterMe = node.insertBefore(insertMe, afterMe);
        else
            afterMe = node.insertAfter(insertMe, afterMe);
    }

    item->refreshChildren();
    endInsertRows();

    return true;
}

void AfssapsClassTreeModel::addBuggyInclusions(const QMultiHash<QString, QString> &buggyIncludes)
{
    d->m_BuggyIncludes = buggyIncludes;
    reset();
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////    INTEGRATOR WIDGET    //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
AfssapsIntegratorWidget::AfssapsIntegratorWidget(QWidget *parent) :
        QWidget(parent), ui(new Ui::AfssapsIntegratorWidget)
{
    ui->setupUi(this);
}



/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////    LINK WIDGET    ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
AfssapsLinkerWidget::AfssapsLinkerWidget(QWidget *parent) :
        QWidget(parent), ui(new Ui::AfssapsLinkerWidget)
{
    ui->setupUi(this);
    AfssapsLinkerModel *model = AfssapsLinkerModel::instance(this);
    model->selectModel(AfssapsLinkerModel::Model_Labels);
    //    readCSVFile();
    //    recreateTreeFromSQL();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(AfssapsLinkerModel::AfssapsName);

    if (model->rowCount() || model->canFetchMore()) {
        ui->tableView->setModel(proxyModel);
        ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
        ui->tableView->setColumnWidth(0, 24);
        ui->tableView->verticalHeader()->hide();
        ui->tableView->setSortingEnabled(true);
        ui->tableView->setDragEnabled(true);
    }

    connect(ui->reviewer, SIGNAL(activated(QString)), model, SLOT(setActualReviewer(QString)));
    connect(ui->tableView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed(QModelIndex)));

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterWildcard(QString)));
}

void AfssapsLinkerWidget::pressed(const QModelIndex &index)
{
    if (index.column()==AfssapsLinkerModel::FancyButton) {
        QMenu *menu = new QMenu(this);
        QAction *google = new QAction(tr("Search Google (copy molecule to clipboard)"), menu);
        QAction *who = new QAction(tr("Search WHO (copy molecule to clipboard)"), menu);
        QAction *resip = new QAction(tr("Search RESIP (copy molecule to clipboard)"), menu);
        QAction *copyClip = new QAction(tr("Copy molecule name to clipboard"), menu);
        menu->addAction(google);
        menu->addAction(who);
        menu->addAction(resip);
        menu->addAction(copyClip);
        QAction *selected = menu->exec(QCursor::pos());

        QAbstractItemModel *model = proxyModel;
        const QString &name = model->index(index.row(), AfssapsLinkerModel::AfssapsName).data().toString();

        if (selected == google) {
            QDesktopServices::openUrl(QUrl(QString("http://www.google.fr/search?rls=en&q=%1+atc&ie=UTF-8&oe=UTF-8&redir_esc=").arg(name)));
        } else if (selected == who) {
            QDesktopServices::openUrl(QUrl(QString("http://www.whocc.no/atc_ddd_index/?name=%1").arg(name)));
        } else if (selected == resip) {
            QApplication::clipboard()->setText(name);
            QDesktopServices::openUrl(QUrl("http://www.portailmedicaments.resip.fr/bcb_recherche/classes.asp?cc=1"));
        } else if (selected == copyClip) {
            QApplication::clipboard()->setText(name);
        }
    }
}

void AfssapsLinkerWidget::on_saveFile_clicked()
{
    AfssapsLinkerModel *model = AfssapsLinkerModel::instance(this);
    model->saveModel();
}

void AfssapsLinkerWidget::on_findAtc_clicked()
{
//    if (!DrugsDB::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath()))
//        return;

//    Utils::Log::addMessage("Tools", "Getting ATC Information from the interactions database");

//    QHash<QString, int> atc_id;
//    QMultiHash<QString, int> atcName_id;
//    QString req = "SELECT `ID`, `CODE`, `FRENCH` FROM `ATC` WHERE length(CODE)=7;";
//    QSqlQuery query(req, QSqlDatabase::database(Core::Constants::IAM_DATABASE_NAME));
//    if (query.isActive()) {
//        while (query.next()) {
//            atc_id.insert(query.value(1).toString(), query.value(0).toInt());
//            atcName_id.insertMulti(query.value(2).toString().toUpper(), query.value(0).toInt());
//        }
//    }
//    query.finish();

//    // get All afssaps mols
//    AfssapsLinkerModel *model = AfssapsLinkerModel::instance(this);
//    int row = 0;

//    // populate the entire model
//    while (model->canFetchMore(QModelIndex()))
//        model->fetchMore(QModelIndex());

//    // Find link between Afssaps Molecules and ATCs
//    while (model->hasIndex(row, AfssapsLinkerModel::AfssapsName)) {
//        QString toFind = model->index(row, AfssapsLinkerModel::AfssapsName).data().toString();
//        toFind = DrugsDB::Tools::noAccent(toFind);
//        int atcId = atcName_id.value(toFind);
//        if (atcId == 0) {
//            ++row;
//            continue;
//        }
//        const QString &atcLabel = atcName_id.key(atcId);
//        const QList<int> &atcIds = atcName_id.values(atcLabel);
//        QStringList atcCodes;
//        foreach(int id, atcIds) {
//            atcCodes.append(atc_id.key(id));
//        }
//        // check already available ATC codes from the model
//        QStringList modelAtcCodes = model->index(row, AfssapsLinkerModel::AtcCodes).data().toString().split(",");
//        modelAtcCodes.removeAll("");
//        if (modelAtcCodes.count() > 0) {
//            // add autoFounded to the list
//            modelAtcCodes.append(atcCodes);
//            modelAtcCodes.removeDuplicates();
//            qWarning() << atcCodes << modelAtcCodes;
//            qSort(modelAtcCodes);
//            model->setData(model->index(row, AfssapsLinkerModel::AtcCodes), modelAtcCodes.join(","));
//            model->setData(model->index(row, AfssapsLinkerModel::Review), QString("true"));
//            ++row;
//            continue;
//        }
//        qSort(atcCodes);
//        model->setData(model->index(row, AfssapsLinkerModel::References), "AFSSAPS");
//        model->setData(model->index(row, AfssapsLinkerModel::Review), QString("true"));
//        model->setData(model->index(row, AfssapsLinkerModel::AtcCodes), atcCodes.join(","));
//        model->setData(model->index(row, AfssapsLinkerModel::AutoFoundAtcs), "true");
//        ++row;
//    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////    TREE WIDGET    ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
AfssapsTreeWidget::AfssapsTreeWidget(QWidget *parent) :
        QWidget(parent), ui(new Ui::AfssapsTreeWidget)
{
    ui->setupUi(this);
    AfssapsClassTreeModel *model = AfssapsClassTreeModel::instance(this);

    ui->treeView->setModel(model);
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->viewport()->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
    ui->treeView->setDragDropMode(QAbstractItemView::DropOnly);

    connect(ui->comboBox, SIGNAL(activated(QString)), model, SLOT(setActualReviewer(QString)));
    connect(ui->treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed(QModelIndex)));
    connect(ui->save, SIGNAL(clicked()), model, SLOT(saveModel()));
}
