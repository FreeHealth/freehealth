/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDIAMS_MAINWINDOW_H
#define FREEDIAMS_MAINWINDOW_H

#include <coreplugin/core_exporter.h>
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
 * \version 0.0.14
 * \date 10 Sept 2009
*/

namespace Core {
namespace Internal {
namespace Ui {
class MainWindow;
}  // End Ui
}  // End Internal

class CORE_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void refreshPatient() const;
void readSettings();
void writeSettings();
void createStatusBar();
bool savePrescription(const QString &fileName = QString::null);
void changeFontTo(const QFont &font);


public Q_SLOTS: // Interface of MainWidowActionHandler
//    virtual bool newFile()     {return false;}
    bool openFile();
    bool saveFile();
    bool saveAsFile();
    bool print();
//
    bool applicationPreferences();
    bool configureMedintux();
//
//    virtual bool aboutApplication();
    bool applicationHelp();
//    bool aboutQt();
    bool aboutPlugins();

    // slots for patient's datas
    void on_patientName_textChanged(const QString &text);
    void on_patientWeight_valueChanged(const QString &text);
    void on_patientSize_valueChanged(const QString &text);
    void on_sexCombo_currentIndexChanged(const QString &text);
    void on_patientClCr_valueChanged(const QString &text);
    void on_patientCreatinin_valueChanged(const QString &text);
    void on_listOfAllergies_textChanged(const QString &text);

protected:
    void closeEvent( QCloseEvent *event );

public:
    Internal::Ui::MainWindow *m_ui;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QAction *recentFilesAct[MaxRecentFiles];
    QAction *aUserManager;

    QStringList recentFiles;

    QString           m_CurrentFile;
    bool              m_HelpTextShow;
    uint              m_AutomaticSaveInterval;   /*!< Interval between each automatic save in SECONDS */
    int               m_TimerId;
    //     mfRecovererThread thread;
    bool              m_OpenLastOpenedForm;
    QByteArray        windowState;
};

} // End Core

#endif  // FREEDIAMS_MAINWINDOW_H
