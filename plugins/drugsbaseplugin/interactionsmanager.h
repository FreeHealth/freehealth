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

// include drugswidget headers
#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

// include toolkit headers
#include <utils/database.h>

// include Qt headers
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>
#include <QMultiMap>

/**
 * \file interactionsmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/



namespace DrugsDB {
namespace Internal {
class DrugsData;
class DrugsInteraction;
class InteractionsManagerPrivate;
}  // End Internal

class DRUGSBASE_EXPORT InteractionsManager : public QObject
{
    Q_OBJECT
public:
    InteractionsManager(QObject *parent = 0);
    ~InteractionsManager();

    void addDrug(Internal::DrugsData *drug);
    void setDrugsList(const QList<Internal::DrugsData *> &list);
    void removeLastDrug();
    void clearDrugsList();

    bool checkInteractions();            // must be called first

    bool drugHaveInteraction( const Internal::DrugsData *d ) const;                      // must call first interactions()
    DrugsDB::Constants::Interaction::TypesOfIAM getMaximumTypeOfIAM( const Internal::DrugsData *d ) const;   // must call first interactions()
    QList<Internal::DrugsInteraction*> getInteractions( const Internal::DrugsData *d ) const;           // must call first interactions()
    Internal::DrugsInteraction *getLastInteractionFound() const;                      // must call first interactions()
    QList<Internal::DrugsInteraction*> getAllInteractionsFound() const;                      // must call first interactions()

    static QIcon interactionIcon(const int level, const int levelOfWarning = 0, bool medium = false);
    QIcon iamIcon(const Internal::DrugsData *drug, const int &levelOfWarning = 0, bool medium = false) const;
    static QString listToHtml( const QList<Internal::DrugsInteraction *> & list, bool fullInfos );
    static QString synthesisToHtml( const QList<Internal::DrugsInteraction *> & list, bool fullInfos );
    static void synthesisToTreeWidget(const QList<Internal::DrugsInteraction *> &list, QTreeWidget *tree);

private:
    // intialization state
    static bool m_initialized;
    Internal::InteractionsManagerPrivate *d;
};

}  // End DrugsDB

#endif   // MFINTERACTIONSMANAGER_H
