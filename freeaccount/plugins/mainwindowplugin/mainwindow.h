/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEACCOUNT_MAINWINDOW_H
#define FREEACCOUNT_MAINWINDOW_H

#include <mainwindowplugin/mainwindow_exporter.h>
#include <coreplugin/imainwindow.h>

// include Qt headers
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 03 Oct 2010
*/

namespace MainWin {
namespace Internal {
namespace Ui {
class MainWindow;
}  // End Ui
}  // End Internal

class FACCOUNTMAINWIN_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void createDockWindows();
    void refreshPatient();
    void readSettings();
    void writeSettings();
    void createStatusBar();
    void changeFontTo(const QFont &font);


public Q_SLOTS: // Interface of MainWidowActionHandler
    bool newFile();
    bool openFile();
    void readFile(const QString &file);
    bool saveFile();
    bool saveAsFile();
    bool print();
    bool printPreview();

    bool applicationPreferences();
    bool configureMedintux();

    void updateCheckerEnd(bool);

    void aboutToShowRecentFiles();
    void openRecentFile();

    void userChanged();

protected:
    void closeEvent( QCloseEvent *event );
    void changeEvent(QEvent *event);

private Q_SLOTS:
    void postCoreOpened();

public:
    Internal::Ui::MainWindow *m_ui;
};

} // End Core

#endif  // FREEACCOUNT_MAINWINDOW_H
