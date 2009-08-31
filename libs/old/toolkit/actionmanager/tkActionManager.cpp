/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
 *   This code is inspired of the QtCreator project from Nokia             *
 *   QtCreator : Copyright (c) 2009 Nokia Corporation and/or               *
 *               its subsidiary(-ies).                                     *
 *               GNU Lesser General Public License 2.1 Usage               *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "tkActionManager_p.h"
#include "tkActionContainer_p.h"
#include "tkCommand_p.h"

#include <tkUniqueIdentifier.h>
#include <tkConstants.h>
#include <tkLog.h>

#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QShortcut>
#include <QMenuBar>
#include <QMainWindow>

namespace {
    enum { warnAboutFindFailures = 0 };
}

Q_TK_USING_CONSTANTS

tkActionManager* tkActionManager::m_Instance = 0;

tkActionManager *tkActionManager::instance(QMainWindow *win)
{
    if (!m_Instance) {
        m_Instance = new tkActionManager(win);
    }
    return m_Instance;
}

tkActionManager::tkActionManager(QMainWindow *win) : QObject(win), d(0)
{
    Q_ASSERT(win);
    d = new tkActionManagerPrivate(this, win);
}

tkActionManager::~tkActionManager()
{
    if (d) delete d;
    d=0;
}

tkActionManagerPrivate::tkActionManagerPrivate(QObject *parent, QMainWindow *mainWnd) :
        QObject(parent), m_mainWnd(mainWnd)
{
    Q_ASSERT(m_mainWnd);
    m_defaultGroups << tkUID->uniqueIdentifier(G_DEFAULT_ONE);
    m_defaultGroups << tkUID->uniqueIdentifier(G_DEFAULT_TWO);
    m_defaultGroups << tkUID->uniqueIdentifier(G_DEFAULT_THREE);
}

tkActionManagerPrivate::~tkActionManagerPrivate()
{
    qDeleteAll(m_idCmdMap.values());
    qDeleteAll(m_idContainerMap.values());
}

QList<int> tkActionManager::defaultGroups() const
{
    return d->m_defaultGroups;
}

QList<tkCommandPrivate *> tkActionManagerPrivate::commands() const
{
    return m_idCmdMap.values();
}

QList<tkActionContainerPrivate *> tkActionManagerPrivate::containers() const
{
    return m_idContainerMap.values();
}

bool tkActionManager::hasContext(int context) const
{
    return d->m_context.contains(context);
}

void tkActionManager::setContext(const QList<int> &context)
{
    // here are possibilities for speed optimization if necessary:
    // let commands (de-)register themselves for contexts
    // and only update commands that are either in old or new contexts
    d->m_context = context;
//    const IdCmdMap::const_iterator cmdcend = m_idCmdMap.constEnd();
//    for (IdCmdMap::const_iterator it = m_idCmdMap.constBegin(); it != cmdcend; ++it)
//        it.value()->setCurrentContext(m_context);
    foreach(tkCommandPrivate *c, d->m_idCmdMap)
        c->setCurrentContext(d->m_context);

//    const IdContainerMap::const_iterator acend = m_idContainerMap.constEnd();
//    for (IdContainerMap::const_iterator it = m_idContainerMap.constBegin(); it != acend; ++it)
//        it.value()->update();
    foreach(tkActionContainerPrivate *a, d->m_idContainerMap)
        a->update();
}

bool tkActionManagerPrivate::hasContext(QList<int> context) const
{
    for (int i=0; i<m_context.count(); ++i) {
        if (context.contains(m_context.at(i)))
            return true;
    }
    return false;
}

tkActionContainer *tkActionManager::createMenu(const QString &id)
{
    const int uid = tkUID->uniqueIdentifier(id);
    const tkActionManagerPrivate::IdContainerMap::const_iterator it = d->m_idContainerMap.constFind(uid);
    if (it !=  d->m_idContainerMap.constEnd())
        return it.value();

    QMenu *m = new QMenu(d->m_mainWnd);
    m->setObjectName(id);

    tkMenuActionContainer *mc = new tkMenuActionContainer(uid);
    mc->setMenu(m);

    d->m_idContainerMap.insert(uid, mc);

    return mc;
}

