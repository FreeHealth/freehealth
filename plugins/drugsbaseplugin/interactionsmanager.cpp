/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class InteractionsManager
  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retreive interactions found using the other members.

  \sa DrugsInteraction, DrugsBases
  \ingroup freediams drugswidget
*/

#include "interactionsmanager.h"

#include <drugsbaseplugin/idrugengine.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/idruginteraction.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>

#include <extensionsystem/pluginmanager.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QSet>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>

namespace  {
    const char* const LIST_MASK =
            "<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
            "<tr>\n"
            "  <td colspan=2 align=center>\n"
            "   <span style=\"font-weight:bold\">%1\n</span>"
            "</td>\n"
            "</tr>\n"
            "%2\n"
            "</table>\n"
            "</span>\n";
}

using namespace DrugsDB;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }


DrugInteractionQuery::DrugInteractionQuery(const QVector<IDrug *> &testDrugs, QObject *parent) :
        QObject(parent), m_Drugs(testDrugs), m_TestDDI(true), m_TestPDI(true)
{
}

DrugInteractionQuery::DrugInteractionQuery(QObject *parent) :
        QObject(parent), m_TestDDI(true), m_TestPDI(true)
{
}

DrugInteractionQuery::~DrugInteractionQuery()
{
}

void DrugInteractionQuery::clearDrugsList()
{
    m_Drugs.clear();
}

void DrugInteractionQuery::setDrugsList(const QVector<IDrug *> &list)
{
    m_Drugs = list;
}

void DrugInteractionQuery::addDrug(IDrug *drug)
{
    m_Drugs.append(drug);
}

void DrugInteractionQuery::removeDrug(IDrug *drug)
{
    int id = m_Drugs.indexOf(drug);
    if (id!=-1) {
        m_Drugs.remove(id);
    }
}

void DrugInteractionQuery::removeLastInsertedDrug()
{
    if (!m_Drugs.isEmpty())
        m_Drugs.remove(m_Drugs.indexOf(m_Drugs.last()));
}

bool DrugInteractionQuery::containsDrug(const IDrug *drug) const
{
    return m_Drugs.contains((IDrug*)drug);
}



DrugInteractionResult::DrugInteractionResult(const QVector<IDrugInteraction *> &interactions, QObject *parent) :
        QObject(parent), m_Interactions(interactions), m_DDITested(false), m_PDITested(false)
{
}

DrugInteractionResult::DrugInteractionResult(QObject *parent) :
        QObject(parent), m_DDITested(false), m_PDITested(false)
{
}

DrugInteractionResult::~DrugInteractionResult()
{
    qDeleteAll(m_Interactions);
    m_Interactions.clear();
}

QVector<IDrugInteraction *> DrugInteractionResult::interactions(const QString &engineUid) const
{
    QVector<IDrugInteraction *> toReturn;
    for(int i = 0; i < m_Interactions.count(); ++i) {
        IDrugInteraction *di = m_Interactions.at(i);
        if (di->engine()->uid() == engineUid || engineUid.isEmpty()) {
            toReturn << di;
        }
    }
    return toReturn;
}

bool DrugInteractionResult::drugHaveInteraction(const IDrug *drug, const QString &engineUid) const
{
    for(int i = 0; i < m_Interactions.count(); ++i) {
        IDrugInteraction *di = m_Interactions.at(i);
        if (di->engine()->uid()==engineUid || engineUid.isEmpty()) {
            if (di->drugs().contains((IDrug*)drug))
                return true;
        }
    }
    return false;
}

QVector<IDrugInteraction *> DrugInteractionResult::getInteractions(const IDrug *drug, const QString &engineUid) const
{
    QVector<IDrugInteraction *> toReturn;
    for(int i = 0; i < m_Interactions.count(); ++i) {
        IDrugInteraction *di = m_Interactions.at(i);
        if (di->engine()->uid() == engineUid || engineUid.isEmpty()) {
            if (di->drugs().contains((IDrug*)drug))
                toReturn << di;
        }
    }
    return toReturn;
}

QIcon DrugInteractionResult::maxLevelOfInteractionIcon(const IDrug *drug, const int levelOfWarning, const int size, const QString &engineUid)
{
    QVector<IDrugInteraction *> dis = getInteractions(drug, engineUid);
    if (dis.isEmpty()) {
        return QIcon();
    }
    IDrugEngine *engine = dis.at(0)->engine();
    return engine->maximumInteractingLevelIcon(dis, drug, levelOfWarning, size);
}



