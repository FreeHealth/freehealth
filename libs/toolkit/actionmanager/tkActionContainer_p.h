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
#ifndef TKACTIONCONTAINER_P_H
#define TKACTIONCONTAINER_P_H

#include "tkActionManager_p.h"
#include "tkActionContainer.h"
#include "tkCommand.h"

#include <tkConstants.h>

#include <QCoreApplication>
#include <QDebug>

class tkActionContainerPrivate : public tkActionContainer
{
public:
    tkActionContainerPrivate(int id);
    virtual ~tkActionContainerPrivate() {}

    void setEmptyAction(EmptyAction ea);
    bool hasEmptyAction(EmptyAction ea) const;

    QAction *insertLocation(const QString &group) const;
    void appendGroup(const QString &group);
    void addAction(tkCommand *action, const QString &group = QString());
    void addMenu(tkActionContainer *menu, const QString &group = QString());

    int id() const;

    QMenu *menu() const;
    QMenuBar *menuBar() const;

    virtual void insertAction(QAction *before, QAction *action) = 0;
    virtual void insertMenu(QAction *before, QMenu *menu) = 0;

    QList<tkCommand *> commands() const { return m_commands; }
    QList<tkActionContainer *> subContainers() const { return m_subContainers; }

    void setTranslations(const QString &unTrTitle, const QString &trContext = QString::null)
    {
        m_unTrTitle = unTrTitle;
        if (trContext.isEmpty())
            m_TrContext = tkConstants::TK_CONSTANTS_CONTEXT;
        else
            m_TrContext=trContext;
    }
    virtual void retranslate() = 0;


protected:
    bool canAddAction(tkCommand *action) const;
    bool canAddMenu(tkActionContainer *menu) const;
    virtual bool canBeAddedToMenu() const = 0;

    void addAction(tkCommand *action, int pos, bool setpos);
    void addMenu(tkActionContainer *menu, int pos, bool setpos);

protected:
    QString m_unTrTitle, m_TrContext;

private:
    QAction *beforeAction(int pos, int *prevKey) const;
    int calcPosition(int pos, int prevKey) const;

    QList<int> m_groups;
    int m_data;
    int m_id;
    QMap<int, int> m_posmap;
    QList<tkActionContainer *> m_subContainers;

protected:
    QList<tkCommand *> m_commands;
};

class tkMenuActionContainer : public tkActionContainerPrivate
{
public:
    tkMenuActionContainer(int id);

    void setMenu(QMenu *menu);
    QMenu *menu() const;

    void setLocation(const CommandLocation &location);
    CommandLocation location() const;

    void insertAction(QAction *before, QAction *action);
    void insertMenu(QAction *before, QMenu *menu);
    bool update();

    void retranslate()
    {
        if (m_menu)
            m_menu->setTitle(QCoreApplication::translate(m_TrContext.toAscii(), m_unTrTitle.toAscii()));
        foreach(tkCommand *c, m_commands)
            c->retranslate();
    }

protected:
    bool canBeAddedToMenu() const;
private:
    QMenu *m_menu;
    CommandLocation m_location;
};

class tkMenuBarActionContainer : public tkActionContainerPrivate
{
public:
    tkMenuBarActionContainer(int id);

    void setMenuBar(QMenuBar *menuBar);
    QMenuBar *menuBar() const;

    void insertAction(QAction *before, QAction *action);
    void insertMenu(QAction *before, QMenu *menu);
    bool update();

    void retranslate() {}

protected:
    bool canBeAddedToMenu() const;
private:
    QMenuBar *m_menuBar;
};

#endif // TKACTIONCONTAINER_P_H
