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

#ifndef ACTIONMANAGERPRIVATE_H
#define ACTIONMANAGERPRIVATE_H

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command_p.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QMap>
#include <QHash>
#include <QMultiHash>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QLabel;
class QSettings;
QT_END_NAMESPACE

namespace Core {
class IMainWindow;
namespace Internal {

class ActionContainerPrivate;
class CommandPrivate;

class ActionManagerPrivate : public Core::ActionManager
{
    Q_OBJECT

public:
    explicit ActionManagerPrivate(Core::IMainWindow *mainWnd);
    ~ActionManagerPrivate();

    static ActionManagerPrivate *instance();

    void initialize();

    void setContext(const Context &context);
    bool hasContext(int context) const;

    void saveSettings(QSettings *settings);

    QList<Command *> commands() const;

    Command *command(const Id &id) const;
    ActionContainer *actionContainer(const Id &id) const;
    ActionContainer *createMenu(const Id &id);
    ActionContainer *createMenuBar(const Id &id);

    Command *registerAction(QAction *action, const Id &id,
        const Context &context, bool scriptable = false);
    Command *registerShortcut(QShortcut *shortcut, const Id &id,
        const Context &context, bool scriptable = false);

    void unregisterAction(QAction *action, const Id &id);
    void unregisterShortcut(const Id &id);

    void setPresentationModeEnabled(bool enabled);
    bool isPresentationModeEnabled();

    void retranslateMenusAndActions();

private slots:
    void containerDestroyed();

    void actionTriggered();
    void shortcutTriggered();
private:
    void showShortcutPopup(const QString &shortcut);
    bool hasContext(const Context &context) const;
    Action *overridableAction(const Id &id);

    static ActionManagerPrivate *m_instance;

    typedef QHash<Core::Id, CommandPrivate *> IdCmdMap;
    IdCmdMap m_idCmdMap;

    typedef QHash<Core::Id, ActionContainerPrivate *> IdContainerMap;
    IdContainerMap m_idContainerMap;

//    typedef QMap<int, int> GlobalGroupMap;
//    GlobalGroupMap m_globalgroups;
//
    Context m_context;

    IMainWindow *m_mainWnd;

    QLabel *m_presentationLabel;
    QTimer m_presentationLabelTimer;
};

} // namespace Internal
} // namespace Core

#endif // ACTIONMANAGERPRIVATE_H
