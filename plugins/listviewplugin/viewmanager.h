/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QListView>
#include <QPointer>
#include <QPushButton>
#include <QEvent>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QFocusEvent>
#include <QToolBar>
class QHBoxLayout;
class QAction;

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/icontext.h>
#include <listviewplugin/extendedview.h>

namespace Views {
namespace Internal {

class ViewContext : public Core::IContext
{
public:
    ViewContext(IView *parent) : Core::IContext(parent)
    {
        setObjectName("IViewContext");
        setWidget(parent);
    }

};

class ViewActionHandler : public QObject
{
    Q_OBJECT
public:
    ViewActionHandler(QObject *parent = 0);
    virtual ~ViewActionHandler() {}

    void setCurrentView(IView *view);


private Q_SLOTS:
    void moveUp();
    void moveDown();
    void addItem();
    void removeItem();
    void listViewItemChanged();

private:
    bool canMoveUp();
    bool canMoveDown();
    void updateActions();

protected:
    QAction *aAddRow;
    QAction *aRemoveRow;
    QAction *aDown;
    QAction *aUp;
    QAction *aEdit;
    QAction *aClear;

    QPointer<IView> m_CurrentView;
};

// TODO: remove singleton and move the initialization in the IPlugin implementation
class ViewManager : public ViewActionHandler
{
    Q_OBJECT
public:
    static ViewManager *instance(QObject *parent = 0);
    ~ViewManager() {}

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);

private:
    ViewManager(QObject *parent = 0);
    static ViewManager *m_Instance;
};

}  // End Internal
}  // End Views


#endif // VIEWMANAGER_H
