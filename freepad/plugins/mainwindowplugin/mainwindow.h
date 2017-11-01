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
#ifndef FREEPAD_MAINWINDOW_H
#define FREEPAD_MAINWINDOW_H

#include <mainwindowplugin/mainwindow_exporter.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ipadtools.h>

#include <utils/global.h>

#include <QCloseEvent>
#include <QComboBox>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTreeWidgetItem;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 03 Jan 2011
*/

namespace Core {
class IPadWriter;
}

namespace PadTools {
namespace Internal {
class TokenModel;
class PadWriter;
}
}

namespace MainWin {
namespace Internal {
class MainWinPrivate;

namespace Ui {
class MainWindow;
}  // End Ui
}  // End Internal

class FREEPAD_MAINWIN_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void readSettings();
    void writeSettings();
    void createStatusBar();
    void changeFontTo(const QFont &font);


public Q_SLOTS:
    void postCoreInitialization();

    // Interface of MainWidowActionHandler
    bool newFile();
    bool openFile();
    void readFile(const QString &file);
    bool saveFile();
    bool saveAsFile();

    bool applicationPreferences();

    void updateCheckerEnd(bool error);

    void aboutToShowRecentFiles();
    void openRecentFile();

private slots:
	void padTextChanged();
    void tokenChanged(const QString &token, const QString &value);
	void tokenItemChanged(QTreeWidgetItem *item, int column);

protected:
    void closeEvent( QCloseEvent *event );
    void changeEvent(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

//public:
//    Internal::Ui::MainWindow *m_ui;

private:
    PadTools::Internal::TokenModel *m_TokenModel;
    Core::IPadWriter *m_Writer;

	void refreshTokens();
};

} // End Core

#endif  // FREEPAD_MAINWINDOW_H