tkActionContainer *tkActionManager::createMenuBar(const QString &id)
{
    const int uid = tkUID->uniqueIdentifier(id);
    const tkActionManagerPrivate::IdContainerMap::const_iterator it = d->m_idContainerMap.constFind(uid);
    if (it !=  d->m_idContainerMap.constEnd())
        return it.value();

    QMenuBar *mb = new QMenuBar; // No parent (System menu bar on Mac OS X)
    mb->setObjectName(id);

    tkMenuBarActionContainer *mbc = new tkMenuBarActionContainer(uid);
    mbc->setMenuBar(mb);

    d->m_idContainerMap.insert(uid, mbc);

    return mbc;
}

tkCommand *tkActionManager::registerAction(QAction *action, const QString &id, const QList<int> &context)
{
    tkOverrideableAction *a = 0;
    tkCommand *c = d->registerOverridableAction(action, id, false);
    a = static_cast<tkOverrideableAction *>(c);
    if (a)
        a->addOverrideAction(action, context);
    return a;
}

tkCommand *tkActionManagerPrivate::registerOverridableAction(QAction *action, const QString &id, bool checkUnique)
{
    tkOverrideableAction *a = 0;
    const int uid = tkUID->uniqueIdentifier(id);
    if (tkCommandPrivate *c = m_idCmdMap.value(uid, 0)) {
        a = qobject_cast<tkOverrideableAction *>(c);
        if (!a) {
            qWarning() << "registerAction: id" << id << "is registered with a different command type.";
            return c;
        }
    } else {
        a = new tkOverrideableAction(uid);
        m_idCmdMap.insert(uid, a);
    }

    if (!a->action()) {
        QAction *baseAction = new QAction(m_mainWnd);
        baseAction->setObjectName("baseaction"+id);
        baseAction->setCheckable(action->isCheckable());
        baseAction->setIcon(action->icon());
        baseAction->setIconText(action->iconText());
        baseAction->setText(action->text());
        baseAction->setToolTip(action->toolTip());
        baseAction->setStatusTip(action->statusTip());
        baseAction->setWhatsThis(action->whatsThis());
        baseAction->setChecked(action->isChecked());
        baseAction->setCheckable(action->isCheckable());
        baseAction->setSeparator(action->isSeparator());
        baseAction->setShortcutContext(Qt::ApplicationShortcut);
        baseAction->setEnabled(false);
        baseAction->setObjectName(id);
        baseAction->setParent(m_mainWnd);
        a->setAction(baseAction);
        m_mainWnd->addAction(baseAction);
        a->setKeySequence(a->keySequence());
        a->setDefaultKeySequence(QKeySequence());
    } else  if (checkUnique) {
        qWarning() << "registerOverridableAction: id" << id << "is already registered.";
    }

    return a;
}

tkCommand *tkActionManager::registerShortcut(QShortcut *shortcut, const QString &id, const QList<int> &context)
{
    tkShortcut *sc = 0;
    int uid = tkUID->uniqueIdentifier(id);
    if (tkCommandPrivate *c = d->m_idCmdMap.value(uid, 0)) {
        sc = qobject_cast<tkShortcut *>(c);
        if (!sc) {
            qWarning() << "registerShortcut: id" << id << "is registered with a different command type.";
            return c;
        }
    } else {
        sc = new tkShortcut(uid);
        d->m_idCmdMap.insert(uid, sc);
    }

    if (sc->shortcut()) {
        qWarning() << "registerShortcut: action already registered (id" << id << ".";
        return sc;
    }

    if (!d->hasContext(context))
        shortcut->setEnabled(false);
    shortcut->setObjectName(id);
    shortcut->setParent(d->m_mainWnd);
    sc->setShortcut(shortcut);

    if (context.isEmpty())
        sc->setContext(QList<int>() << 0);
    else
        sc->setContext(context);

    sc->setKeySequence(shortcut->key());
    sc->setDefaultKeySequence(QKeySequence());

    return sc;
}

