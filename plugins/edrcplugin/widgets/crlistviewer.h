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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                      *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_CRLISTVIEWER_H
#define EDRC_INTERNAL_CRLISTVIEWER_H

#include <edrcplugin/edrc_exporter.h>
#include <edrcplugin/widgets/edrcwontextualwidget.h>
#include <QWidget>
#include <QList>
#include <QModelIndex>
#include <QStyledItemDelegate>

/**
 * \file crlistviewer.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 28 Sept 2013
*/

namespace eDRC {
namespace Internal {
class CrTreeModel;
class eDRCContext;
class CrListViewerPrivate;
namespace Ui {
class CrListViewer;
}  // namespace Ui

class TreeViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TreeViewDelegate(QObject *parent = 0);
    void setCrTreeModel(CrTreeModel *model);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

public:
    mutable QModelIndex pressedIndex;
    CrTreeModel *_crTreeModel;
};

class EDRC_EXPORT CrListViewer : public EdrcContextualWidget
{
    Q_OBJECT
    
public:
    explicit CrListViewer(QWidget *parent = 0);
    ~CrListViewer();

    void setConsultResultTreeModel(CrTreeModel *model);
    void addHeaderWidget(QWidget *widget);
    
    void clear();

    // Contextual interface
private Q_SLOTS:
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSavePDF();
    void filePrint();
    void filePrintPreview();
    void editItem();
    void addItem();
    void removeItem();
    void clearItems();
    void renewItem();

private Q_SLOTS:
    void onModelReset();
    void onModelPostReset();
    void onCurrentItemChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Internal::CrListViewerPrivate *d;
};

} // namespace Internal
} // namespace eDRC

#endif // EDRC_INTERNAL_CRLISTVIEWER_H

