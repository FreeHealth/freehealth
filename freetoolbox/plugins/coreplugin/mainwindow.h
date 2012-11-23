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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <coreplugin/imainwindow.h>

#include <QSettings>
#include <QFutureWatcher>
class QProgressDialog;

namespace Core {
class FullReleasePage;
class IFullReleaseStep;

namespace Ui {
    class MainWindow;
}

class IToolPage;

class MainWindow : public Core::IMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized() {}

public Q_SLOTS:
    void postCoreInitialization();
    void preparePages();

private Q_SLOTS:
    void pageSelected();
    void showHelp();
    void createFullRelease();
    void startNextDownload();
    void startNextProcess();
    void startNextPostProcessDownload();

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *e);
    void saveSettings();
    void readSettings();

private:
    Ui::MainWindow *ui;
    FullReleasePage *m_FullReleasePage;
    QList<IToolPage *> m_pages;
    Core::IFullReleaseStep *m_ActiveStep;
    QFutureWatcher<void> *m_Watcher;
    bool m_applied;
    QString m_currentCategory;
    QString m_currentPage;
    QProgressDialog *m_FullReleaseProgress;
};


}  //  End namespace Core


#endif // MAINWINDOW_H
