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
#ifndef TEMPLATEVIEW_P_H
#define TEMPLATEVIEW_P_H

#include <templatesplugin/templatesview.h>

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QPointer>

namespace Templates {
class TemplatesCore;
namespace Internal {

class TemplatesViewContext : public Core::IContext
{
public:
    TemplatesViewContext(Templates::TemplatesView *parent) : Core::IContext(parent)
    {
        setObjectName("TemplatesViewContext");
        setWidget(parent);
    }
};

class TemplatesViewActionHandler : public QObject
{
    Q_OBJECT
public:
    TemplatesViewActionHandler(QObject *parent = 0);
    virtual ~TemplatesViewActionHandler() {}

    void setCurrentView(Templates::TemplatesView *view);


private Q_SLOTS:
    void addCategory();
    void removeItem();
    void editCurrentItem();
    void saveModel();
    void print();
    void lock();
    void databaseInformation();
    void templatesViewItemChanged();

private:
//    bool canMoveUp();
//    bool canMoveDown();
    void updateActions();

protected:
    QAction *aAdd;
    QAction *aRemove;
    QAction *aEdit;
    QAction *aPrint;
    QAction *aSave;
    QAction *aLocker;
    QAction *aDatabaseInfos;
    QPointer<Templates::TemplatesView> m_CurrentView;
    bool m_IsLocked;
};

class TemplatesViewManager : public TemplatesViewActionHandler
{
    Q_OBJECT
    friend class Templates::TemplatesCore;

protected:
    TemplatesViewManager(QObject *parent = 0);

public:
    ~TemplatesViewManager() {}

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);
};

}  // End namespace Internal
}  // End namespace Templates

#endif // TEMPLATEVIEW_P_H
