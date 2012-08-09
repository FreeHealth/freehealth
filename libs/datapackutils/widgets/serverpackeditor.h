/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_SERVERPACKEDITOR_H
#define DATAPACK_SERVERPACKEDITOR_H

#include <datapackutils/datapack_exporter.h>

#include <QWidget>
class QStandardItemModel;
class QModelIndex;
class QToolBar;
class QListWidgetItem;
class QDataWidgetMapper;

/**
 * \file serverpackeditor.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Feb 2012
*/

namespace DataPack {
class PackDescription;
class Pack;
class PackModel;
class ServerModel;
namespace Internal {
class ServerPackEditorPrivate;
namespace Ui {
    class ServerPackEditor;
} // Ui
} // Internal

class DATAPACK_EXPORT ServerPackEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ServerPackEditor(QWidget *parent = 0);
    ~ServerPackEditor();

public Q_SLOTS:
    bool refreshServerContent();
    bool submitChanges();

private:
    void createActions();
    void createToolbar();
//    void createServerDataWidgetMapper();
    void populatePackView(const int packId);
    void populateServerView(const int serverId);

private Q_SLOTS:
    void switchToPackView();
    void switchToServerView();
    void onPackCategoriesChanged(const QModelIndex &index, const QModelIndex &previous);
    void onPackIndexActivated(const QModelIndex &index, const QModelIndex &previous);
    void serverActionTriggered(QAction *a);
    void refreshPacks();
    void processPacks();
    void serverCurrentChanged(const QModelIndex &c, const QModelIndex &p);
    void selectedFirstRow();

private:
    void retranslate();
    void changeEvent(QEvent *e);

private:
    Internal::ServerPackEditorPrivate *d;
};

}  // End namespace DataPack

#endif // DATAPACK_SERVERPACKEDITOR_H
