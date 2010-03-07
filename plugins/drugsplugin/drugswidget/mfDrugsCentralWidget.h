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
#ifndef DRUGSCENTRALWIDGET_H
#define DRUGSCENTRALWIDGET_H

#include <drugsplugin/drugs_exporter.h>

#include <QWidget>
#include <QObject>
#include <QListView>

/**
 * \file mfDrugsCentralWidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
 * \brief Includes in the same widget : drugselector, prescriptionviewer. Connections are made easy.
   \ingroup freediams
*/

namespace DrugsDB {
class DrugsModel;
}

namespace DrugsWidget {
namespace Internal {
class DrugsContext;
class DrugsActionHandler;
class DrugSelector;

namespace Ui {
class DrugsCentralWidget;
}  // End Ui
}  // End Internal

class PrescriptionViewer;
class DrugsModel;

class DRUGS_EXPORT DrugsCentralWidget : public QWidget
{
    Q_OBJECT
    friend class DrugsWidget::Internal::DrugsActionHandler;

public:
    DrugsCentralWidget(QWidget *parent = 0);
    bool initialize();
    void enterSelectionOnlyMode();

    void changeFontTo(const QFont &font);
    DrugsDB::DrugsModel *currentDrugsModel() const;

    QListView *prescriptionListView();
    PrescriptionViewer *prescriptionView();
    Internal::DrugSelector *drugSelector();

    void setCurrentSearchMethod(int method);
    bool printPrescription();
    void printPreview();
    bool createTemplate();
    void showDatabaseInformations();

protected:
    void createConnections();
    void disconnect();

private Q_SLOTS:
    // drugs slots
    void selector_drugSelected( const int CIS );

private:
    void focusInEvent(QFocusEvent *event);

private:
    Internal::Ui::DrugsCentralWidget *m_ui;
    DrugsDB::DrugsModel   *m_CurrentDrugModel;
    Internal::DrugsContext *m_Context;
    bool m_SelectionOnlyMode;
};

}  // End DrugsWidget

#endif // DRUGSCENTRALWIDGET_H
