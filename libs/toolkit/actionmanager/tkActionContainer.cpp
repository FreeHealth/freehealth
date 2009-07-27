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
 *               GNU Lesser General Public License Usage                   *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "tkActionContainer_p.h"
#include "tkActionManager_p.h"
#include "tkCommand_p.h"

#include <tkUniqueIdentifier.h>
#include <tkConstants.h>
Q_TK_USING_CONSTANTS

#include <QDebug>
#include <QAction>
#include <QToolBar>
#include <QMenuBar>

Q_DECLARE_METATYPE(tkMenuActionContainer*)

/*!
    \class tkActionContainer
    \mainclass

    \brief The ActionContainer class represents a menu or menu bar in Qt Creator.

    You don't create instances of this class directly, but instead use the
    \l{ActionManager::createMenu()}
    and \l{ActionManager::createMenuBar()} methods.
    Retrieve existing action containers for an ID with
    \l{ActionManager::actionContainer()}.

    Within a menu or menu bar you can group menus and items together by defining groups
    (the order of the groups is defined by the order of the \l{ActionContainer::appendGroup()} calls), and
    adding menus/actions to these groups. If no custom groups are defined, an action container
    has three default groups \c{Core::Constants::G_DEFAULT_ONE}, \c{Core::Constants::G_DEFAULT_TWO}
    and \c{Core::Constants::G_DEFAULT_THREE}.

    You can define if the menu represented by this action container should automatically disable
    or hide whenever it only contains disabled items and submenus by setting the corresponding
    \l{tkActionContainer::setEmptyAction()}{EmptyAction}.
*/

/*!
    \enum tkActionContainer::EmptyAction
    Defines what happens when the represented menu is empty or contains only disabled/invisible items.
    \omitvalue EA_Mask
    \value EA_None
        The menu will still be visible and active.
    \value EA_Disable
        The menu will be visible but disabled.
    \value EA_Hide
        The menu will not be visible until the state of the subitems change.
*/

/*!
    \fn tkActionContainer::setEmptyAction(EmptyAction disableOrHide)
    Defines if the menu represented by this action container should automatically \a disableOrHide
    whenever it only contains disabled items and submenus.
    \sa ActionContainer::EmptyAction
*/

/*!
    \fn int tkActionContainer::id() const
    \internal
*/

/*!
    \fn QMenu *tkActionContainer::menu() const
    Returns the QMenu instance that is represented by this action container, or
    0 if this action container represents a menu bar.
*/

/*!
    \fn QMenuBar *tkActionContainer::menuBar() const
    Returns the QMenuBar instance that is represented by this action container, or
    0 if this action container represents a menu.
*/

/*!
    \fn QAction *tkActionContainer::insertLocation(const QString &group) const
    Returns an action representing the \a group,
    that could be used with \c{QWidget::insertAction}.
*/

/*!
    \fn void tkActionContainer::appendGroup(const QString &identifier)
    Adds a group with the given \a identifier to the action container. Using groups
    you can segment your action container into logical parts and add actions and
    menus directly to these parts.
    \sa addAction()
    \sa addMenu()
*/

/*!
    \fn void tkActionContainer::addAction(Core::Command *action, const QString &group)
    Add the \a action as a menu item to this action container. The action is added as the
    last item of the specified \a group.
    \sa appendGroup()
    \sa addMenu()
*/

/*!
    \fn void tkActionContainer::addMenu(Core::ActionContainer *menu, const QString &group)
    Add the \a menu as a submenu to this action container. The menu is added as the
    last item of the specified \a group.
    \sa appendGroup()
    \sa addAction()
*/

/*!
    \fn bool tkActionContainer::update()
    \internal
*/

/*!
    \fn tkActionContainer::~ActionContainer()
    \internal
*/

// ---------- tkActionContainerPrivate ------------

/*!
    \class Core::Internal::tkActionContainerPrivate
    \internal
*/

tkActionContainerPrivate::tkActionContainerPrivate(int id)
    : m_data(0), m_id(id)
{

}

void tkActionContainerPrivate::setEmptyAction(EmptyAction ea)
{
    m_data = ((m_data & ~EA_Mask) | ea);
}

