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
 ***************************************************************************/
#include "pimintegrator.h"
#include "pim_constants.h"
#include "ui_pimintegrator.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>
#include <QFile>
#include <QDir>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Constants;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/BIAM/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/druginfos-fr_FR.db");}

static inline QString treeXmlFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::PIMS_FILENAME);}

namespace DrugsDB {
namespace Internal {

class PimData
{
public:
    enum DataRepresentation {
        Source = 0,
        Molecule,
        Class,
        RelatedDiagnosis,
        RelatedCountry,
        Type,
        Level,
        RelatedMaxDose,
        RelatedMinDose,
        RelatedToLongTermUse
    };

    PimData() {}
    ~PimData() {}


private:
    QHash<int, QVariant> m_Datas;
};

}  // End namespace Internal
}  // End namespace DrugInfos

QString PimsTreePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + tr("PIM database");
}

PimTreeModel::PimTreeModel(QObject *parent) :
        QAbstractXmlTreeModel(treeXmlFile(), "FreeMedForms_PIMs", parent)
{
    setObjectName("PimTreeModel");
    setSubMainTag("PIMs");
//    d = new PimTreeModelPrivate(this);
}

PimTreeModel::~PimTreeModel()
{
//    if (d)
//        delete d;
//    d = 0;
}

int PimTreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant PimTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QDomNode node = this->node(index);
    if (node.isNull())
        return QVariant();

    QDomNamedNodeMap attributeMap = node.attributes();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case NameOrTranslatedText:
            if (node.toElement().tagName()==XML_TAG_RISK) {
                return attributeMap.namedItem(XML_ATTRIB_RISK_VALUE).nodeValue();
            }
            if (node.toElement().tagName()==XML_TAG_PIM) {
                return attributeMap.namedItem(XML_ATTRIB_PIM_LEVEL).nodeValue() .toUpper() + " Level PIM";
            }
            if (attributeMap.contains("name"))
                return attributeMap.namedItem("name").nodeValue();
            return node.toElement().tagName();
        case TypeOrLanguage:
            if (node.toElement().tagName()==XML_TAG_PIM)
                return attributeMap.namedItem(XML_ATTRIB_PIM_TYPE).nodeValue();
            if (node.toElement().tagName()==XML_TAG_RISK)
                return attributeMap.namedItem(XML_ATTRIB_RISK_LANG).nodeValue();
        case Level:
            if (node.toElement().tagName()==XML_TAG_PIM)
                return attributeMap.namedItem(XML_ATTRIB_PIM_LEVEL).nodeValue();
        case Review:
            return attributeMap.namedItem("review").nodeValue();
        default:
            return QVariant();
        }
    }
//    else if (role == Qt::CheckStateRole) {
//        if (index.column()==Review) {
//            if (attributeMap.namedItem("review").nodeValue() == "true")
//                return Qt::Checked;
//            else
//                return Qt::Unchecked;
//        }
//    } else if (role == Qt::ToolTipRole) {
//        QString tmp;
//        //        tmp += attributeMap.namedItem("name").nodeValue();
//        //        QStringList codes = attributeMap.namedItem("AtcCode").nodeValue().split(",");
//        //        tmp += "\n  " + codes.join("\n  ");
//        //        tmp += "\n  " + AtcModel::instance()->getAtcLabel(codes).join("\n  ");
//        return tmp;
//    } else if (role==Qt::ForegroundRole) {
//        if (!attributeMap.namedItem("autoFound").nodeValue().isEmpty()) {
//            return QColor("lightgray");
//        }
//    } else if (role == Qt::DecorationRole) {
//        if (index.column()==Name && node.toElement().tagName()=="Class") {
//            if (attributeMap.namedItem("review").nodeValue() == "true")
//                return QIcon(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/pixmap/16x16/ok.png");
//            else
//                return QIcon(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/pixmap/16x16/help.png");
//        }
//    } else if (role==Qt::BackgroundRole) {
//        if (node.toElement().tagName()=="Class") {
//            if (d->m_BuggyIncludes.uniqueKeys().contains(node.attributes().namedItem("name").nodeValue())) {
//                QColor c("red");
//                c.setAlpha(125);
//                return c;
//            }
//        }
//        if (node.toElement().tagName()=="Molecule") {
//            const QString &cl = node.parentNode().attributes().namedItem("name").nodeValue();
//            if (d->m_BuggyIncludes.values(cl).contains(attributeMap.namedItem("name").nodeValue())) {
//                QColor c("red");
//                c.setAlpha(125);
//                return c;
//            }
//        }
//    }
    return QVariant();
}

//bool PimTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if (!index.isValid())
//        return false;

//    DomItem *item = static_cast<DomItem*>(index.internalPointer());
//    QDomNode node = item->node();
//    QDomNamedNodeMap attributeMap = node.attributes();

//    if (role==Qt::EditRole) {
//        switch (index.column()) {
//        case Name:
//            if (node.toElement().tagName()=="Class")
//                return false;
//            attributeMap.namedItem("name").setNodeValue(value.toString());
//            break;
//        case Review:
//            attributeMap.namedItem("review").setNodeValue(value.toString());
//            break;
//        default:
//            return false;
//        }
//        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
//            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
//        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
//        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
//        return true;
//    } else if (role==Qt::CheckStateRole && index.column()==Review) {
//        if (value.toInt() == Qt::Checked) {
//            attributeMap.namedItem("review").setNodeValue("true");
//        } else {
//            attributeMap.namedItem("review").setNodeValue("false");
//        }
//        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
//            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
//        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
//        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
//        return true;
//    }

//    return false;
//}

//Qt::ItemFlags PimTreeModel::flags(const QModelIndex &index) const
//{
//    if (!index.isValid())
//        return 0;

//    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

//    DomItem *item = static_cast<DomItem*>(index.internalPointer());
//    QDomNode node = item->node();

//    if (node.isComment()) {
//        return f;
//    }

//    if (node.toElement().tagName() == "Class")
//        f |= Qt::ItemIsDropEnabled;

//    f |= Qt::ItemIsEditable;

//    if (index.column() == Name || index.column() == Date)
//        return f;

//    if (index.column() == Review)
//        f |= Qt::ItemIsUserCheckable;

//    return f;
//}

//QVariant PimTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//        switch (section) {
//        case Name:
//            return tr("Name");
//        case Review:
//            return tr("Review state");
//        case Reviewer:
//            return tr("Reviewer");
//        case Date:
//            return tr("Date of review");
//        default:
//            return QVariant();
//        }
//    }

//    return QVariant();
//}


QWidget *PimsTreePage::createPage(QWidget *parent)
{
    return new PimIntegrator(parent);
}

PimIntegrator::PimIntegrator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PimIntegrator)
{
    ui->setupUi(this);
    ui->treeView->setModel(new PimTreeModel(this));
}

PimIntegrator::~PimIntegrator()
{
    delete ui;
}

void PimIntegrator::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
