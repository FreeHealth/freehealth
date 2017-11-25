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
#include "icdgrouplinker.h"
#include "ui_icdgrouplinker.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <icdplugin/icdcollectiondialog.h>
#include <icdplugin/icdcollectionmodel.h>
#include <icdplugin/icddatabase.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDesktopServices>
#include <QMenu>
#include <QUrl>
#include <QClipboard>

using namespace Icd10;

//#define MODEL_PREFETCH 100

static int modelPrefetch = 100;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

static inline QString workingPath()        {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/IcgGroupLinkerSources/") + QDir::separator();}
//static inline QString iamDatabaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

static inline QString icdGroupLinkerFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::ICD_GROUP_LINKER_FILENAME);}

QWidget *IcdGroupLinkerPage::createPage(QWidget *parent)
{
    return new IcdGroupLinkerWidget(parent);
}


IcdGroupLinkerWidget::IcdGroupLinkerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IcdGroupLinkerWidget)
{
    ui->setupUi(this);
    IcdGroupLinkerModel *model = IcdGroupLinkerModel::instance(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(IcdGroupLinkerModel::GroupId);

    if (model->rowCount() || model->canFetchMore()) {
        ui->tableView->setModel(proxyModel);
        ui->tableView->horizontalHeader()->setResizeMode(IcdGroupLinkerModel::FancyButton, QHeaderView::Fixed);
        ui->tableView->horizontalHeader()->setResizeMode(IcdGroupLinkerModel::GroupId, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setResizeMode(IcdGroupLinkerModel::GroupNameFr, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setResizeMode(IcdGroupLinkerModel::GroupNameEn, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setResizeMode(IcdGroupLinkerModel::GroupNameDe, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setResizeMode(IcdGroupLinkerModel::GroupNameEs, QHeaderView::Stretch);
        ui->tableView->setColumnWidth(0, 24);
        ui->tableView->verticalHeader()->hide();
        ui->tableView->setSortingEnabled(true);
        ui->tableView->setDragEnabled(true);
    }

//    connect(ui->reviewer, SIGNAL(activated(QString)), model, SLOT(setActualReviewer(QString)));
    connect(ui->tableView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed(QModelIndex)));

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterWildcard(QString)));
}

IcdGroupLinkerWidget::~IcdGroupLinkerWidget()
{
    delete ui;
}

void IcdGroupLinkerWidget::pressed(const QModelIndex &index)
{
    if (index.column()==IcdGroupLinkerModel::FancyButton) {
        QMenu *menu = new QMenu(this);
        QAction *google = new QAction(tr("Search Google (copy molecule to clipboard)"), menu);
        QAction *who = new QAction(tr("Search WHO (copy molecule to clipboard)"), menu);
        QAction *copyClip = new QAction(tr("Copy molecule name to clipboard"), menu);
        menu->addAction(google);
        menu->addAction(who);
        menu->addAction(copyClip);
        QAction *selected = menu->exec(QCursor::pos());

        QAbstractItemModel *model = proxyModel;
        const QString &name = model->index(index.row(), IcdGroupLinkerModel::GroupId).data().toString();

        if (selected == google) {
            QApplication::clipboard()->setText(name);
            QDesktopServices::openUrl(QUrl(QString("http://www.google.fr/search?rls=en&q=%1+icd10&ie=UTF-8&oe=UTF-8&redir_esc=").arg(name)));
        } else if (selected == who) {
            QApplication::clipboard()->setText(name);
            QDesktopServices::openUrl(QUrl(QString("http://apps.who.int/classifications/apps/icd/icd10online/")));
        } else if (selected == copyClip) {
            QApplication::clipboard()->setText(name);
        }
    } else if (index.column()==IcdGroupLinkerModel::IcdCodes) {
        ICD::IcdCollectionDialog dlg(this);
        dlg.setWindowTitle("Searching group: " + proxyModel->index(index.row(), IcdGroupLinkerModel::GroupId).data().toString());
        ICD::IcdCollectionModel *model = new ICD::IcdCollectionModel(this);
        model->setCollectionIsSimpleList(true);
        dlg.setIcdCollectionModel(model);
        foreach(const QString &code, index.data().toString().split(",", QString::SkipEmptyParts)) {
            model->addCode(code);
        }
        if (dlg.exec()==QDialog::Accepted) {
            proxyModel->setData(index, model->includedCodesWithoutDaget().join(","));
        }
    }
}

void IcdGroupLinkerWidget::on_saveFile_clicked()
{
    IcdGroupLinkerModel *model = IcdGroupLinkerModel::instance(this);
    model->saveModel();
}

void IcdGroupLinkerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


namespace {

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

}  // End anonymous namespace

namespace Icd10 {
class IcdGroupLinkerModelPrivate
{
public:
    IcdGroupLinkerModelPrivate(IcdGroupLinkerModel *parent) :
            m_RootItem(0), m_FetchedRows(0), q(parent)
    {
        QFile file(icdGroupLinkerFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                LOG_ERROR_FOR(q, QApplication::translate("IcdGroupLinkerModel","Can not read XML file content %1").arg(file.fileName()));
                LOG_ERROR_FOR(q, QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
            } else {
                LOG_FOR(q, QApplication::translate("IcdGroupLinkerModel","Reading file: %1").arg(file.fileName()));
            }
            file.close();
            m_RootNode = domDocument.firstChildElement("IcdLinks");
            m_RootItem = new DomItem(m_RootNode, 0);
        } else {
            LOG_ERROR_FOR(q, QApplication::translate("IcdGroupLinkerModel","Can not open XML file %1").arg(file.fileName()));
        }

    }

public:
    QDomDocument domDocument;
    QDomNode m_RootNode;
    DomItem *m_RootItem;
    QString reviewer, actualDbUid;
    int m_FetchedRows;

private:
    IcdGroupLinkerModel *q;
};
} // namespace Icd10

IcdGroupLinkerModel *IcdGroupLinkerModel::m_Instance = 0;
IcdGroupLinkerModel *IcdGroupLinkerModel::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new IcdGroupLinkerModel(parent);
    return m_Instance;
}

IcdGroupLinkerModel::IcdGroupLinkerModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    setObjectName("IcdGroupLinkerModel");
    d = new IcdGroupLinkerModelPrivate(this);
}

