/***************************************************************************
 *   FreeMedForms Project                                                  *
 *   Copyright (C) 2008-2009 by Eric MAEKER, MD                            *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MFDRUGDOSAGEDIALOG_H
#define MFDRUGDOSAGEDIALOG_H

// include drugwidget headers
class mfDrugDosage;

// include Ui
#include "ui_mfDrugDosageDialog.h"

/**
 * \file mfDrugDosageDialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.01
 * \date 31 Janv 2009
*/

class mfDrugDosageDialog : public QDialog, private Ui::mfDrugDosageDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( mfDrugDosageDialog );

public:
    explicit mfDrugDosageDialog( QWidget *parent = 0 , int CIS_id = 0, mfDrugDosage *d = 0);

    // setter
    void setDosage( mfDrugDosage* d );

    // getter
    mfDrugDosage * getDosage();

private:
    void resizeEvent( QResizeEvent * event );
    void resizeTableWidget();

private slots:
    void on_tabWidget_currentChanged( int index );
    void on_minIntakesSlider_valueChanged( int value );
    void on_maxIntakesSlider_valueChanged( int value );
    void on_minDurationSlider_valueChanged( int value );
    void on_maxDurationSlider_valueChanged( int value );

private:
    mfDrugDosage *  m_Dosage;
};

#endif // MFDRUGDOSAGEDIALOG_H
