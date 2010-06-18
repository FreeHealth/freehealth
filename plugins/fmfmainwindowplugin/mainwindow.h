/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FREEMEDFORMS_MAINWINDOW_H
#define FREEMEDFORMS_MAINWINDOW_H

#include <fmfmainwindowplugin/mainwindow_exporter.h>
#include <coreplugin/imainwindow.h>

#include <QCloseEvent>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 01 May 2010
*/

namespace Form {
class IFormIO;
}

namespace Utils {
class FancyTabWidget;
}

namespace Patients {
class PatientBar;
}
//namespace Form {
//class EpisodeModel;
//}

namespace Core {
class FileManager;
}

namespace MainWin {
namespace Internal {
class PatientModelWrapper;
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
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    void refreshPatient() const;
    void readSettings();
    void writeSettings();
    void createStatusBar();
    QStatusBar *statusBar();
    QString currentPatient() const;

public Q_SLOTS:
    void postCoreInitialization();
    void on_currentUser_Changed();
    void setCurrentPatient(const QModelIndex &index);

    // Interface of MainWidowActionHandler
//    bool newFile();
    bool openFile();
    bool loadFile(const QString &filename, const QList<Form::IFormIO *> &iolist = QList<Form::IFormIO *>());
//    bool saveFile();
//    bool saveAsFile();
//    bool print();

    bool createNewPatient();
    bool viewPatientIdentity();
    bool removePatient();

    bool applicationPreferences();
    bool applicationConfiguratorWizard();
//    bool configureMedintux();
//
//    virtual bool aboutApplication();
//    bool applicationHelp();
//    bool aboutQt();
//    bool aboutPlugins();

    void updateCheckerEnd();

    void openPatientFormsFile();
    void aboutToShowRecentFiles();
    void aboutToShowRecentPatients();
    void openRecentFile();
    void openRecentPatient();

protected:
    void closeEvent( QCloseEvent *event );
    void changeEvent(QEvent *event);

public:
    Utils::FancyTabWidget *m_modeStack;
    Patients::PatientBar *m_PatientBar;

    /** \todo Move this Patient History in the ICore ? */
    Core::FileManager *m_RecentPatients;

    bool m_HelpTextShow;
    uint m_AutomaticSaveInterval;   /*!< Interval between each automatic save in SECONDS */
    int  m_TimerId;
    bool m_OpenLastOpenedForm;
    QByteArray windowState;

    Internal::PatientModelWrapper *m_PatientModelWrapper;
};

} // End Core

#endif  // FREEMEDFORMS_MAINWINDOW_H
