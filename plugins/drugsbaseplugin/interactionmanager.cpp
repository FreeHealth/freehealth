/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class DrugsDB::InteractionManager
  \todo write documentation
  \sa DrugsDB::DrugsInteractionResult, DrugsDB::DrugsInteractionQuery
*/

#include "interactionmanager.h"

#include <drugsbaseplugin/idrugengine.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/druginteractionquery.h>
#include <drugsbaseplugin/druginteractionresult.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>
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
#include <QStandardItemModel>

enum { WarnComputations = false };

using namespace DrugsDB;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }

namespace DrugsDB {
namespace Internal {
class InteractionManagerPrivate
{
public:
    InteractionManagerPrivate(InteractionManager *parent) :
        m_LogChrono(false),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~InteractionManagerPrivate()
    {
    }

public:
    QVector<IDrugEngine*> m_Engines;
    bool m_LogChrono;

private:
    InteractionManager *q;
};

}  // End Internal
}  // End Drugs

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Managing drugs interactions -----------------------------------
//--------------------------------------------------------------------------------------------------------
InteractionManager::InteractionManager(QObject *parent) :
        QObject(parent), d(0)
{
    static int handler = 0;
    ++handler;
    d = new Internal::InteractionManagerPrivate(this);
    setObjectName("InteractionManager" + QString::number(handler));

    // Get all engines from plugin manager
    d->m_Engines = pluginManager()->getObjects<IDrugEngine>().toVector();
    connect(pluginManager(), SIGNAL(objectAdded(QObject*)), this, SLOT(onNewObjectAddedToPluginManagerPool(QObject*)));
}

InteractionManager::~InteractionManager()
{
    if (d) delete d;
    d=0;
}

DrugInteractionResult *InteractionManager::checkInteractions(const DrugInteractionQuery &query, QObject *parent)
{
    if (query.drugsList().isEmpty())
        return new DrugInteractionResult(parent);

    QTime t;
    t.start();
    int nbInteractions = 0;
    DrugInteractionResult *result = new DrugInteractionResult(parent);
    result->setTestedDrugs(query.drugsList());

    for(int i = 0; i < d->m_Engines.count(); ++i) {
        IDrugEngine *engine = d->m_Engines.at(i);

        if (WarnComputations) {
            qWarning() << "DrugEngine" << engine->name()
                       << "Active" << engine->isActive()
                       << "CanCompute" <<  engine->canComputeInteractions()
                       << "nbDrugs" << query.drugsList().count();
        }

        if (!engine->isActive() || !engine->canComputeInteractions())
            continue;

        nbInteractions += engine->calculateInteractions(query.drugsList());
        if (WarnComputations) {
            qWarning() << "Found" << nbInteractions;
        }

        if (engine->isCalculatingDrugDrugInteractions())
            result->setDDITested(true);
        if (engine->isCalculatingPatientDrugInteractions())
            result->setPDITested(true);

        result->addInteractions(engine->getAllInteractionsFound());
        result->addInteractionAlerts(engine->getAllAlerts(result));

        if (d->m_LogChrono)
            Utils::Log::logTimeElapsed(t, engine->name(), QString("calculateInteractions(): Engine %1").arg(engine->name()));
    }

    return result;
}

//QString InteractionManager::listToHtml(const QVector<IDrugInteraction *> &list, bool fullInfos) // static
//{
//    QString tmp, toReturn;

//    // get all engines
//    QVector<IDrugEngine*> engines;
//    for(int i=0; i < list.count(); ++i) {
//        if (!engines.contains(list.at(i)->engine()))
//            engines << list.at(i)->engine();
//    }

//    // for all engine create the interaction list
//    for(int i=0; i<engines.count(); ++i) {
//        IDrugEngine *eng = engines.at(i);
//        for(int j=0; j < list.count(); ++j) {
//            IDrugInteraction *di = list.at(j);
//            if (di->engine()==eng) {
////                tmp += di->toHtml(fullInfos);
//                tmp += "-&nbsp;" + di->type() + "<br />";
//            }
//        }
//        if (!tmp.isEmpty()) {
//            tmp.chop(6);
//            toReturn.append(QString(LIST_MASK)
//                            .arg(eng->name())
//                            .arg(tmp));
//        }
//    }

//    return toReturn;
//}

QString InteractionManager::drugInteractionSynthesisToHtml(const IDrug *drug, const QVector<IDrugInteraction *> &list, bool fullInfos)
{
    // create the interaction list
    QVector<IDrugInteraction *> interactions;
    for(int i=0; i < list.count(); ++i) {
        IDrugInteraction *interaction = list.at(i);

        if (interaction->drugs().contains((IDrug*)drug)) {
            interactions << interaction;
        }
    }

    return synthesisToHtml(interactions, fullInfos);
}

