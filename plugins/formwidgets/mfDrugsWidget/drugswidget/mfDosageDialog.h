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
#ifndef MFDOSAGEDIALOG_H
#define MFDOSAGEDIALOG_H

// include drugwidget headers
class mfDosageModel;

// include Qt headers
#include <QtGlobal>
QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

// include Ui
#include "ui_mfDosageDialog.h"

/**
 * \file mfDosageDialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.11
 * \date 07 June 2009
*/

/**
 * \brief Dialog for dosage prescription based on a standard dosage set.
 * Before all, this dialog is a wrapper on the mfDrugsModel (not the mfDosageModel). The mfDrugsModel is a kind of proxy
 * that manages drugs (view only) / dosages (via mfDosageModel) / interactions (view only).
 * If you want to create a new dosage, you must create a new row onto the model BEFORE.
 * If you want to edit or modify a dosage, you must inform the dialog of the row and the CIS of the drug.
 \ingroup drugsinteractions drugswidget
*/
class mfDosageDialog : public QDialog, public Ui::mfDosageDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( mfDosageDialog );

public:
    explicit mfDosageDialog( QWidget *parent, const int drugRow , const int dosageRow );
    ~mfDosageDialog();

    void changeRow( const int drugRow , const int dosageRow );

private:
    void createDefaultDosage();
    void setPrescriptionState( const int index, const int qtCheckState );
    void modelToCheckBoxes();

private Q_SLOTS:
    void done( int r );
    void on_drugNameButton_clicked();
    void availableDosages_activated( const QModelIndex & item );
    void on_fromToIntakesCheck_stateChanged( int state );
    void on_fromToDurationCheck_stateChanged( int state );
    void on_ALDCheck_stateChanged( int state );
    void on_INNCheck_stateChanged( int state );
    void on_noFormCheck_stateChanged( int state );
    void on_noPresentationCheck_stateChanged( int state );
    void on_intakesFromSpin_valueChanged( double d );
    void on_durationFromSpin_valueChanged( double d  );
    void on_userformsButton_clicked();
    void on_toogleInnButton_clicked();

private:
    QDataWidgetMapper  *m_Mapper;
    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
    int m_DrugRow, m_DosageRow;
    QMenu *m_UserFormButtonPopup;
};

#endif // MFDOSAGEDIALOG_H