namespace DrugsDB {
namespace Internal {

class InteractionsManagerPrivate
{
public:
    InteractionsManagerPrivate() :
            m_LogChrono(false)
    {
    }

    ~InteractionsManagerPrivate()
    {
    }

public:
    QVector<IDrugEngine*> m_Engines;
    bool m_LogChrono;
};

}  // End Internal
}  // End Drugs

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Managing drugs interactions -----------------------------------
//--------------------------------------------------------------------------------------------------------
InteractionsManager *InteractionsManager::m_Instance = 0;

InteractionsManager *InteractionsManager::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new InteractionsManager(parent);
    return m_Instance;
}

InteractionsManager::InteractionsManager(QObject *parent) :
        QObject(parent), d(0)
{
    static int handler = 0;
    ++handler;
    d = new Internal::InteractionsManagerPrivate();
    setObjectName("InteractionsManager" + QString::number(handler));

    // Get all engines from plugin manager
    d->m_Engines = pluginManager()->getObjects<IDrugEngine>().toVector();
    connect(pluginManager(), SIGNAL(objectAdded(QObject*)), this, SLOT(onNewObjectAddedToPluginManagerPool(QObject*)));
}

InteractionsManager::~InteractionsManager()
{
    if (d) delete d;
    d=0;
}

DrugInteractionResult *InteractionsManager::checkInteractions(const DrugInteractionQuery &query)
{
    QTime t;
    t.start();

    int nbInteractions = 0;
    DrugInteractionResult *result = new DrugInteractionResult;
    for(int i = 0; i < d->m_Engines.count(); ++i) {
        IDrugEngine *engine = d->m_Engines.at(i);
        if (!engine->isActive() || !engine->canComputeInteractions())
            continue;
        nbInteractions += engine->calculateInteractions(query.drugsList());
        if (engine->isCalculatingDrugDrugInteractions())
            result->setDDITested(true);
        if (engine->isCalculatingPatientDrugInteractions())
            result->setPDITested(true);
        result->addInteractions(engine->getAllInteractionsFound());

        if (d->m_LogChrono)
            Utils::Log::logTimeElapsed(t, engine->name(), QString("calculateInteractions() : Engine %1").arg(engine->name()));
    }

    return result;
}

///** \todo move this */
//QIcon InteractionsManager::interactionIcon(const int level, const int levelOfWarning, bool medium)  // static
//{
//    using namespace DrugsDB::Constants;
//    Core::ITheme *th = Core::ICore::instance()->theme();
//    Core::ITheme::IconSize size = Core::ITheme::SmallIcon;
//    if (medium)
//        size = Core::ITheme::MediumIcon;
//    if (level & Interaction::ContreIndication)
//        return th->icon(INTERACTION_ICONCRITICAL, size);
//    else if (level & Interaction::Deconseille)
//        return th->icon(INTERACTION_ICONDECONSEILLEE, size);
//    else if ((level & Interaction::APrendreEnCompte) && (levelOfWarning <= 1))
//        return th->icon(INTERACTION_ICONTAKEINTOACCOUNT, size);
//    else if ((level & Interaction::P450) && (levelOfWarning <= 1))
//        return th->icon(INTERACTION_ICONP450, size);
//    else if ((level & Interaction::GPG) && (levelOfWarning <= 1))
//        return th->icon(INTERACTION_ICONGPG, size);
//    else if ((level & Interaction::Precaution) && (levelOfWarning <= 1))
//        return th->icon(INTERACTION_ICONPRECAUTION, size);
//    else if ((level & Interaction::Information) && (levelOfWarning == 0))
//        return th->icon(INTERACTION_ICONINFORMATION, size);
//    else if ((level & Interaction::InnDuplication) && (levelOfWarning == 0))
//        return th->icon(INTERACTION_ICONINFORMATION, size);
//    else if (level & Interaction::noIAM)
//        return th->icon(INTERACTION_ICONOK, size);
//    else
//        return th->icon(INTERACTION_ICONUNKONW, size);
//}

