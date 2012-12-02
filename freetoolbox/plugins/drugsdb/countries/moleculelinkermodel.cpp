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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class MoleculeLinkerModel
  Based on an XML formatted file to ease the SVN commit process.
  XML File format is:
  \code
 <FreeToolBox>
  <MoleculeLinker database="">
    <Molecule name="NAME" AtcCode="" review="" reviewer="" reference="" comment=""/>
  </MoleculeLinker>
 </FreeToolBox>
 \endcode
*/

#include "moleculelinkermodel.h"
#include "atcmodel.h"
#include "moleculelinkdata.h"
#include <drugsdb/drugsdbcore.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>
#include <drugsdb/tools.h>

#include <utils/log.h>

#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QColor>
#include <QIcon>

#define MODEL_PREFETCH 100

using namespace DrugsDB;
using namespace Internal;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::DrugsDBCore *dbCore() {return DrugsDB::DrugsDBCore::instance();}

static inline QString workingPath()         {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/MolLinker/") + QDir::separator();}
static inline QString linkerXmlFile()       {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::MOL_LINK_FILENAME);}

namespace DrugsDB {
namespace Internal {
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

class MoleculeLinkerModelPrivate
{
public:
    MoleculeLinkerModelPrivate(MoleculeLinkerModel *parent) :
            m_RootItem(0), m_FetchedRows(0), q(parent)
    {
        QFile file(linkerXmlFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                Utils::Log::addError(q, q->tr("Can not read XML file content %1").arg(file.fileName()), __FILE__, __LINE__);
                Utils::Log::addError(q, QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error), __FILE__, __LINE__);
            } else {
                Utils::Log::addMessage(q, q->tr("Reading Molecule to ATC linker XML file: %1").arg(file.fileName()));
            }
            file.close();

            m_RootNode = domDocument.firstChildElement("MoleculeLinkerModel");
            m_RootItem = new DomItem(m_RootNode, 0);

        } else {
            Utils::Log::addError(q, q->tr("Can not open XML file %1").arg(file.fileName()), __FILE__, __LINE__);
        }

    }

public:
    QDomDocument domDocument;
    QDomNode m_RootNode;
    DomItem *m_RootItem;
    QString reviewer, actualDbUid;
    int m_FetchedRows;

private:
    MoleculeLinkerModel *q;
};

}  // namespace Internal
}  // namespace DrugsDB

MoleculeLinkerModel::MoleculeLinkerModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::MoleculeLinkerModelPrivate(this))
{
    setObjectName("MoleculeLinkerModel");
}

MoleculeLinkerModel::~MoleculeLinkerModel()
{
    if (d)
        delete d;
    d = 0;
}

bool MoleculeLinkerModel::initialize()
{
    return true;
}

bool MoleculeLinkerModel::saveModel()
{
    QFile file(linkerXmlFile());
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write(d->domDocument.toString(2).toUtf8());
        file.close();
        return true;
    }
    return false;
}

QStringList MoleculeLinkerModel::availableDrugsDatabases() const
{
    QStringList list;
    QDomElement el = d->m_RootNode.firstChildElement("Database");
    while (!el.isNull()) {
        if (!el.attribute("uid").isEmpty()) {
            list << el.attribute("uid");
        }
        el = el.nextSiblingElement("Database");
    }
    return list;
}

bool MoleculeLinkerModel::selectDatabase(const QString &dbUid)
{
    if (dbUid == d->actualDbUid)
        return true;
    qWarning() << "MoleculeLinkerModel::selectDatabase" << dbUid;
    QDomElement el = d->m_RootNode.firstChildElement("Database");
    while (!el.isNull()) {
        if (el.attribute("uid") == dbUid) {
            qWarning() << "FOUND";
            delete d->m_RootItem;
            d->m_RootItem = new DomItem(el, 0);
            d->actualDbUid = dbUid;
            d->m_FetchedRows = 0;
            reset();
            return true;
        }
        el = el.nextSiblingElement("Database");
    }
    return false;
}

void MoleculeLinkerModel::setActualReviewer(const QString &name)
{
    d->reviewer = name;
}

bool MoleculeLinkerModel::canFetchMore(const QModelIndex &parent) const
{
    int nbItemRows = 0;
    DomItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (parentItem)
        nbItemRows = parentItem->node().childNodes().count();

//    qWarning() << "canFetchMore" << (d->m_FetchedRows < nbItemRows) << parent << nbItemRows << d->m_FetchedRows;

    return (d->m_FetchedRows < nbItemRows);
}

