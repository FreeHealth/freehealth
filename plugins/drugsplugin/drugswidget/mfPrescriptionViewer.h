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
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PRESCRIPTIONVIEWER_H
#define PRESCRIPTIONVIEWER_H

#include <drugsplugin/drugs_exporter.h>

#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QListView>

#include "ui_mfPrescriptionViewer.h"

/**
 * \file mfPrescriptionViewer.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 06 Mar 2010
*/

namespace DrugsDB {
class DrugsModel;
}

namespace DrugsWidget {

/**
 \brief This widget is the prescription viewer. It deals data with mfDrugsModel. The model MUST BE SETTED using setModel() and setModelColumn().
 Signals are emitted when user ask for : save (saveTriggered()) and print (printTriggered()) prescription.
 \ingroup freediams drugswidget
  */
class DRUGS_EXPORT PrescriptionViewer : public QWidget, private Internal::Ui::PrescriptionViewer
{
    Q_OBJECT
    Q_DISABLE_COPY(PrescriptionViewer)
public:
    explicit PrescriptionViewer(QWidget *parent = 0);
    void initialize();
    QListView *listview();
    void setModel(DrugsDB::DrugsModel *model);
    void setListViewPadding(const int pad);

Q_SIGNALS:
    void saveTriggered();
    void printTriggered();

protected:
    virtual void changeEvent(QEvent *e);

private:
    void createActionsAndToolbar();

public Q_SLOTS:
    void clearTriggered();
    void removeTriggered();
    void moveUp();
    void moveDown();
    void sortDrugs();
    void showDrugInfo(const QModelIndex &item);
    void showDosageDialog(const QModelIndex &item);
    void viewInteractions();
    void changeDuration();
    void changeDurationTo();

private:
    QToolBar *m_ToolBar;               // drugslist widget's toolbar
};

}  // End DrugsWidget

#endif // PRESCRIPTIONVIEWER_H
