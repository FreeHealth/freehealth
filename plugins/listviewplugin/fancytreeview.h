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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FANCYTREEVIEW_H
#define FANCYTREEVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <utils/widgets/qbuttonlineedit.h>

#include <QWidget>
#include <QTreeView>
#include <QPushButton>
#include <QStyledItemDelegate>


QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
QT_END_NAMESPACE


namespace Views {
namespace Internal {
class FancyTreeViewPrivate;
}  // End namespace Internal

namespace Ui {
    class FancyTreeView;
}

class LISTVIEW_EXPORT FancyTreeView : public QWidget
{
    Q_OBJECT

public:
    enum ButtonAction {
        FTV_SaveModel    = 0x0001,
        FTV_RevertModel  = 0x0002,
        FTV_CreateNew    = 0x0004,
        FTV_RemoveRow    = 0x0008,
        FTV_PrintItem    = 0x0010
    };
    Q_DECLARE_FLAGS(ButtonActions, ButtonAction)


    explicit FancyTreeView(QWidget *parent = 0);
    ~FancyTreeView();

    QTreeView *treeView() const;

    QToolButton *button();
    void setButtonActions(const ButtonActions &actions, bool connectToDefault = false);

    Utils::QButtonLineEdit *searchLine();

    void setHeaderMenu(QMenu *menu);
    void setSubHeadingMenu(QMenu *menu);
    void setItemMenu(QMenu *menu);
    void useContextMenu(bool state);

    void setModel(QAbstractItemModel *model, const int columnForFancyButton = -1);

    // QTreeView wrapping
    void hideColumn(int column);

private Q_SLOTS:
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);
    void save();
    void addItem();

Q_SIGNALS:
    void saveRequested();
    void addItemRequested();
    void removeItemRequested();
    void printItemRequested();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FancyTreeView *ui;
    Internal::FancyTreeViewPrivate *d;
};


namespace Internal {

class TreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    friend class Views::FancyTreeView;

public:
    TreeItemDelegate(QObject *parent = 0);
    void setModel(QAbstractItemModel *model) {m_Model = model;}
    void setFancyColumn(int col) {m_FancyColumn=col;}
    int fancyColumn() {return m_FancyColumn;}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

protected:
    mutable QModelIndex pressedIndex;
    QAbstractItemModel *m_Model;
    int m_FancyColumn;
};

}  // End namespace Internal

}  // End namespace Views;

Q_DECLARE_OPERATORS_FOR_FLAGS(Views::FancyTreeView::ButtonActions)

#endif // FANCYTREEVIEW_H
