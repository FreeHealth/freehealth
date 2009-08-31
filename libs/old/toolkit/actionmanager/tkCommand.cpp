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
#include "tkCommand_p.h"

#include <QAction>
#include <QShortcut>
#include <QApplication>

#include <tkLog.h>
#include <tkConstantTranslations.h>


tkCommandPrivate::tkCommandPrivate(int id)
    : m_attributes(0), m_id(id)
{
}

void tkCommandPrivate::setDefaultKeySequence(const QKeySequence &key)
{
    m_defaultKey = key;
}

QKeySequence tkCommandPrivate::defaultKeySequence() const
{
    return m_defaultKey;
}

int tkCommandPrivate::id() const
{
    return m_id;
}

QAction *tkCommandPrivate::action() const
{
    return 0;
}

QShortcut *tkCommandPrivate::shortcut() const
{
    return 0;
}

void tkCommandPrivate::setAttribute(CommandAttribute attr)
{
    m_attributes |= attr;
}

void tkCommandPrivate::removeAttribute(CommandAttribute attr)
{
    m_attributes &= ~attr;
}

bool tkCommandPrivate::hasAttribute(CommandAttribute attr) const
{
    return (m_attributes & attr);
}

QString tkCommandPrivate::stringWithAppendedShortcut(const QString &str) const
{
    if (keySequence().isEmpty())
        return str;
    else
        return QString("%1 <span style=\"color: darkblue; font-size: small\">%2</span>").arg(str).arg(
            keySequence().toString(QKeySequence::NativeText));
}

// ---------- tkShortcut ------------

/*!
    \class tkShortcut
    \internal
*/

tkShortcut::tkShortcut(int id)
    : tkCommandPrivate(id), m_shortcut(0)
{

}

QString tkShortcut::name() const
{
    if (!m_shortcut)
        return QString();

    return m_shortcut->whatsThis();
}

void tkShortcut::setShortcut(QShortcut *shortcut)
{
    m_shortcut = shortcut;
}

QShortcut *tkShortcut::shortcut() const
{
    return m_shortcut;
}

void tkShortcut::setContext(const QList<int> &context)
{
    m_context = context;
}

QList<int> tkShortcut::context() const
{
    return m_context;
}

void tkShortcut::setDefaultKeySequence(const QKeySequence &key)
{
    setKeySequence(key);
    tkCommandPrivate::setDefaultKeySequence(key);
}

void tkShortcut::setKeySequence(const QKeySequence &key)
{
    m_shortcut->setKey(key);
    emit keySequenceChanged();
}

QKeySequence tkShortcut::keySequence() const
{
    return m_shortcut->key();
}

bool tkShortcut::setCurrentContext(const QList<int> &context)
{
    foreach (int ctxt, m_context) {
        if (context.contains(ctxt)) {
            m_shortcut->setEnabled(true);
            return true;
        }
    }
    m_shortcut->setEnabled(false);
    return false;
}

bool tkShortcut::isActive() const
{
    return m_shortcut->isEnabled();
}

// ---------- tkAction ------------

/*!
  \class tkAction
  \internal
*/
tkAction::tkAction(int id)
    : tkCommandPrivate(id), m_action(0)
{

}

QString tkAction::name() const
{
    if (!m_action)
        return QString();

    return m_action->text();
}

void tkAction::setAction(QAction *action)
{
    m_action = action;
    if (m_action) {
        m_action->setParent(this);
        m_toolTip = m_action->toolTip();
    }
}

QAction *tkAction::action() const
{
    return m_action;
}

void tkAction::setLocations(const QList<CommandLocation> &locations)
{
    m_locations = locations;
}

QList<CommandLocation> tkAction::locations() const
{
    return m_locations;
}

void tkAction::setDefaultKeySequence(const QKeySequence &key)
{
    setKeySequence(key);
    tkCommandPrivate::setDefaultKeySequence(key);
}

void tkAction::setKeySequence(const QKeySequence &key)
{
    m_action->setShortcut(key);
    updateToolTipWithKeySequence();
    emit keySequenceChanged();
}

void tkAction::updateToolTipWithKeySequence()
{
    if (m_action->shortcut().isEmpty())
        m_action->setToolTip(m_toolTip);
    else
        m_action->setToolTip(stringWithAppendedShortcut(m_toolTip));
}

QKeySequence tkAction::keySequence() const
{
    return m_action->shortcut();
}

// ---------- tkOverrideableAction ------------

/*!
    \class tkOverrideableAction
    \internal
*/

