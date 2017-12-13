/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEMEDFORMS_MAINWINDOW_H
#define FREEMEDFORMS_MAINWINDOW_H

#include <fmfmainwindowplugin/mainwindow_exporter.h>
#include <coreplugin/imainwindow.h>
#include <usermanagerplugin/iuserlistener.h>

#include <QCloseEvent>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER
 * \version 0.8.4
 * \date 12 Apr 2013
*/

namespace Form {
class IFormIO;
}

namespace Utils {
class FancyTabWidget;
}

namespace Core {
class FileManager;
class IMode;
}

namespace MainWin {
class MainWindow;

namespace Internal {

class MainWindowUserListener : public UserPlugin::IUserListener
{
    Q_OBJECT
public:
    explicit MainWindowUserListener(MainWindow *parent);
    virtual ~MainWindowUserListener();

    virtual bool userAboutToChange();
    virtual bool currentUserAboutToDisconnect();

private:
    MainWindow *m_MainWindow;
};

}  // End Internal

class MAINWIN_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();

    // IMainWindow Interface
    bool initialize(const QStringList &, QString *) {return true;}
    void extensionsInitialized();

    void refreshPatient() const;
    void readSettings();
    void writeSettings();
    QStatusBar *statusBar();

private Q_SLOTS:
    void postCoreInitialization();
    void onCurrentModeChanged(Core::IMode *newMode);
    void onCurrentUserChanged();
    void onUserDataChanged(int);
    void onCurrentPatientChanged();

    // Interface of MainWindowActionHandler
    bool openFile();
    bool loadFile(const QString &absDirPath);

    bool createNewPatient();
    bool viewPatientIdentity();
    bool removePatient();

    bool applicationPreferences();
    bool applicationConfiguratorWizard();

    void updateCheckerEnd(bool error);

    void openPatientFormsFile();
    void aboutToShowRecentFiles();
    void openRecentFile();

protected:
    void manageIModeEnabledState();
    void closeEvent( QCloseEvent *event );
    void changeEvent(QEvent *event);

public:
    Utils::FancyTabWidget *m_modeStack;
    bool m_HelpTextShow;
    uint m_AutomaticSaveInterval;   /*!< Interval between each automatic save in SECONDS */
    int  m_TimerId;
    bool m_OpenLastOpenedForm;
    QByteArray windowState;
    Internal::MainWindowUserListener *m_UserListener;
};

} // End Core

#endif  // FREEMEDFORMS_MAINWINDOW_H
