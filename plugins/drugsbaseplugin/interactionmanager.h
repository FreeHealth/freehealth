/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef INTERACTIONSMANAGER_H
#define INTERACTIONSMANAGER_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QVector>
#include <QIcon>

class QTreeWidget;


/**
 * \file interactionsmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 05 Feb 2012
*/

namespace DrugsDB {
class DrugBaseCore;
class IDrug;
class IDrugInteraction;
class DrugInteractionResult;
class DrugInteractionQuery;

namespace Internal {
class InteractionManagerPrivate;
}  // End Internal


class DRUGSBASE_EXPORT InteractionManager : public QObject
{
    Q_OBJECT
    friend class DrugsDB::DrugBaseCore;

protected:
    InteractionManager(QObject *parent = 0);

public:
    ~InteractionManager();

    DrugInteractionResult *checkInteractions(const DrugInteractionQuery &query, QObject *parent);

//    static QIcon interactionIcon(const int level, const int levelOfWarning = 0, bool medium = false);
//    QIcon iamIcon(const IDrug *drug, const int &levelOfWarning = 0, bool medium = false) const;
//    static QString listToHtml(const QVector<IDrugInteraction *> &list, bool fullInfos);
    static QString drugInteractionSynthesisToHtml(const IDrug *drug, const QVector<IDrugInteraction *> &list, bool fullInfos);
    static QString synthesisToHtml(const QVector<IDrugInteraction *> &list, bool fullInfos);
//    static void synthesisToTreeWidget(const QList<IDrugInteraction *> &list, QTreeWidget *tree);

private Q_SLOTS:
    void onNewObjectAddedToPluginManagerPool(QObject *object);

private:
    Internal::InteractionManagerPrivate *d;
};

}  // End DrugsDB

#endif   // MFINTERACTIONSMANAGER_H
