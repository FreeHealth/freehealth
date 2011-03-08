/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef TEMPLATEVIEW_P_H
#define TEMPLATEVIEW_P_H

#include <templatesplugin/templatesview.h>

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QPointer>
#include <QDebug>

namespace Templates {
namespace Internal {
class TemplatesViewContext : public Core::IContext
{
public:
    TemplatesViewContext(Templates::TemplatesView *parent) : Core::IContext(parent), w(parent)
    {
        setObjectName("TemplatesViewContext");
    }

    void addContext(int uid)
    {
        if (!m_Context.contains(uid))
            m_Context.append(uid);
    }
    void clearContext() { m_Context.clear(); }

    QList<int> context() const { return m_Context; }
    QWidget *widget()          { return w; }

private:
    Templates::TemplatesView *w;
    QList<int> m_Context;
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
    void databaseInformations();
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
public:
    static TemplatesViewManager *instance(QObject *parent);
    ~TemplatesViewManager() {}

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    TemplatesViewManager(QObject *parent = 0);
    static TemplatesViewManager *m_Instance;
};

}  // End namespace Internal
}  // End namespace Templates

#endif // TEMPLATEVIEW_P_H