bool tkActionContainerPrivate::hasEmptyAction(EmptyAction ea) const
{
    return (m_data & EA_Mask) == ea;
}

void tkActionContainerPrivate::appendGroup(const QString &group)
{
    int gid = tkUID->uniqueIdentifier(group);
    m_groups << gid;
}

QAction *tkActionContainerPrivate::insertLocation(const QString &group) const
{
    int grpid = tkUID->uniqueIdentifier(group);
    int prevKey = 0;
    int pos = ((grpid << 16) | 0xFFFF);
    return beforeAction(pos, &prevKey);
}

void tkActionContainerPrivate::addAction(tkCommand *action, const QString &group)
{
    if (!canAddAction(action))
        return;

    tkActionManager *am = tkActionManager::instance();
    int grpid = tkUID->uniqueIdentifier(G_DEFAULT_TWO);
    if (!group.isEmpty())
        grpid = tkUID->uniqueIdentifier(group);
    if (!m_groups.contains(grpid) && !am->defaultGroups().contains(grpid))
        qWarning() << "*** addAction(): Unknown group: " << group;
    int pos = ((grpid << 16) | 0xFFFF);
    addAction(action, pos, true);
}

void tkActionContainerPrivate::addMenu(tkActionContainer *menu, const QString &group)
{
    tkActionContainerPrivate *container = static_cast<tkActionContainerPrivate *>(menu);
    if (!container->canBeAddedToMenu())
        return;

    tkActionManager *am = tkActionManager::instance();
    int grpid = tkUID->uniqueIdentifier(G_DEFAULT_TWO);
    if (!group.isEmpty())
        grpid = tkUID->uniqueIdentifier(group);
    if (!m_groups.contains(grpid) && !am->defaultGroups().contains(grpid))
        qWarning() << "*** addMenu(): Unknown group: " << group;
    int pos = ((grpid << 16) | 0xFFFF);
    addMenu(menu, pos, true);
}

int tkActionContainerPrivate::id() const
{
    return m_id;
}

QMenu *tkActionContainerPrivate::menu() const
{
    return 0;
}

QMenuBar *tkActionContainerPrivate::menuBar() const
{
    return 0;
}

bool tkActionContainerPrivate::canAddAction(tkCommand *action) const
{
    return (action->action() != 0);
}

void tkActionContainerPrivate::addAction(tkCommand *action, int pos, bool setpos)
{
    tkAction *a = static_cast<tkAction *>(action);

    int prevKey = 0;
    QAction *ba = beforeAction(pos, &prevKey);

    if (setpos) {
        pos = calcPosition(pos, prevKey);
        CommandLocation loc;
        loc.m_container = m_id;
        loc.m_position = pos;
        QList<CommandLocation> locs = a->locations();
        locs.append(loc);
        a->setLocations(locs);
    }

    m_commands.append(action);
    m_posmap.insert(pos, action->id());
    insertAction(ba, a->action());
}

void tkActionContainerPrivate::addMenu(tkActionContainer *menu, int pos, bool setpos)
{
    tkMenuActionContainer *mc = static_cast<tkMenuActionContainer *>(menu);

    int prevKey = 0;
    QAction *ba = beforeAction(pos, &prevKey);

    if (setpos) {
        pos = calcPosition(pos, prevKey);
        CommandLocation loc;
        loc.m_container = m_id;
        loc.m_position = pos;
        mc->setLocation(loc);
    }

    m_subContainers.append(menu);
    m_posmap.insert(pos, menu->id());
    insertMenu(ba, mc->menu());
}

QAction *tkActionContainerPrivate::beforeAction(int pos, int *prevKey) const
{
    tkActionManager *am = tkActionManager::instance();

    int baId = -1;

    (*prevKey) = -1;

    QMap<int, int>::const_iterator i = m_posmap.constBegin();
    while (i != m_posmap.constEnd()) {
        if (i.key() > pos) {
            baId = i.value();
            break;
        }
        (*prevKey) = i.key();
        ++i;
    }

    if (baId == -1)
        return 0;

    if (tkCommand *cmd = am->command(baId))
        return cmd->action();
    if (tkActionContainer *container = am->actionContainer(baId))
        if (QMenu *menu = container->menu())
            return menu->menuAction();

    return 0;
}

