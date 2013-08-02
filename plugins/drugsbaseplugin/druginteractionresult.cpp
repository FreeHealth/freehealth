/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
  \class DrugsDB::DrugInteractionResult
  \todo write documentation
  \sa DrugsDB::DrugsInteractionQuery, DrugsDB::InteractionManager
*/

#include "druginteractionresult.h"
#include "idruginteractionalert.h"
#include "idruginteraction.h"
#include "idrugengine.h"
#include "idrug.h"

#include <QDebug>

using namespace DrugsDB;


DrugInteractionResult::DrugInteractionResult(const QVector<IDrugInteraction *> &interactions, QObject *parent) :
        QObject(parent), m_Interactions(interactions), m_DDITested(false), m_PDITested(false), m_StandardModel(0)
{
}

DrugInteractionResult::DrugInteractionResult(QObject *parent) :
        QObject(parent), m_DDITested(false), m_PDITested(false), m_StandardModel(0)
{
}

DrugInteractionResult::~DrugInteractionResult()
{
    qDeleteAll(m_Interactions);
    m_Interactions.clear();
    qDeleteAll(m_Alerts);
    m_Alerts.clear();
    if (m_StandardModel)
        delete m_StandardModel;
}

void DrugInteractionResult::clear()
{
    qDeleteAll(m_Interactions);
    m_Interactions.clear();
    m_DDITested = false;
    m_PDITested = false;
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

QIcon DrugInteractionResult::icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const
{
    for(int i=0; i < m_Alerts.count(); ++i) {
        // TODO: manage processtime of alerts
        QIcon icon = m_Alerts.at(i)->icon(drug, query);
        if (!icon.isNull())
            return icon;
    }
    return QIcon();
}

QString DrugInteractionResult::alertMessagesToHtml(const IDrug *drug, const DrugInteractionInformationQuery &query) const
{
    QHash<QString,QString> msgs; // one msg per engineUid
    for(int i=0; i < m_Alerts.count(); ++i) {
        IDrugInteractionAlert *alert = m_Alerts.at(i);
        if (query.engineUid.isEmpty() || alert->engineUid()==query.engineUid) {
            QString &msg = msgs[alert->engineUid()];
            if (drug)
                msg += alert->message(drug, query);
            else
                msg += alert->message(query);
        }
    }
    if (query.engineUid.isEmpty()) {
        QString msg;
        // TODO: add drugengine header
        foreach(const QString &uids, msgs) {
            msg += msgs.value(uids);
        }
        return msg;
    } else {
        return msgs.value(query.engineUid);
    }
    return QString();
}

QString DrugInteractionResult::alertMessagesToHtml(const DrugInteractionInformationQuery &query) const
{
    return alertMessagesToHtml(0, query);
}

QVector<IDrugInteractionAlert *> DrugInteractionResult::alerts(const DrugInteractionInformationQuery &query) const
{
    Q_UNUSED(query);
    // get alerts according to the ProcessTime, RelatedDrug...
    return m_Alerts;
}

QStandardItemModel *DrugInteractionResult::toStandardModel() const
{
    if (!m_StandardModel) {
        m_StandardModel = new QStandardItemModel;
    }
    m_StandardModel->clear();

    // get all engines
    QVector<IDrugEngine *> engines;
    for(int i=0; i < m_Interactions.count(); ++i) {
        IDrugEngine *engine = m_Interactions.at(i)->engine();
        if (!engines.contains(engine))
            engines << engine;
    }

    // for all engines
    QFont bold;
    bold.setBold(true);
    for(int i=0; i<engines.count();++i) {
        // add a root item
        IDrugEngine *engine = engines.at(i);
        QStandardItem *root = new QStandardItem(engine->icon(Core::ITheme::MediumIcon),engine->name());
        root->setData(-1, Qt::UserRole);
        root->setFont(bold);
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_StandardModel->appendRow(root);

        // for all interactions for this engine
        QHash<QString, QStandardItem *> levels;
        QHash<QString, QStandardItem *> mainInteractors;
        for(int j=0; j < m_Interactions.count(); ++j) {
            IDrugInteraction *di = m_Interactions.at(j);
            if (di->engine()!=engine)
                continue;

            // TODO: code here : no double

            // Get the interaction level item
            QStandardItem *level = levels.value(di->type(), 0);
            if (!level) {
                level = new QStandardItem(di->type());
                level->setIcon(di->icon());
                level->setFont(bold);
//                level->setForeground(QBrush(Qt::red));
                level->setData(-1, Qt::UserRole);
                level->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                root->appendRow(level);
                levels.insert(di->type(), level);
            }

            // Include the interaction's data
            QStringList h = di->header("@@").split("@@", QString::SkipEmptyParts);
            if (h.count()==0)
                continue;
            // Main interactor
            QStandardItem *mainInteractor = mainInteractors.value(di->type() + h.at(0).trimmed(), 0);
            if (!mainInteractor) {
                mainInteractor = new QStandardItem(h.at(0).trimmed());
                level->appendRow(mainInteractor);
                mainInteractors.insert(di->type() + h.at(0).trimmed(), mainInteractor);
            }
            // Interactors
            for(int w = 1; w < h.count(); ++w) {
                QString childName = h.at(w).trimmed();
                // already in clidren's list
                bool included = false;
                for(int z = 0; z < mainInteractor->rowCount(); ++z) {
                    if (mainInteractor->child(z)->text()==childName) {
                        included = true;
                        break;
                    }
                }
                if (included)
                    continue;
                // include as children of mainInteractor
                QStandardItem *interactor = new QStandardItem(childName);
                interactor->setData(j, Qt::UserRole);
                interactor->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                mainInteractor->appendRow(interactor);
            }

        }
    }
    return m_StandardModel;
}

void DrugInteractionResult::setInteractionAlert(const QVector<IDrugInteractionAlert *> &alerts)
{
    qDeleteAll(m_Alerts);
    m_Alerts.clear();
    m_Alerts = alerts;
}

void DrugInteractionResult::addInteractionAlert(IDrugInteractionAlert *alert)
{
    if (!m_Alerts.contains(alert))
        m_Alerts.append(alert);
}

void DrugInteractionResult::addInteractionAlerts(const QVector<IDrugInteractionAlert *> &alerts)
{
    for(int i=0; i < alerts.count(); ++i)
        addInteractionAlert(alerts.at(i));
}

void DrugInteractionResult::warn() const
{
    // get all IDrugEngine names
    QStringList names;
    for(int i=0; i<m_Interactions.count();++i) {
        if (!names.contains(m_Interactions.at(i)->engine()->name()))
            names << m_Interactions.at(i)->engine()->name();
    }
    QString tmp = QString("DrugInteractionResult: %1\n"
                          "    (DDITested: %2; PDITested: %3)\n"
                          "    (NbOfInteractions: %4)"
                          "    (NbOfAlerts: %5)")
            .arg(names.join("; "))
            .arg(m_DDITested)
            .arg(m_PDITested)
            .arg(m_Interactions.count())
            .arg(m_Alerts.count());
    qWarning() << tmp;
}


