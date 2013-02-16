/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/
/***************************************************************************
 *   Adaptation to FreeMedForms : Eric MAEKER, <eric.maeker@gmail.com>     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

#ifndef IMODE_H
#define IMODE_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QIcon>

namespace Core {

class CORE_EXPORT IMode : public IContext
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

public:
    IMode(QObject *parent = 0);

    QString displayName() const { return m_displayName; }
    QIcon icon() const { return m_icon; }
    int priority() const { return m_priority; }
    QString id() const { return m_id; }
    QString type() const { return m_type; }
    bool isEnabled() const;
    bool patientBarVisibility() const {return m_isPatientBarVisible;}
    bool isEnabledOnlyWithCurrentPatient() const {return m_onlyWithCurrentPatient;}

    void setEnabled(bool enabled);
    void setDisplayName(const QString &displayName) { m_displayName = displayName; }
    void setIcon(const QIcon &icon) { m_icon = icon; }
    void setPriority(int priority) { m_priority = priority; }
    void setId(const QString &id) { m_id = id; }
    void setType(const QString &type) { m_type = type; }

    void setPatientBarVisibility(bool visible) {m_isPatientBarVisible=visible;}
    void setEnabledOnlyWithCurrentPatient(bool enablewithcurrentpatient) {m_onlyWithCurrentPatient = enablewithcurrentpatient;}

Q_SIGNALS:
    void enabledStateChanged(bool enabled);

private:
    QString m_displayName;
    QIcon m_icon;
    int m_priority;
    QString m_id;
    QString m_type;
    bool m_isEnabled, m_isPatientBarVisible, m_onlyWithCurrentPatient;
};

} // namespace Core

#endif // IMODE_H
