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
  \class DrugsDB::DrugInteractionQuery
  \todo write documentation
  \sa DrugsDB::DrugsInteractionResult, DrugsDB::InteractionManager
*/

#include "druginteractionquery.h"
#include "idrug.h"

#include <QDebug>

using namespace DrugsDB;

DrugInteractionQuery::DrugInteractionQuery(const QVector<IDrug *> &testDrugs, QObject *parent) :
        QObject(parent), m_Drugs(testDrugs), m_TestDDI(true), m_TestPDI(true), m_StandardModel(0)
{
}

DrugInteractionQuery::DrugInteractionQuery(QObject *parent) :
        QObject(parent), m_TestDDI(true), m_TestPDI(true), m_StandardModel(0)
{
}

DrugInteractionQuery::~DrugInteractionQuery()
{
    if (m_StandardModel)
        delete m_StandardModel;
    m_StandardModel = 0;
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
    if (!m_Drugs.contains(drug))
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

/** Creates a StandardItemModel with the queried drugs. Items userData are DrugID. */
QStandardItemModel *DrugInteractionQuery::toStandardModel() const
{
    if (!m_StandardModel) {
        m_StandardModel = new QStandardItemModel;
    }

    // for all drugs
    QFont bold;
    bold.setBold(true);
    QVector<IDrug *> insertedDrugs;
    for(int i=0; i < m_Drugs.count(); ++i) {
        IDrug *drug = m_Drugs.at(i);
        if (insertedDrugs.contains(drug))
            continue;
        insertedDrugs.append(drug);
        // add a root item
        QStandardItem *root = new QStandardItem(drug->brandName());
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        root->setData(drug->drugId());
        m_StandardModel->appendRow(root);
        // TODO: foreach component of the drug, append total daily dose
    }
    return m_StandardModel;
}

QString DrugInteractionQuery::warnText() const
{
    QString tmp;
    for(int i=0; i < m_Drugs.count(); ++i) {
        tmp += "  * " + m_Drugs.at(i)->brandName() + "\n";
    }
    if (tmp.isEmpty())
        tmp = "  !! No drug\n";
    tmp = QString("DrugInteractionQuery: testing\n%1"
                  "  * TestDDI: %2\n"
                  "  * TestPDI: %3")
          .arg(tmp)
          .arg(m_TestDDI)
          .arg(m_TestPDI);
    return tmp;
}

QDebug operator<<(QDebug dbg, const DrugsDB::DrugInteractionQuery *c)
{
    if (!c) {
        dbg.nospace() << "DrugInteractionQuery(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

QDebug operator<<(QDebug dbg, const DrugsDB::DrugInteractionQuery &c)
{
    dbg.nospace() << c.warnText();
    return dbg.space();
}


