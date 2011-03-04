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
#ifndef INTERACTIONSMANAGER_H
#define INTERACTIONSMANAGER_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

#include <coreplugin/itheme.h>

#include <utils/database.h>

#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>
#include <QMultiMap>

/**
 * \file interactionsmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 03 Mar 2011
*/

namespace DrugsDB {
class IDrug;
class IDrugInteraction;
class InteractionManager;

namespace Internal {
class InteractionManagerPrivate;
}  // End Internal


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

    void setTestDrugDrugInteractions(bool test) {m_TestDDI = test;}
    void setTestPatientDrugInteractions(bool test) {m_TestPDI = test;}

    void warn() const;

private:
    QVector<IDrug *> m_Drugs;
    bool m_TestDDI, m_TestPDI;
};


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

    QIcon maxLevelOfInteractionIcon(const IDrug *drug, const int levelOfWarning, const int size = Core::ITheme::SmallIcon, const QString &engineUid = QString::null);

    bool isDrugDrugInteractionsTested() const {return m_DDITested;}
    bool isPatientDrugInteractionsTested() const {return m_PDITested;}

    void warn() const;

protected:
    DrugInteractionResult(const QVector<IDrugInteraction *> &interactions, QObject *parent = 0);
    DrugInteractionResult(QObject *parent = 0);

    void setInteractions(const QVector<IDrugInteraction *> &list) {m_Interactions = list;}
    void addInteractions(const QVector<IDrugInteraction *> &list) {m_Interactions << list;}
    void setDDITested(const bool test) {m_DDITested = test;}
    void setPDITested(const bool test) {m_PDITested = test;}

private:
    QVector<IDrugInteraction *> m_Interactions;
    bool m_DDITested, m_PDITested;
};



class DRUGSBASE_EXPORT InteractionManager : public QObject
{
    Q_OBJECT
    InteractionManager(QObject *parent = 0);
public:
    static InteractionManager *instance(QObject *parent = 0);
    ~InteractionManager();

    DrugInteractionResult *checkInteractions(const DrugInteractionQuery &query);

//    static QIcon interactionIcon(const int level, const int levelOfWarning = 0, bool medium = false);
//    QIcon iamIcon(const IDrug *drug, const int &levelOfWarning = 0, bool medium = false) const;
    static QString listToHtml(const QVector<IDrugInteraction *> &list, bool fullInfos);
    static QString drugInteractionSynthesisToHtml(const IDrug *drug, const QVector<IDrugInteraction *> &list, bool fullInfos);
    static QString synthesisToHtml(const QVector<IDrugInteraction *> &list, bool fullInfos);
    static void synthesisToTreeWidget(const QList<IDrugInteraction *> &list, QTreeWidget *tree);

private Q_SLOTS:
    void onNewObjectAddedToPluginManagerPool(QObject *object);

private:
    // intialization state
    static InteractionManager *m_Instance;
    static bool m_initialized;
    Internal::InteractionManagerPrivate *d;
};

}  // End DrugsDB

#endif   // MFINTERACTIONSMANAGER_H
