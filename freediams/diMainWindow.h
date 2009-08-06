/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DIMAINWINDOW_H
#define DIMAINWINDOW_H

#include <tkMainWindow.h>

#include <QCloseEvent>
#include <QListView>
class mfDrugsModel;

// include toolkit headers
class tkSettings;
class tkTranslators;

#include "ui_diMainWindow.h"

/**
 * \file diMainWindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.10
 * \date 21 July 2009
 * \brief Mainwindow of freediams.
   \ingroup freediams
*/

class diMainWindow: public tkMainWindow , public Ui::diMainWindow
{
    Q_OBJECT
    friend class diCore;
public:
    diMainWindow( QWidget *parent = 0 );
    ~diMainWindow();
    void initialize();

    void refreshPatient() const;

protected:
    void closeEvent( QCloseEvent *event );
    void readSettings();
    void writeSettings();
    void createDefaultSettings();
    void createStatusBar();
    void changeFontTo( const QFont &font );

public Q_SLOTS:
    bool updateFound() { return tkMainWindow::updateFound(); }

private Q_SLOTS:
    // Automatic slots for tkMainWindow // File actions
//    bool newFile() { return false; }
    bool openFile();
    bool saveFile();
    bool saveAsFile();
    bool print();
    bool savePrescription( const QString &fileName = QString::null );

    // Automatic slots for tkMainWindow // Configuration actions
    bool applicationPreferences();
    bool configureMedintux();

    // Automatic slots for tkMainWindow // Help actions
    bool applicationHelp();
    bool aboutApplication();
    bool debugDialog();

    // slots for patient's datas
    void on_patientName_textChanged(const QString &text);
    void on_patientWeight_valueChanged(const QString &text);
    void on_patientSize_valueChanged(const QString &text);
    void on_sexCombo_currentIndexChanged(const QString &text);
    void on_patientClCr_valueChanged(const QString &text);
    void on_patientCreatinin_valueChanged(const QString &text);
    void on_listOfAllergies_textChanged(const QString &text);

private:
    QString               m_iniPath;

    QAction *prefAct, *aboutAct, *debugAct, *configureMedinTuxAct, *saveAct;

};

#endif  // DIMAINWINDOW_H
