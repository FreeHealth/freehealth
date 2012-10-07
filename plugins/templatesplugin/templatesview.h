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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEMPLATESVIEW_H
#define TEMPLATESVIEW_H

#include <templatesplugin/templates_exporter.h>

#include <QtGui/QWidget>
#include <QItemSelectionModel>
#include <QAbstractItemView>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace Templates {
class TemplatesModel;
namespace Internal {
class TemplatesViewPrivate;
class TemplatesViewActionHandler;
}  // End namespace Internal

class TEMPLATES_EXPORT TemplatesView : public QWidget
{
    Q_OBJECT
    friend class Internal::TemplatesViewPrivate;
    friend class Internal::TemplatesViewActionHandler;

public:
    enum ViewContent {
        TemplatesAndCategories = 0,
        CategoriesOnly
    };

    enum EditMode {
        None       = 0x00,
        Add        = 0x01,
        Remove     = 0x02,
        Edit       = 0x04,
        Drag       = 0x08,
        Drop       = 0x10,
        LockUnlock = 0x20,
        Save       = 0x40,
        Print      = 0x80,
        Defaults   = Add | Remove | Edit | Drag | Drop | LockUnlock | Save | Print
    };
    Q_DECLARE_FLAGS(EditModes, EditMode)

    TemplatesView(QWidget *parent = 0, int viewContent = TemplatesAndCategories, EditModes editModes = Defaults);
    ~TemplatesView();

    void setViewContent(int viewContent);
    void setEditMode(EditModes);

    QItemSelectionModel *selectionModel() const;
    bool currentItemIsTemplate() const;
    QModelIndex currentItem() const;

    TemplatesModel *templatesModel() const;
    void expandAll() const;
    void setSelectionMode(QAbstractItemView::SelectionMode mode);


    //    ITemplates *currentTemplate() const;

Q_SIGNALS:
    void currentItemChanged();

protected Q_SLOTS:
    void addCategory();
    void removeItem();
    void editCurrentItem();
    void saveModel();
    void lock(bool state = true);
    bool printTemplate();

protected:
    void changeEvent(QEvent *e);
    bool isLocked() const;

private:
    Internal::TemplatesViewPrivate *d;
};

}  // End namespace Templates
Q_DECLARE_OPERATORS_FOR_FLAGS(Templates::TemplatesView::EditModes)

#endif // TEMPLATESVIEW_H
