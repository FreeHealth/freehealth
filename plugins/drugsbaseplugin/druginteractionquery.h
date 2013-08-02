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
#ifndef DRUGINTERACTIONQUERY_H
#define DRUGINTERACTIONQUERY_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <coreplugin/itheme.h>

#include <QObject>
#include <QIcon>
#include <QStandardItemModel>
#include <QPointer>

/**
 * \file druginteractionquery.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 11 Mar 2011
*/

namespace DrugsDB {
class IDrug;
class IDrugInteraction;

class DRUGSBASE_EXPORT DrugInteractionQuery : public QObject
{
    Q_OBJECT
public:
    DrugInteractionQuery(const QVector<IDrug *> &testDrugs, QObject *parent = 0);
    DrugInteractionQuery(QObject *parent = 0);
    ~DrugInteractionQuery();

    void clearDrugsList();
    void setDrugsList(const QVector<IDrug *> &list);
    QVector<IDrug *> drugsList() const {return m_Drugs;}

    void addDrug(IDrug *drug);
    void removeDrug(IDrug *drug);
    void removeLastInsertedDrug();

    bool containsDrug(const IDrug *drug) const;
    QStandardItemModel *toStandardModel() const;

    void setTestDrugDrugInteractions(bool test) {m_TestDDI = test;}
    void setTestPatientDrugInteractions(bool test) {m_TestPDI = test;}

    QString warnText() const;

private:
    QVector<IDrug *> m_Drugs;
    bool m_TestDDI, m_TestPDI;
    mutable QPointer<QStandardItemModel> m_StandardModel;
};


}  // End namespace DrugsDB


DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::DrugInteractionQuery *c);
DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::DrugInteractionQuery &c);

#endif // DRUGINTERACTIONRESULT_H
