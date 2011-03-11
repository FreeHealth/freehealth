/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
  \class DrugsDB::DrugInteractionResult
  \todo write documentation
  \sa DrugsDB::DrugsInteractionQuery, DrugsDB::InteractionManager
*/

#include "druginteractionresult.h"
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

QIcon DrugInteractionResult::maxLevelOfInteractionIcon(const IDrug *drug, const int levelOfWarning, const int size, const QString &engineUid)
{
    QVector<IDrugInteraction *> dis = getInteractions(drug, engineUid);
    if (dis.isEmpty()) {
        return QIcon();
    }
    IDrugEngine *engine = dis.at(0)->engine();
    return engine->maximumInteractingLevelIcon(dis, drug, levelOfWarning, size);
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
        for(int j=0; j < m_Interactions.count(); ++j) {
            IDrugInteraction *di = m_Interactions.at(j);
            if (di->engine()!=engine)
                continue;

//            // No double
//            if (id_di.contains(di->value(Internal::DrugsInteraction::DI_Id).toInt()))
//                continue;
//            id_di << di->value(Internal::DrugsInteraction::DI_Id).toInt();

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

            // Include the interaction's datas
            QString h = di->header();
            QStringList splitters;
            splitters << "<br>" << "<br />";
            bool interactorsAdded = false;
            foreach(const QString &s, splitters) {
                if (h.contains(s)) {
                    foreach(const QString &part, h.split(s)) {
                        QStandardItem *interactor = new QStandardItem(part.trimmed());
                        interactor->setData(j, Qt::UserRole);
                        interactor->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                        level->appendRow(interactor);
                        interactorsAdded = true;
                    }
                    break;
                }
            }
            if (!interactorsAdded) {
                QStandardItem *interactors = new QStandardItem(di->header());
                interactors->setData(j, Qt::UserRole);
                interactors->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                level->appendRow(interactors);
            }

//            QStandardItem *risk = new QStandardItem(di->risk());
//            QLabel *riskLabel = new QLabel(QString("%1: %2")
//                                           .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::NATURE_OF_RISK))
//                                           .arg(di->value(Internal::DrugsInteraction::DI_Risk).toString()));
//            risk->setWordWrap(true);
//            tree->setItemWidget(risk, 0, riskLabel);

//            QStandardItem *management = new QStandardItem(di->management());
//            QLabel *managementLabel = new QLabel(QString("%1: %2")
//                                                 .arg(tr("Management: "))
//                                                 .arg(di->value(Internal::DrugsInteraction::DI_Management).toString()));
//            managementLabel->setWordWrap(true);
//            managementLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//            tree->setItemWidget(management, 0, managementLabel);
//            managementLabel->setMargin(0);
            //        qWarning() << managementLabel << managementLabel->contentsMargins();
//            interactors->appendRow(risk);
//            interactors->appendRow(management);
        }
    }
    return m_StandardModel;
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
                          "    (NbOfInteractions: %4)")
            .arg(names.join("; "))
            .arg(m_DDITested)
            .arg(m_PDITested)
            .arg(m_Interactions.count());
    qWarning() << tmp;
}