int tkActionContainerPrivate::calcPosition(int pos, int prevKey) const
{
    int grp = (pos & 0xFFFF0000);
    if (prevKey == -1)
        return grp;

    int prevgrp = (prevKey & 0xFFFF0000);

    if (grp != prevgrp)
        return grp;

    return grp + (prevKey & 0xFFFF) + 10;
}

// ---------- MenuActionContainer ------------

/*!
    \class Core::Internal::MenuActionContainer
    \internal
*/

tkMenuActionContainer::tkMenuActionContainer(int id)
    : tkActionContainerPrivate(id), m_menu(0)
{
    setEmptyAction(EA_Disable);
}

void tkMenuActionContainer::setMenu(QMenu *menu)
{
    m_menu = menu;

    QVariant v;
    qVariantSetValue<tkMenuActionContainer*>(v, this);

    m_menu->menuAction()->setData(v);
}

QMenu *tkMenuActionContainer::menu() const
{
    return m_menu;
}

void tkMenuActionContainer::insertAction(QAction *before, QAction *action)
{
    m_menu->insertAction(before, action);
}

void tkMenuActionContainer::insertMenu(QAction *before, QMenu *menu)
{
    m_menu->insertMenu(before, menu);
}

void tkMenuActionContainer::setLocation(const CommandLocation &location)
{
    m_location = location;
}

CommandLocation tkMenuActionContainer::location() const
{
    return m_location;
}

bool tkMenuActionContainer::update()
{
    if (hasEmptyAction(EA_None))
        return true;

    bool hasitems = false;

    foreach (tkActionContainer *container, subContainers()) {
        if (container == this) {
            qWarning() << Q_FUNC_INFO << "container" << (this->menu() ? this->menu()->title() : "") <<  "contains itself as subcontainer";
            continue;
        }
        if (container->update()) {
            hasitems = true;
            break;
        }
    }
    if (!hasitems) {
        foreach (tkCommand *command, commands()) {
            if (command->isActive()) {
                hasitems = true;
                break;
            }
        }
    }
    if (!hasitems)
        qWarning() << "updating" << tkUID->stringForUniqueIdentifier(this->id()) << "no items --> hide = " << hasEmptyAction(EA_Hide) << "disable = "<< hasEmptyAction(EA_Disable);

    if (hasEmptyAction(EA_Hide))
        m_menu->setVisible(hasitems);
    else if (hasEmptyAction(EA_Disable))
        m_menu->setEnabled(hasitems);

    return hasitems;
}

bool tkMenuActionContainer::canBeAddedToMenu() const
{
    return true;
}


// ---------- MenuBarActionContainer ------------

/*!
    \class Core::Internal::MenuBarActionContainer
    \internal
*/

tkMenuBarActionContainer::tkMenuBarActionContainer(int id)
    : tkActionContainerPrivate(id), m_menuBar(0)
{
    setEmptyAction(EA_None);
}

void tkMenuBarActionContainer::setMenuBar(QMenuBar *menuBar)
{
    m_menuBar = menuBar;
}

QMenuBar *tkMenuBarActionContainer::menuBar() const
{
    return m_menuBar;
}

void tkMenuBarActionContainer::insertAction(QAction *before, QAction *action)
{
    m_menuBar->insertAction(before, action);
}

void tkMenuBarActionContainer::insertMenu(QAction *before, QMenu *menu)
{
    m_menuBar->insertMenu(before, menu);
}

bool tkMenuBarActionContainer::update()
{
    if (hasEmptyAction(EA_None))
        return true;

    bool hasitems = false;
    QList<QAction *> actions = m_menuBar->actions();
    for (int i=0; i<actions.size(); ++i) {
        if (actions.at(i)->isVisible()) {
            hasitems = true;
            break;
        }
    }

    if (hasEmptyAction(EA_Hide))
        m_menuBar->setVisible(hasitems);
    else if (hasEmptyAction(EA_Disable))
        m_menuBar->setEnabled(hasitems);

    return hasitems;
}

bool tkMenuBarActionContainer::canBeAddedToMenu() const
{
    return false;
}