void MoleculeLinkerModel::fetchMore(const QModelIndex &parent)
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

//    qWarning() << "MoleculeLinkerModel::fetchMore" << parent << d->m_FetchedRows << itemsToFetch;

    beginInsertRows(parent, d->m_FetchedRows, d->m_FetchedRows + itemsToFetch);
    d->m_FetchedRows += itemsToFetch;
    endInsertRows();
}

QModelIndex MoleculeLinkerModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex MoleculeLinkerModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == d->m_RootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int MoleculeLinkerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    return d->m_FetchedRows;
}

int MoleculeLinkerModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant MoleculeLinkerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case MoleculeName:
            return attributeMap.namedItem("name").nodeValue();
        case ATC_Code:
            return attributeMap.namedItem("AtcCode").nodeValue();
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
            return attributeMap.namedItem("autofound").nodeValue();
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
        tmp += attributeMap.namedItem("name").nodeValue();
        QStringList codes = attributeMap.namedItem("AtcCode").nodeValue().split(",");
        tmp += "\n  " + codes.join("\n  ");
        tmp += "\n  " + dbCore()->atcModel()->getAtcLabel(codes).join("\n  ");
        return tmp;
    } else if (role==Qt::ForegroundRole) {
        if (!attributeMap.namedItem("autofound").nodeValue().isEmpty()) {
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

bool MoleculeLinkerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role==Qt::EditRole && index.column()!=Review) {
        switch (index.column()) {
        case ATC_Code:
            attributeMap.namedItem("AtcCode").setNodeValue(value.toString());
            if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
                attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
            break;
        case References:
            attributeMap.namedItem("references").setNodeValue(value.toString());
            if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
                attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
            break;
        case Comments:
            attributeMap.namedItem("comment").setNodeValue(value.toString());
            if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
                attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
            break;
        default:
            return false;
        }
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount()));
        return true;
    } else if (role==Qt::CheckStateRole && index.column()==Review) {
        if (value.toInt() == Qt::Checked) {
            attributeMap.namedItem("review").setNodeValue("true");
            if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
                attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        } else {
            attributeMap.namedItem("review").setNodeValue("false");
            if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
                attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        }
        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount()));
        return true;
    }

    return false;
}

Qt::ItemFlags MoleculeLinkerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == MoleculeName || index.column() == Date || index.column() == FancyButton)
        return f;

    if (index.column() == Review)
        f |= Qt::ItemIsUserCheckable;

    f |= Qt::ItemIsEditable;

    return f;
}

QVariant MoleculeLinkerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case MoleculeName:
            return tr("Molecule name");
        case ATC_Code:
            return tr("ATC code");
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

bool MoleculeLinkerModel::addUnreviewedMolecules(const QString &dbUid, const QStringList &molecules)
{
    QDomElement el = d->m_RootNode.firstChildElement("Database");
    while (!el.isNull()) {
        if (el.attribute("uid").compare(dbUid, Qt::CaseInsensitive) == 0) {
            break;
        }
        el = el.nextSiblingElement("Database");
    }

    if (el.isNull())
        return false;

    selectDatabase(dbUid);

    QStringList known;
    for(int i=0; i < rowCount(); ++i) {
        known << index(i, MoleculeName).data().toString();
    }
    known.removeDuplicates();
    known.removeAll("");

    foreach(const QString &mol, molecules) {
        if (mol.isEmpty())
            continue;
        if (known.contains(mol, Qt::CaseInsensitive))
            continue;
        QDomElement newmol = d->domDocument.createElement("Molecule");
        newmol.setAttribute("name", mol);
        newmol.setAttribute("AtcCode", QString());
        newmol.setAttribute("review", "false");
        newmol.setAttribute("reviewer", QString());
        newmol.setAttribute("references", QString());
        newmol.setAttribute("comment", QString());
        newmol.setAttribute("dateofreview", QString());
        el.appendChild(newmol);
        known << mol;
    }

    reset();
    return true;
}

bool MoleculeLinkerModel::addAutoFoundMolecules(const QMultiHash<QString, QString> &mol_atc, bool removeFromModel)
{
    int nb = 0;
    foreach(const QString &mol, mol_atc.keys()) {
        QDomNode n = d->m_RootItem->node().firstChild();

         while (!n.isNull()) {
             QDomNamedNodeMap attributeMap = n.attributes();
             if (attributeMap.namedItem("name").nodeValue() == mol) {
                 if (removeFromModel) {
                     const QStringList &list = mol_atc.values(mol);
                     n.toElement().setAttribute("autofound", list.join(","));
                     QDomNode rem = d->m_RootItem->node().removeChild(n);
                 } else {
                     const QStringList &list = mol_atc.values(mol);
                     n.toElement().setAttribute("autofound", list.join(","));
                 }
                 ++nb;
                 break;
             }
             n = n.nextSibling();
         }
    }
    return true;
}