tkOverrideableAction::tkOverrideableAction(int id)
    : tkAction(id), m_currentAction(0), m_active(false),
    m_contextInitialized(false)
{
}

//void tkOverrideableAction::setAction(QAction *tkAction)
//{
//    tkAction::settkAction(tkAction);
//}

bool tkOverrideableAction::setCurrentContext(const QList<int> &context)
{
    m_context = context;

    QAction *oldAction = m_currentAction;
    m_currentAction = 0;
    for (int i = 0; i < m_context.size(); ++i) {
        if (QAction *a = m_contextActionMap.value(m_context.at(i), 0)) {
            m_currentAction = a;
            break;
        }
    }

    if (m_currentAction == oldAction && m_contextInitialized)
        return true;
    m_contextInitialized = true;

    if (oldAction) {
        disconnect(oldAction, SIGNAL(changed()), this, SLOT(actionChanged()));
        disconnect(m_action, SIGNAL(triggered(bool)), oldAction, SIGNAL(triggered(bool)));
        disconnect(m_action, SIGNAL(toggled(bool)), oldAction, SLOT(setChecked(bool)));
    }
    if (m_currentAction) {
        connect(m_currentAction, SIGNAL(changed()), this, SLOT(actionChanged()));
        // we want to avoid the toggling semantic on slot trigger(), so we just connect the signals
        connect(m_action, SIGNAL(triggered(bool)), m_currentAction, SIGNAL(triggered(bool)));
        // we need to update the checked state, so we connect to setChecked slot, which also fires a toggled signal
        connect(m_action, SIGNAL(toggled(bool)), m_currentAction, SLOT(setChecked(bool)));
        actionChanged();
        m_active = true;
        return true;
    }
    if (hasAttribute(CA_Hide))
        m_action->setVisible(false);
    m_action->setEnabled(false);
    m_active = false;
    return false;
}

void tkOverrideableAction::addOverrideAction(QAction *action, const QList<int> &context)
{
    if (context.isEmpty()) {
        m_contextActionMap.insert(0, action);
    } else {
        for (int i=0; i<context.size(); ++i) {
            int k = context.at(i);
            if (m_contextActionMap.contains(k))
                qWarning() << QString("addOverrideAction: action already registered for context %1 when registering '%2'").arg(k).arg(action->text() + " " +action->objectName());
            m_contextActionMap.insert(k, action);
        }
    }
}

void tkOverrideableAction::actionChanged()
{
    if (hasAttribute(CA_UpdateIcon)) {
        m_action->setIcon(m_currentAction->icon());
        m_action->setIconText(m_currentAction->iconText());
    }
    if (hasAttribute(CA_UpdateText)) {
        m_action->setText(m_currentAction->text());
        m_toolTip = m_currentAction->toolTip();
        updateToolTipWithKeySequence();
        m_action->setStatusTip(m_currentAction->statusTip());
        m_action->setWhatsThis(m_currentAction->whatsThis());
    }

    bool block = m_action->blockSignals(true);
    m_action->setChecked(m_currentAction->isChecked());
    m_action->setCheckable(m_currentAction->isCheckable());
    m_action->blockSignals(block);

    m_action->setEnabled(m_currentAction->isEnabled());
    m_action->setVisible(m_currentAction->isVisible());
}

bool tkOverrideableAction::isActive() const
{
    return m_active;
}

void tkOverrideableAction::retranslate()
{
    QAction *a;
    if (m_currentAction) {
        a = m_currentAction;
        bool block = a->blockSignals(true);
        if (!m_unTrText.isEmpty())
            a->setText(QCoreApplication::translate(m_TrContext.toAscii(), m_unTrText.toAscii()));
        if (!m_unTrTooltip.isEmpty())
            a->setToolTip(stringWithAppendedShortcut(QCoreApplication::translate(m_TrContext.toAscii(), m_unTrTooltip.toAscii())));
        else
            a->setToolTip(stringWithAppendedShortcut(a->text()));
        a->blockSignals(block);
    }
    if (m_action) {
        a = m_action;
        bool block = a->blockSignals(true);
        if (!m_unTrText.isEmpty())
            a->setText(QCoreApplication::translate(m_TrContext.toAscii(), m_unTrText.toAscii()));
        if (!m_unTrTooltip.isEmpty())
            a->setToolTip(stringWithAppendedShortcut(QCoreApplication::translate(m_TrContext.toAscii(), m_unTrTooltip.toAscii())));
        else
            a->setToolTip(stringWithAppendedShortcut(a->text()));
        a->blockSignals(block);
    }
}
