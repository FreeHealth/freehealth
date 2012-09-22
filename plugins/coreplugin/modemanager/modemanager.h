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

#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include <QObject>
#include <coreplugin/core_exporter.h>
#include <coreplugin/imainwindow.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace Utils {
class FancyTabWidget;
}

namespace Core {
class IMode;

class CORE_EXPORT ModeManager : public QObject
{
    Q_OBJECT

public:
    explicit ModeManager(Core::IMainWindow *mainWindow);
    virtual ~ModeManager();

    static void init(Utils::FancyTabWidget *modeStack);
    static ModeManager *instance();

    static IMode *currentMode();
    static IMode *mode(const QString &id);

    static void addAction(QAction *action, int priority);
    static void addProjectSelector(QAction *action);
    static void addWidget(QWidget *widget);

    static void activateModeType(const QString &type);
    static void setModeBarHidden(bool hidden);
    static void activateMode(const QString &id);
    static void setFocusToCurrentMode();

signals:
    void currentModeAboutToChange(Core::IMode *mode);

    // the default argument '=0' is important for connects without the oldMode argument.
    void currentModeChanged(Core::IMode *mode, Core::IMode *oldMode = 0);

private slots:
    void slotActivateMode(const QString &id);
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);
    void currentTabAboutToChange(int index);
    void currentTabChanged(int index);
    void updateModeToolTip();
    void enabledStateChanged();
};

} // namespace Core

#endif // MODEMANAGER_H
