/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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