//QIcon InteractionsManager::iamIcon(const IDrug *drug, const int &levelOfWarning, bool medium) const
//{
//    /** \todo code here, move this */
////    using namespace DrugsDB::Constants;
////    Core::ITheme::IconSize size = Core::ITheme::SmallIcon;
////    if (medium)
////        size = Core::ITheme::MediumIcon;
////    Core::ITheme *th = Core::ICore::instance()->theme();
////    if (drugHaveInteraction(drug)) {
////        Interaction::TypesOfIAM r = getMaximumTypeOfIAM(drug);
////        return interactionIcon(r, levelOfWarning);
////    } else if (levelOfWarning <= 1) {
////        if (!Internal::DrugsBase::instance()->drugsINNIsKnown(drug))
////            return th->icon(INTERACTION_ICONUNKONW,size);
////        else return th->icon(INTERACTION_ICONOK,size);
////    }
//    return QIcon();
//}

QString InteractionsManager::listToHtml(const QList<IDrugInteraction *> &list, bool fullInfos) // static
{
    /** \todo code here */
    QString tmp, toReturn;
//    QList<int> id_di;
//    foreach(IDrugInteraction *di, list) {
//        if (id_di.contains(di->value(IDrugInteraction::DI_Id).toInt()))
//            continue;
//        id_di << di->value(Internal::DrugsInteraction::DI_Id).toInt();
//        tmp += di->toHtml(fullInfos);
//    }
//    toReturn.append(QString(LIST_MASK)
//                    .arg(tr("Interaction(s) Found : ") , tmp));
    return toReturn;
}

QString InteractionsManager::synthesisToHtml(const QList<IDrugInteraction *> &list, bool fullInfos) // static
{
    /** \todo code here */
    QString tmp, toReturn;
//    QList<int> id_di;
//    foreach(IDrugInteraction *di, list) {
//        if (id_di.contains(di->value(Internal::DrugsInteraction::DI_Id).toInt()))
//            continue;
//        id_di << di->value(Internal::DrugsInteraction::DI_Id).toInt();
//        tmp += di->toHtml();
//    }
//    toReturn.append(QString(LIST_MASK)
//                    .arg(tr("Interaction(s) Found : ") , tmp));
    return toReturn;
}

void InteractionsManager::synthesisToTreeWidget(const QList<IDrugInteraction *> &list, QTreeWidget *tree) // static
{
    /** \todo code here */
    Q_ASSERT(tree);
//    using namespace DrugsDB::Constants;
//    QString tmp, toReturn;
//    QList<int> id_di;
//    QHash<QString, QTreeWidgetItem *> categories;
//    QFont bold;
//    bold.setBold(true);

//    foreach(Internal::DrugsInteraction *di, list) {

//        // No double
//        if (id_di.contains(di->value(Internal::DrugsInteraction::DI_Id).toInt()))
//            continue;
//        id_di << di->value(Internal::DrugsInteraction::DI_Id).toInt();

//        // Get the category
//        QTreeWidgetItem *category;
//        const QString &catName = di->value(Internal::DrugsInteraction::DI_TypeId).toString();
//        if (!categories.value(catName)) {
//            category = new QTreeWidgetItem(tree, QStringList() << catName);
//            category->setExpanded(true);
//            category->setFont(0, bold);
//            category->setForeground(0, QBrush(Qt::red));
//            categories.insert(catName, category);
//        }

//        // Include the interaction's datas
//        QTreeWidgetItem *interactors = new QTreeWidgetItem(category, QStringList()
//                                                           << QString("%1 <-> %2").arg(di->value(Internal::DrugsInteraction::DI_ATC1_Label).toString()).arg(di->value(Internal::DrugsInteraction::DI_ATC2_Label).toString()));
//        interactors->setFont(0, bold);
//        QTreeWidgetItem *risk = new QTreeWidgetItem(interactors);
//        QLabel *riskLabel = new QLabel(QString("%1: %2")
//                                       .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::NATURE_OF_RISK))
//                                       .arg(di->value(Internal::DrugsInteraction::DI_Risk).toString()));
//        riskLabel->setWordWrap(true);
//        tree->setItemWidget(risk, 0, riskLabel);

//        QTreeWidgetItem *management = new QTreeWidgetItem(interactors);
//        QLabel *managementLabel = new QLabel(QString("%1: %2")
//                                             .arg(tr("Management: "))
//                                             .arg(di->value(Internal::DrugsInteraction::DI_Management).toString()));
//        managementLabel->setWordWrap(true);
//        managementLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//        tree->setItemWidget(management, 0, managementLabel);
//        managementLabel->setMargin(0);
//        //        qWarning() << managementLabel << managementLabel->contentsMargins();
//    }
}

void InteractionsManager::onNewObjectAddedToPluginManagerPool(QObject *object)
{
    IDrugEngine *engine = qobject_cast<IDrugEngine*>(object);
    if (!engine)
        return;
}