IcdGroupLinkerModel::~IcdGroupLinkerModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Return true is the ICD database is initialized and accessible */
bool IcdGroupLinkerModel::databaseAvailable()
{
    return icdBase()->isInitialized();
}

bool IcdGroupLinkerModel::saveModel()
{
    QFile file(icdGroupLinkerFile());
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write(d->domDocument.toString(2).toUtf8());
        file.close();
        return true;
    }
    return false;
}

bool IcdGroupLinkerModel::canFetchMore(const QModelIndex &parent) const
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

void IcdGroupLinkerModel::fetchMore(const QModelIndex &parent)
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
    int itemsToFetch = qMin(modelPrefetch, remainder);

//    qWarning() << Q_FUNC_INFO << parent << d->m_FetchedRows << itemsToFetch;

    beginInsertRows(parent, d->m_FetchedRows, d->m_FetchedRows + itemsToFetch);
    d->m_FetchedRows += itemsToFetch;
    endInsertRows();
}

QModelIndex IcdGroupLinkerModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex IcdGroupLinkerModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == d->m_RootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int IcdGroupLinkerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    return d->m_FetchedRows;
}

int IcdGroupLinkerModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant IcdGroupLinkerModel::data(const QModelIndex &index, int role) const
{
    // <IcdLinks>
    //     <IcdGroup id="" fr="" en="" de="" es="" icdCodes="" references="" comments="" review="" reviewer="" dateofreview=""/>
    // </IcdLinks>
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
        case GroupId:
            return attributeMap.namedItem("id").nodeValue();
        case GroupNameFr:
            return attributeMap.namedItem("fr").nodeValue();
        case GroupNameEn:
            return attributeMap.namedItem("en").nodeValue();
        case GroupNameDe:
            return attributeMap.namedItem("de").nodeValue();
        case GroupNameEs:
            return attributeMap.namedItem("es").nodeValue();
        case IcdCodes:
            return attributeMap.namedItem("icdCodes").nodeValue();
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
//        if (index.column() == En_Label || index.column() == De_Label || index.column() == Es_Label) {
//            // If (molecule or class) molecule --> Unable language editing
//            DomItem *item = static_cast<DomItem*>(index.internalPointer());
//            QDomNode node = item->node();
//            QDomNamedNodeMap attributeMap = node.attributes();
//            if (attributeMap.namedItem("category").nodeValue()!="class") {
//                return tr("Language can only be set to CLASS, not to molecules");
//            }
//        }
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

bool IcdGroupLinkerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role==Qt::EditRole || role==Qt::DisplayRole) {
        switch (index.column()) {
        case GroupId:
            attributeMap.namedItem("id").setNodeValue(value.toString());
            break;
        case GroupNameFr:
            attributeMap.namedItem("fr").setNodeValue(value.toString());
            break;
        case GroupNameEn:
            attributeMap.namedItem("en").setNodeValue(value.toString());
            break;
        case GroupNameDe:
            attributeMap.namedItem("de").setNodeValue(value.toString());
            break;
        case GroupNameEs:
            attributeMap.namedItem("es").setNodeValue(value.toString());
            break;
        case IcdCodes:
            attributeMap.namedItem("icdCodes").setNodeValue(value.toString());
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

Qt::ItemFlags IcdGroupLinkerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == Review) {
        return f | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;;
    }

    if (index.column()==IcdCodes) {
        return f | Qt::ItemIsEditable;
    }

//    if (index.column() == En_Label || index.column() == De_Label || index.column() == Es_Label) {
//        // If (molecule or class) molecule --> Unable language editing
//        DomItem *item = static_cast<DomItem*>(index.internalPointer());
//        QDomNode node = item->node();
//        QDomNamedNodeMap attributeMap = node.attributes();
//        if (attributeMap.namedItem("category").nodeValue()=="class") {
//            f |= Qt::ItemIsEditable;
//        }
//    }

    return f;
}

QVariant IcdGroupLinkerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case GroupId:
            return tr("Id");
        case GroupNameFr:
            return tr("Fr");
        case GroupNameEn:
            return tr("En");
        case GroupNameDe:
            return tr("De");
        case GroupNameEs:
            return tr("Es");
        case IcdCodes:
            return tr("ICD Codes");
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
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QList<QVariant> IcdGroupLinkerModel::getSidsForRow(int row)
{
    while (canFetchMore())
        fetchMore();
    QList<QVariant> toReturn;
    // get atcCodes
    QModelIndex idx = index(row, IcdCodes);
    if (!idx.isValid())
        return toReturn;
    foreach(const QString &code, idx.data().toString().split(",", QString::SkipEmptyParts)) {
        toReturn << icdBase()->getSid(code);
    }
    return toReturn;
}

QList<QVariant> IcdGroupLinkerModel::getSidsForGroup(const QString &icdGroup)
{
    while (canFetchMore())
        fetchMore();
    for(int i = 0; i < rowCount(); ++i) {
        if (index(i, GroupId).data().toString().compare(icdGroup, Qt::CaseInsensitive)==0) {
            return getSidsForRow(i);
        }
    }
    return QList<QVariant>();
}

