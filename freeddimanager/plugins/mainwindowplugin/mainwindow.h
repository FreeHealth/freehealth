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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDDIMANAGER_PLUGIN_MAINWINDOW_H
#define FREEDDIMANAGER_PLUGIN_MAINWINDOW_H

#include <coreplugin/imainwindow.h>

/**
 * \file mainwindow.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 09 Oct 2014
*/

namespace Utils {
class FancyTabWidget;
}

namespace Core {
class IMode;
}

namespace MainWin {
class MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void refreshPatient();
    void readSettings();
    void writeSettings();

public Q_SLOTS:
    bool openFile();
    bool saveFile();
    bool saveAsFile();

    void aboutToShowRecentFiles();
    void openRecentFile();

    void onDatabaseReportRequested();
    void onBackupDatabaseRequested();
protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);

private:
    bool saveFileContent(const QString &file);
    void readFile(const QString &file);

private Q_SLOTS:
    void postCoreOpened();

public:
    Utils::FancyTabWidget *m_modeStack;
};

} // End MainWin

#endif  // FREEDDIMANAGER_PLUGIN_MAINWINDOW_H