tkCommand *tkActionManager::command(const QString &id) const
{
    const int uid = tkUID->uniqueIdentifier(id);
    const tkActionManagerPrivate::IdCmdMap::const_iterator it = d->m_idCmdMap.constFind(uid);
    if (it == d->m_idCmdMap.constEnd()) {
        if (warnAboutFindFailures)
            qWarning() << "tkActionManagerPrivate::command(): failed to find :" << id << '/' << uid;
        return 0;
    }
    return it.value();
}

tkActionContainer *tkActionManager::actionContainer(const QString &id) const
{
    const int uid = tkUID->uniqueIdentifier(id);
    const tkActionManagerPrivate::IdContainerMap::const_iterator it = d->m_idContainerMap.constFind(uid);
    if (it == d->m_idContainerMap.constEnd()) {
        if (warnAboutFindFailures)
            qWarning() << "tkActionManagerPrivate::actionContainer(): failed to find :" << id << '/' << uid;
        return 0;
    }
    return it.value();
}

tkCommand *tkActionManager::command(int uid) const
{
    const tkActionManagerPrivate::IdCmdMap::const_iterator it = d->m_idCmdMap.constFind(uid);
    if (it == d->m_idCmdMap.constEnd()) {
        if (warnAboutFindFailures)
            qWarning() << "tkActionManagerPrivate::command(): failed to find :" <<  tkUID->stringForUniqueIdentifier(uid) << '/' << uid;
        return 0;
    }
    return it.value();
}

tkActionContainer *tkActionManager::actionContainer(int uid) const
{
    const tkActionManagerPrivate::IdContainerMap::const_iterator it = d->m_idContainerMap.constFind(uid);
    if (it == d->m_idContainerMap.constEnd()) {
        if (warnAboutFindFailures)
            qWarning() << "tkActionManagerPrivate::actionContainer(): failed to find :" << tkUID->stringForUniqueIdentifier(uid) << uid;
        return 0;
    }
    return it.value();
}

void tkActionManager::retranslateMenusAndActions()
{
    foreach( tkCommandPrivate *c, d->m_idCmdMap.values())
        c->retranslate();
    foreach(tkActionContainerPrivate *c, d->m_idContainerMap.values())
        c->retranslate();
}


static const char *settingsGroup = "KeyBindings";
static const char *idKey = "ID";
static const char *sequenceKey = "Keysequence";

void tkActionManagerPrivate::initialize()
{
//    QSettings *settings = m_mainWnd->settings();
//    const int shortcuts = settings->beginReadArray(QLatin1String(settingsGroup));
//    for (int i=0; i<shortcuts; ++i) {
//        settings->setArrayIndex(i);
//        const QString sid = settings->value(QLatin1String(idKey)).toString();
//        const QKeySequence key(settings->value(QLatin1String(sequenceKey)).toString());
//        const int id = tkUID->uniqueIdentifier(sid);
//
//        tkCommand *cmd = command(id);
//        if (cmd)
//            cmd->setKeySequence(key);
//    }
//    settings->endArray();
}

void tkActionManagerPrivate::saveSettings(QSettings *settings)
{
//    settings->beginWriteArray(QLatin1String(settingsGroup));
//    int count = 0;
//
//    const IdCmdMap::const_iterator cmdcend = m_idCmdMap.constEnd();
//    for (IdCmdMap::const_iterator j = m_idCmdMap.constBegin(); j != cmdcend; ++j) {
//        const int id = j.key();
//        tkCommandPrivate *cmd = j.value();
//        QKeySequence key = cmd->keySequence();
//        if (key != cmd->defaultKeySequence()) {
//            const QString sid = tkUID->stringForUniqueIdentifier(id);
//            settings->setArrayIndex(count);
//            settings->setValue(QLatin1String(idKey), sid);
//            settings->setValue(QLatin1String(sequenceKey), key.toString());
//            count++;
//        }
//    }
//
//    settings->endArray();
}