struct MolLink {
    int lk_nature;
    int mol;
    QString mol_form;
};

bool MoleculeLinkerModel::moleculeLinker(Internal::MoleculeLinkData *data)
{
    Q_ASSERT(data);
    if (!data)
        return false;
    // get all ATC ids
    QSqlDatabase iam = data->database->database();
    if (!iam.open()) {
        LOG_ERROR("Can not connect to MASTER db");
        return false;
    }
    data->moleculeIdToAtcId.clear();
    data->unfoundMoleculeAssociations.clear();
    QHash<QString, int> atc_id;
    QMultiHash<QString, int> atcName_id;
    QString req;
    QSqlQuery query(iam);
    using namespace DrugsDB::Constants;

    // Get all ATC Code and Label
    LOG("Getting ATC Informations from the interactions database");
    Utils::FieldList get;
    get << Utils::Field(Table_ATC, ATC_ID);
    get << Utils::Field(Table_ATC, ATC_CODE);
    get << Utils::Field(Table_LABELS, LABELS_LABEL);
    Utils::JoinList joins;
    joins << Utils::Join(Table_ATC_LABELS, ATC_LABELS_ATCID, Table_ATC, ATC_ID)
          << Utils::Join(Table_LABELSLINK, LABELSLINK_MASTERLID, Table_ATC_LABELS, ATC_LABELS_MASTERLID)
          << Utils::Join(Table_LABELS, LABELS_LID, Table_LABELSLINK, LABELSLINK_LID);
    Utils::FieldList cond;
    cond << Utils::Field(Table_LABELS, LABELS_LANG, QString("='%1'").arg(data->lang));

    if (query.exec(data->database->select(get,joins,cond))) {
        while (query.next()) {
            atc_id.insert(query.value(1).toString(), query.value(0).toInt());
            atcName_id.insertMulti(query.value(2).toString().toUpper(), query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();
    qWarning() << "ATC" << atc_id.count();

    // Get source ID (SID)
    int sid = data->sourceId;
    if (sid==-1) {
        LOG_ERROR("NO SID: " + data->drugDbUid);
        return false;
    }

    // Get all MOLS.MID and Label
    LOG("Getting Drugs Composition from " + data->drugDbUid);
    QMultiHash<QString, int> mols;
    QHash<int, QString> w;
    w.insert(MOLS_SID, QString("=%1").arg(sid));
    req = data->database->selectDistinct(Table_MOLS, QList<int>()
                                   << MOLS_MID
                                   << MOLS_NAME, w);
    if (query.exec(req)) {
        while (query.next()) {
            mols.insertMulti(query.value(1).toString(), query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();
    qWarning() << "Number of distinct molecules" << mols.uniqueKeys().count();
    const QStringList &knownMoleculeNames = mols.uniqueKeys();

    // manage corrected molecules
    foreach(const QString &mol, data->correctedByName.keys()) {
        if (!knownMoleculeNames.contains(mol))
            continue;
        foreach(int id, atcName_id.values(data->correctedByName.value(mol)))
            data->moleculeIdToAtcId.insertMulti(mols.value(mol), id);
    }
    foreach(const QString &mol, data->correctedByAtcCode.uniqueKeys()) {  // Key=mol, Val=ATC
        if (!knownMoleculeNames.contains(mol))
            continue;
        // For all ATC codes corresponding to the molecule name
        foreach(const QString &atc, data->correctedByAtcCode.values(mol)) {
            // Get the ATC label and retreive all ATC_ids that have the same label --> NO
            //                QString atcLabel = atcName_id.key(atc_id.value(atc));
            //                foreach(int id, atcName_id.values(atcLabel))
            //                    data->moleculeIdToAtcId.insertMulti(mols.value(mol), id);
            // Associate molecule to the ATC_Id corresponding to the code
            foreach(const int molId, mols.values(mol)) {
                data->moleculeIdToAtcId.insertMulti(molId, atc_id.value(atc));
            }
            qWarning() << "Corrected by ATC" << mol << atc << atcName_id.key(atc_id.value(atc));
        }
    }
    LOG("Hand made association: " + QString::number(data->moleculeIdToAtcId.count()));

    // find links
    data->unfoundMoleculeAssociations.clear();
    foreach(const QString &mol, knownMoleculeNames) {
        if (mol.isEmpty())
            continue;
        foreach(const int codeMol, mols.values(mol)) {
            if (data->moleculeIdToAtcId.keys().contains(codeMol)) {
                continue;
            }
            // Does molecule name exact-matches an ATC label
            QString molName = mol.toUpper();
            QList<int> atcIds = atcName_id.values(molName);
            if (atcIds.count()==0) {
                // No --> Try to find the ATC label by transforming the molecule name
                // remove accents
                molName = molName.replace(QString::fromUtf8("É"), "E");
                molName = molName.replace(QString::fromUtf8("È"), "E");
                molName = molName.replace(QString::fromUtf8("À"), "A");
                molName = molName.replace(QString::fromUtf8("Ï"), "I");
                atcIds = atcName_id.values(molName);
                if (atcIds.count()>0) {
                    qWarning() << "Without accent >>>>>>>>" << molName;
                }
                // Not found try some transformations
                // remove '(*)'
                if ((atcIds.count()==0) && (molName.contains("("))) {
                    molName = molName.left(molName.indexOf("(")).simplified();
                    atcIds = atcName_id.values(molName);
                    if (atcIds.count()>0) {
                        qWarning() << "Without (*) >>>>>>>>" << molName;
                    }
                }
                if (atcIds.count()==0) {
                    // remove last word : CITRATE, DIHYDRATE, SODIUM, HYDROCHLORIDE, POLISTIREX, BROMIDE, MONOHYDRATE, CHLORIDE, CARBAMATE
                    //  INDANYL SODIUM, ULTRAMICROCRYSTALLINE, TROMETHAMINE
                    molName = molName.left(molName.lastIndexOf(" ")).simplified();
                    atcIds = atcName_id.values(molName);
                    if (atcIds.count()>0) {
                        qWarning() << "Without last word >>>>>>>>" << molName;
                    }
                }
                if (atcIds.count()==0) {
                    // remove first words : CHLORHYDRATE DE, ACETATE DE
                    QStringList toRemove;
                    toRemove << "CHLORHYDRATE DE" << "CHLORHYDRATE D'" << "ACETATE DE" << "ACETATE D'";
                    foreach(const QString &prefix, toRemove) {
                        if (molName.startsWith(prefix)) {
                            QString tmp = molName;
                            tmp.remove(prefix);
                            tmp = tmp.simplified();
                            atcIds = atcName_id.values(tmp);
                            if (atcIds.count()) {
                                molName = tmp;
                                qWarning() << "Without prefix"<< prefix << ">>>>>>>>" << tmp << atcIds;
                                break;
                            }
                        }
                    }
                }
                if (atcIds.count()==0) {
                    // Manage (DINITRATE D')
                    if (molName.contains("(DINITRATE D')")) {
                        QString tmp = molName;
                        tmp = tmp.remove("(DINITRATE D')");
                        tmp += "DINITRATE";
                        atcIds = atcName_id.values(tmp);
                        if (atcIds.count()) {
                            molName = tmp;
                            qWarning() << "With DINITRATE"<< molName << ">>>>>>>>" << tmp << atcIds;
                            break;
                        }
                    }
                }
            }
            bool found = false;
            foreach(int id, atcIds) {
                found = true;
                data->moleculeIdToAtcId.insertMulti(codeMol, id);
                qWarning() << "Linked" << mol << atcName_id.key(id);
            }
            if (!found) {
                if (!data->unfoundMoleculeAssociations.contains(mol))
                    data->unfoundMoleculeAssociations.append(mol);
            }
        }
    }

    // Inform model of founded links
    QMultiHash<QString, QString> mol_atc_forModel;
    QHashIterator<int,int> it(data->moleculeIdToAtcId);
    while (it.hasNext()) {
        it.next();
        mol_atc_forModel.insertMulti(mols.key(it.key()), atc_id.key(it.value()));
    }
    addAutoFoundMolecules(mol_atc_forModel);
    mol_atc_forModel.clear();

    // Try to find molecules in the MoleculeLinkerModel
    QHash<QString, QString> model_mol_atc;
    int modelFound = 0;
    int reviewedWithoutAtcLink = 0;
    selectDatabase(data->drugDbUid);
    while (canFetchMore(QModelIndex()))
        fetchMore(QModelIndex());

    for(int i=0; i< rowCount(); ++i) {
        if (index(i, MoleculeLinkerModel::Review).data().toString() == "true") {
            model_mol_atc.insert(index(i, MoleculeLinkerModel::MoleculeName).data().toString(),
                                 index(i, MoleculeLinkerModel::ATC_Code).data().toString());
        }
    }
    qWarning() << "MoleculeLinkerModel::AvailableLinks" << model_mol_atc.count();

    foreach(const QString &mol, data->unfoundMoleculeAssociations) {
        if (model_mol_atc.keys().contains(mol)) {
            int codeMol = mols.value(mol);
            if (!data->moleculeIdToAtcId.keys().contains(codeMol)) {
                if (model_mol_atc.value(mol).trimmed().isEmpty()) {
                    ++reviewedWithoutAtcLink;
                    continue;
                }
                QStringList atcCodes = model_mol_atc.value(mol).split(",");
                foreach(const QString &atcCode, atcCodes) {
                    QString atcName = atcName_id.key(atc_id.value(atcCode));
                    QList<int> atcIds = atcName_id.values(atcName);
                    foreach(int id, atcIds) {
                        data->moleculeIdToAtcId.insertMulti(codeMol, id);
                        qWarning() << "ModelLinker Found" << codeMol << mol << id << atcName_id.key(id);
                        data->unfoundMoleculeAssociations.removeAll(mol);
                    }
                    if (atcIds.count())
                        ++modelFound;
                }
            }
        }
    }

    // TODO: Try to find new associations via the COMPOSITION.LK_NATURE field
    int natureLinkerNb = 0;
//    if (drugsDbUid == "FR_AFSSAPS") {
//        // TODO: code here */
//        QMap<int, QMultiHash<int, int> > cis_codeMol_lk;
//        QMap<int, QVector<MolLink> > cis_compo;
//        {
//            //            QString req = "SELECT `DID`, `MID`, `LK_NATURE` FROM `COMPOSITION` ORDER BY `DID`";
//            QString req = database->select(Table_COMPO, QList<int>()
//                                                             << COMPO_DID
//                                                             << COMPO_MID
//                                                             << COMPO_LK_NATURE
//                                                             );
//            req += QString(" ORDER BY `%1`").arg(database->fieldName(Table_COMPO, COMPO_DID));
//            if (query.exec(req)) {
//                while (query.next()) {
//                    QVector<MolLink> &receiver = cis_compo[query.value(0).toInt()];
//                    MolLink lk;
//                    lk.mol = query.value(1).toInt();
//                    lk.lk_nature = query.value(2).toInt();
//                    //                    lk.mol_form = query.value(3).toString();
//                    receiver.append(lk);
//                }
//            } else {
//                LOG_QUERY_ERROR(query);
//            }
//            QMutableMapIterator<int, QVector<MolLink> > i(cis_compo);
//            while (i.hasNext()) {
//                i.next();
//                const QVector<MolLink> &links = i.value();
//                QMultiHash<int, int> lk_mol;
//                QMultiHash<int, QString> lk_form;
//                foreach(const MolLink &lk, links) {
//                    lk_mol.insertMulti(lk.lk_nature, lk.mol);
//                    lk_form.insertMulti(lk.lk_nature, lk.mol_form);
//                }
//                foreach(int key, lk_mol.uniqueKeys()) {
//                    QStringList forms = lk_form.values(key);
//                    const QList<int> &mols = lk_mol.values(key);
//                    forms.removeDuplicates();
//                    if (forms.count()==1 && mols.count()==2) {
//                        // link molecules
//                        QMultiHash<int, int> &code_lk = cis_codeMol_lk[i.key()];
//                        code_lk.insertMulti(key, mols.at(0));
//                        code_lk.insertMulti(key, mols.at(1));
//                    }
//                }
//            }
//        }
//        // Computation
//        int nb;
//        do
//        {
//            nb=0;
//            QMutableMapIterator<int, QMultiHash<int, int> > i (cis_codeMol_lk);
//            while (i.hasNext()) {
//                i.next();
//                const QMultiHash<int, int> lk_codemol = i.value();
//                // for all molecule_codes
//                foreach(int lk, lk_codemol.uniqueKeys()) {
//                    if (lk_codemol.count(lk) == 2) {
//                        // Ok for computation
//                        int one, two;
//                        one = lk_codemol.values(lk).at(0);
//                        two = lk_codemol.values(lk).at(1);

//                        // if both molecule_codes are known or unknown --> continue
//                        if ((!data->moleculeIdToAtcId.keys().contains(one)) &&
//                                (!data->moleculeIdToAtcId.keys().contains(two)))
//                            continue;
//                        if ((data->moleculeIdToAtcId.keys().contains(one)) &&
//                                (data->moleculeIdToAtcId.keys().contains(two)))
//                            continue;

//                        // Associate unknown molecule_code with the known ATC
//                        if (data->moleculeIdToAtcId.keys().contains(one)) {
//                            // The second molecule is unknown
//                            const QList<int> &atcIds = data->moleculeIdToAtcId.values(one);
//                            foreach(int actId, atcIds) {
//                                data->moleculeIdToAtcId.insertMulti(two, actId);
//                                qWarning() << "LK_NATURE: Linked" << i.key() << mols.key(one) << mols.key(two) << lk << atcName_id.key(actId);
//                                data->unfoundMoleculeAssociations.removeAll(mols.key(two));
//                                data->moleculeIdToAtcId_forModel.insertMulti(mols.key(one), atcName_id.key(actId));
//                            }
//                        } else if (data->moleculeIdToAtcId.keys().contains(two)) {
//                            // The first is unknown
//                            const QList<int> &atcIds = data->moleculeIdToAtcId.values(two);
//                            foreach(int actId, atcIds) {
//                                data->moleculeIdToAtcId.insertMulti(one, actId);
//                                qWarning() << "LK_NATURE: Linked" << i.key() << mols.key(one) << mols.key(two) << lk << atcName_id.key(actId);
//                                data->unfoundMoleculeAssociations.removeAll(mols.key(one));
//                                data->moleculeIdToAtcId_forModel.insertMulti(mols.key(one), atcName_id.key(actId));
//                            }
//                        }
//                        ++nb;
//                        ++natureLinkerNb;
//                    }
//                }
//            }
//            LOG(QString("Link composition by nature: %1 total associations founded.").arg(nb));
//        }
//        while (nb > 0);
//    }

    // Save completion percent in drugs database INFORMATION table
    data->completionPercentage = ((double) (1.0 - ((double)(data->unfoundMoleculeAssociations.count() - reviewedWithoutAtcLink) / (double)knownMoleculeNames.count())) * 100.00);
    LOG(QString("Molecule links completion: %1").arg(data->completionPercentage));
    //    DrugsDB::Tools::executeSqlQuery(QString("UPDATE SOURCES SET MOL_LINK_COMPLETION=%1 WHERE SID=%2")
    //                                 .arg(completion).arg(sid),
    //                                 Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
    QHash<int, QString> where;
    where.insert(SOURCES_SID, QString("='%1'").arg(sid));
    query.prepare(data->database->prepareUpdateQuery(Table_SOURCES, SOURCES_COMPLETION, where));
    query.bindValue(0, data->completionPercentage);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Tools", query);
        return false;
    }
    query.finish();

    // Inform model of founded links
    addAutoFoundMolecules(mol_atc_forModel, true);
    mol_atc_forModel.clear();

    qWarning()
            << "\nNUMBER OF MOLECULES" << knownMoleculeNames.count()
            << "\nCORRECTED BY NAME" << data->correctedByName.keys().count()
            << "\nCORRECTED BY ATC" << data->correctedByAtcCode.uniqueKeys().count()
            << "\nFOUNDED" << data->moleculeIdToAtcId.uniqueKeys().count()
            << QString("\nLINKERMODEL (WithATC:%1;WithoutATC:%2) %3").arg(modelFound).arg(reviewedWithoutAtcLink).arg(modelFound + reviewedWithoutAtcLink)
            << "\nLINKERNATURE" << natureLinkerNb
            << "\nLEFT" << (data->unfoundMoleculeAssociations.count() - reviewedWithoutAtcLink)
            << "\nCONFIDENCE INDICE" << data->completionPercentage
            << "\n\n";

    return true;
}

int MoleculeLinkerModel::removeUnreviewedMolecules()
{
    int nb = 0;
    int examined = 0;
    QDomElement n = d->m_RootItem->node().firstChildElement();

     while (!n.isNull()) {
         if (n.attribute("review").compare("true", Qt::CaseInsensitive) != 0) {
             QDomElement save = n.nextSiblingElement();
             QDomNode rem = d->m_RootItem->node().removeChild(n);
             if (!rem.isNull())
                 ++nb;
             n = save;
             ++examined;
             continue;
         }
         ++examined;
         n = n.nextSiblingElement();
     }
     reset();
     return nb;
}

