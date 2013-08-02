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
#ifndef DRUGINTERACTIONRESULT_H
#define DRUGINTERACTIONRESULT_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <coreplugin/itheme.h>

#include <QObject>
#include <QIcon>
#include <QStandardItemModel>
#include <QPointer>

/**
 * \file druginteractionresult.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 12 Mar 2011
*/

namespace DrugsDB {
class IDrug;
class IDrugInteraction;
class IDrugInteractionAlert;
class InteractionManager;
struct DrugInteractionInformationQuery;

class DRUGSBASE_EXPORT DrugInteractionResult : public QObject
{
    Q_OBJECT
    friend class DrugsDB::InteractionManager;

public:
    ~DrugInteractionResult();

    void clear();

    QVector<IDrugInteraction *> interactions(const QString &engineUid = QString::null) const;
    bool drugHaveInteraction(const IDrug *d, const QString &engineUid = QString::null) const;
    QVector<IDrugInteraction *> getInteractions(const IDrug *drug, const QString &engineUid = QString::null) const;

    // Alerts
    QIcon icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const;
    QString alertMessagesToHtml(const IDrug *drug, const DrugInteractionInformationQuery &query) const;
    QString alertMessagesToHtml(const DrugInteractionInformationQuery &query) const;
//    IDrugInteractionAlert *alert() const; ??

    bool isDrugDrugInteractionsTested() const {return m_DDITested;}
    bool isPatientDrugInteractionsTested() const {return m_PDITested;}

    QVector<IDrug *> testedDrugs() {return m_TestedDrugs;}
    QVector<IDrugInteractionAlert *> alerts(const DrugInteractionInformationQuery &query) const;

    QStandardItemModel *toStandardModel() const;

    void warn() const;

protected:
    DrugInteractionResult(const QVector<IDrugInteraction *> &interactions, QObject *parent = 0);
    DrugInteractionResult(QObject *parent = 0);

    void setInteractions(const QVector<IDrugInteraction *> &list) {m_Interactions = list;}
    void addInteractions(const QVector<IDrugInteraction *> &list) {m_Interactions << list;}
    void setDDITested(const bool test) {m_DDITested = test;}
    void setPDITested(const bool test) {m_PDITested = test;}

    void setTestedDrugs(const QVector<IDrug *> &drugs) {m_TestedDrugs = drugs;}
    void setInteractionAlert(const QVector<IDrugInteractionAlert *> &alerts);
    void addInteractionAlert(IDrugInteractionAlert *alert);
    void addInteractionAlerts(const QVector<IDrugInteractionAlert *> &alerts);

private:
    QVector<IDrugInteraction *> m_Interactions;
    QVector<IDrug *> m_TestedDrugs;
    bool m_DDITested, m_PDITested;
    mutable QPointer<QStandardItemModel> m_StandardModel;
    QVector<IDrugInteractionAlert *> m_Alerts;
};

}  // End namespace DrugsDB

#endif // DRUGINTERACTIONRESULT_H
