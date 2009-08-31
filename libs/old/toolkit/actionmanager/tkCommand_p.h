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
#ifndef TKCOMMAND_P_H
#define TKCOMMAND_P_H

#include "tkCommand.h"
#include "tkActionManager_p.h"
#include <tkConstants.h>

#include <QList>
#include <QMultiMap>
#include <QPointer>
#include <QKeySequence>

#include <QDebug>

class tkCommandPrivate : public tkCommand
{
    Q_OBJECT
public:
    tkCommandPrivate(int id);
    virtual ~tkCommandPrivate() {}

    virtual QString name() const = 0;

    void setDefaultKeySequence(const QKeySequence &key);
    QKeySequence defaultKeySequence() const;


    int id() const;

    QAction *action() const;
    QShortcut *shortcut() const;

    void setAttribute(CommandAttribute attr);
    void removeAttribute(CommandAttribute attr);
    bool hasAttribute(CommandAttribute attr) const;

    virtual bool setCurrentContext(const QList<int> &context) = 0;

    virtual void retranslate() = 0;
    void setTranslations(const QString &unTrText,
                         const QString &unTrTooltip = QString::null ,
                         const QString &trContext = QString::null )
    {
        m_unTrText = unTrText;
        if (unTrTooltip.isEmpty())
            m_unTrTooltip = unTrText;
        else
            m_unTrTooltip = unTrTooltip;
        if (trContext.isEmpty())
            m_TrContext = tkConstants::TK_CONSTANTS_CONTEXT;
        else
            m_TrContext = trContext;
    }

    QString stringWithAppendedShortcut(const QString &str) const;

protected:
    QString m_category;
    int m_attributes;
    int m_id;
    QKeySequence m_defaultKey;

    QString m_unTrText;
    QString m_unTrTooltip;
    QString m_TrContext;
};

class tkShortcut : public tkCommandPrivate
{
    Q_OBJECT
public:
    tkShortcut(int id);

    QString name() const;

    void setDefaultKeySequence(const QKeySequence &key);
    void setKeySequence(const QKeySequence &key);
    QKeySequence keySequence() const;

    void setShortcut(QShortcut *shortcut);
    QShortcut *shortcut() const;

    void setContext(const QList<int> &context);
    QList<int> context() const;
    bool setCurrentContext(const QList<int> &context);

    bool isActive() const;

    void retranslate() { qWarning() << "shortcut retranslate"; }

private:
    QList<int> m_context;
    QShortcut *m_shortcut;
};

class tkAction : public tkCommandPrivate
{
    Q_OBJECT
public:
    tkAction(int id);

    QString name() const;

    void setDefaultKeySequence(const QKeySequence &key);
    void setKeySequence(const QKeySequence &key);
    QKeySequence keySequence() const;

    virtual void setAction(QAction *action);
    QAction *action() const;

    void setLocations(const QList<CommandLocation> &locations);
    QList<CommandLocation> locations() const;

    virtual void retranslate() { qWarning() << "action translate" ; }

protected:
    void updateToolTipWithKeySequence();
    
    QAction *m_action;
    QList<CommandLocation> m_locations;
    QString m_toolTip;
};

class tkOverrideableAction : public tkAction
{
    Q_OBJECT

public:
    tkOverrideableAction(int id);

//    void setAction(QAction *action);
    bool setCurrentContext(const QList<int> &context);
    void addOverrideAction(QAction *action, const QList<int> &context);
    bool isActive() const;

    void retranslate();

private Q_SLOTS:
    void actionChanged();

private:
    QPointer<QAction> m_currentAction;
    QList<int> m_context;
    QMap<int, QPointer<QAction> > m_contextActionMap;
    bool m_active;
    bool m_contextInitialized;
};


#endif // TKCOMMAND_P_H