QString InteractionManager::synthesisToHtml(const QVector<IDrugInteraction *> &list, bool fullInfos) // static
{
    Q_UNUSED(fullInfos);
    QString display;

    // get all engines
    QVector<IDrugEngine*> engines;
    for(int i=0; i < list.count(); ++i) {
        if (!engines.contains(list.at(i)->engine()))
            engines << list.at(i)->engine();
    }

    // for all engine create the interaction list
    for(int j=0; j<engines.count(); ++j) {
        IDrugEngine *eng = engines.at(j);
        QList<IDrug *> concernedDrugs;
        // retrieve all tested drugs
        for(int i=0; i < list.count(); ++i) {
            IDrugInteraction *interaction = list.at(i);

            if (interaction->engine()!=eng)
                continue;

            foreach(IDrug *drg, interaction->drugs()) {
                if (!concernedDrugs.contains(drg))
                    concernedDrugs.append(drg);
            }
        }

        // add tested drug brand names
        display.append(QString("<p><center>%1</center></p><p>").arg(eng->name()));
        for(int i = 0; i < concernedDrugs.count(); ++i) {
            IDrug *drg = concernedDrugs.at(i);
            display.append(QString("%1&nbsp;.&nbsp;%2<br />")
                           .arg(i)
                           .arg(drg->brandName()));
        }
        display.append("</p><p>");

        if (concernedDrugs.count() > 0) {
            for(int i=0; i<list.count(); ++i)
                display.append(list.at(i)->toHtml(true));
        } else {
            display = tkTr(Trans::Constants::NO_1_FOUND).arg(tkTr(Trans::Constants::INTERACTION));
        }
        display.append("</p>");
    }
    return display;
}

//void InteractionManager::synthesisToTreeWidget(const QList<IDrugInteraction *> &list, QTreeWidget *tree) // static
//{
//    // TODO: code here
//    Q_ASSERT(tree);
////    using namespace DrugsDB::Constants;
////    QString tmp, toReturn;
////    QList<int> id_di;
////    QHash<QString, QTreeWidgetItem *> categories;
////    QFont bold;
////    bold.setBold(true);

////    foreach(Internal::DrugsInteraction *di, list) {

////        // No double
////        if (id_di.contains(di->value(Internal::DrugsInteraction::DI_Id).toInt()))
////            continue;
////        id_di << di->value(Internal::DrugsInteraction::DI_Id).toInt();

////        // Get the category
////        QTreeWidgetItem *category;
////        const QString &catName = di->value(Internal::DrugsInteraction::DI_TypeId).toString();
////        if (!categories.value(catName)) {
////            category = new QTreeWidgetItem(tree, QStringList() << catName);
////            category->setExpanded(true);
////            category->setFont(0, bold);
////            category->setForeground(0, QBrush(Qt::red));
////            categories.insert(catName, category);
////        }

////        // Include the interaction's data
////        QTreeWidgetItem *interactors = new QTreeWidgetItem(category, QStringList()
////                                                           << QString("%1 <-> %2").arg(di->value(Internal::DrugsInteraction::DI_ATC1_Label).toString()).arg(di->value(Internal::DrugsInteraction::DI_ATC2_Label).toString()));
////        interactors->setFont(0, bold);
////        QTreeWidgetItem *risk = new QTreeWidgetItem(interactors);
////        QLabel *riskLabel = new QLabel(QString("%1: %2")
////                                       .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::NATURE_OF_RISK))
////                                       .arg(di->value(Internal::DrugsInteraction::DI_Risk).toString()));
////        riskLabel->setWordWrap(true);
////        tree->setItemWidget(risk, 0, riskLabel);

////        QTreeWidgetItem *management = new QTreeWidgetItem(interactors);
////        QLabel *managementLabel = new QLabel(QString("%1: %2")
////                                             .arg(tr("Management: "))
////                                             .arg(di->value(Internal::DrugsInteraction::DI_Management).toString()));
////        managementLabel->setWordWrap(true);
////        managementLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
////        tree->setItemWidget(management, 0, managementLabel);
////        managementLabel->setMargin(0);
////        //        qWarning() << managementLabel << managementLabel->contentsMargins();
////    }
//}

void InteractionManager::onNewObjectAddedToPluginManagerPool(QObject *object)
{
    IDrugEngine *engine = qobject_cast<IDrugEngine*>(object);
    if (!engine)
        return;
    d->m_Engines.append(engine);
}
