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

#include <QMainWindow>
#include <QCloseEvent>

#include <QListView>
class mfDrugSelector;
class mfDrugsPrescriptor;
class mfPrescriptionViewer;
class mfDrugsModel;
class mfDosageDialog;

// include toolkit headers
class tkSettings;
class tkTranslators;

#include "ui_diMainWindow.h"

/**
 * \file diMainWindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 12 May 2009
 * \brief Mainwindow of DrugsInteractions.
   \ingroup drugsinteractions
*/

class diMainWindow: public QMainWindow , public Ui::diMainWindow
{
    Q_OBJECT
    friend class diCore;
public:
    diMainWindow( QWidget *parent = 0 );
    ~diMainWindow();

protected Q_SLOTS:
    void on_updateFounded();
//    void showUpdateInformations();

protected:
    void closeEvent( QCloseEvent *event );
    void readSettings();
    void writeSettings();
    void createDefaultSettings();
    void createStatusBar();
    void createMenus();
    void createConnections();
    void changeFontTo( const QFont &font );

private Q_SLOTS:
    void about();
    void preferences();
    void debugDialog();
    void on_selector_drugSelected( const int CIS );
    void showDosageDialog(const QModelIndex &item);
    void configureMedinTux();
    void openPrescription();
    void savePrescription();
    void printPrescription();
    void on_patientName_textChanged(const QString &text);

private:
//    mfDrugSelector       *m_DrugSelector;
    mfDrugsPrescriptor   *m_DrugPrescriptor;
//    mfPrescriptionViewer *m_PrescriptionView;
    mfDrugsModel         *m_PrescriptionModel;
    mfDosageDialog       *m_DosageDialog;
    QString               m_iniPath;

    QAction *prefAct, *aboutAct, *debugAct, *configureMedinTuxAct, *saveAct;

};

#endif  // DIMAINWINDOW_H
