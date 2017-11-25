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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDIAMS_MAINWINDOW_H
#define FREEDIAMS_MAINWINDOW_H

#include <mainwindowplugin/mainwindow_exporter.h>
#include <coreplugin/imainwindow.h>

#include <utils/global.h>

#include <QCloseEvent>
#include <QComboBox>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.1
 * \date 21 Oct 2010
*/

namespace MainWin {
namespace Internal {
class MainWinPrivate;

namespace Ui {
class MainWindow;
}  // End Ui
}  // End Internal

class FDMAINWIN_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    enum PatientLabels {
        PatientSurnameLabel = 0,
        PatientFirstnameLabel,
        PatientNameMax
    };

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void createDockWindows();
    void readSettings();
    void writeSettings();
    void createStatusBar();
    bool savePrescription(const QString &fileName = QString::null);
    void changeFontTo(const QFont &font);


public Q_SLOTS:
    void postCoreInitialization();
    void refreshPatient();
    void updateIconBadgeOnMacOs();

    void changePatientNameLabelOrder(const int first);

    // Interface of MainWidowActionHandler
    bool newFile();
    bool openFile();
    void readFile(const QString &file);
    bool saveFile();
    bool saveAsFile();
    bool print();
    bool printPreview();

    bool configureMedintux();

    void updateCheckerEnd(bool error);

    void aboutToShowRecentFiles();
    void openRecentFile();

    void clearPatientInfos();
    void togglePrecautions();

protected:
    void closeEvent( QCloseEvent *event );
    void changeEvent(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

public:
    QAction *aClearPatient;
    Internal::Ui::MainWindow *m_ui;
    Internal::MainWinPrivate *d;
};

} // End Core

#endif  // FREEDIAMS_MAINWINDOW_H
