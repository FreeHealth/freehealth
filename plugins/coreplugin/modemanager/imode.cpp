/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

/**
 * \fn bool Core::IMode::setEnabledOnlyWithCurrentPatient(bool enablewithcurrentpatient)
 * If set to true, the mode will be enabled only when a current patient is defined and
 * disabled if no current patient is defined. By default, set to false (always enabled).
 */

/**
 * \fn void Core::IMode::setPatientBarVisibility(bool visible)
 * Define is the patient bar should be visible or not inside this mode. By default, set to true.
 * If you set to false, the patient bar will not be presented in the UI of the mode.
 * This property is managed by the main window
 * \sa Core::IPatientBar, Core::IPatient
 */

#include "imode.h"

using namespace Core;
IMode::IMode(QObject *parent)
    : IContext(parent),
    m_isEnabled(true),
    m_isPatientBarVisible(true),
    m_onlyWithCurrentPatient(false)
{
}

void IMode::setEnabled(bool enabled)
{
    if (m_isEnabled == enabled)
        return;
    m_isEnabled = enabled;
    Q_EMIT enabledStateChanged(m_isEnabled);
}

bool IMode::isEnabled() const
{
    return m_isEnabled;
}
