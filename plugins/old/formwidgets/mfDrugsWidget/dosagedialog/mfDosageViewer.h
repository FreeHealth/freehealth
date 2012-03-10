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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFDOSAGEVIEWER_H
#define MFDOSAGEVIEWER_H

// include drugwidget headers
class mfDosageModel;
class mfDosageViewerPrivate;

// include Qt headers
#include <QWidget>

// include Ui
#include "ui_mfDosageViewer.h"

/**
 * \file mfDosageViewer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.6
 * \date 30 June 2009
*/

class mfDosageViewer : public QWidget, public Ui::mfDosageViewer
{
    Q_OBJECT
    Q_DISABLE_COPY( mfDosageViewer );

public:
    explicit mfDosageViewer( QWidget *parent );
    ~mfDosageViewer();

    void setDosageModel( mfDosageModel *model );
    void useDrugsModel(const int CIS, const int drugRow);

public Q_SLOTS:
    void done( int r );
    void changeCurrentRow( const int dosageRow );
    void changeCurrentRow( const QModelIndex &item );

private:
    void resizeEvent( QResizeEvent * event );

private Q_SLOTS:
    void on_fromToIntakesCheck_stateChanged(int state);
    void on_fromToDurationCheck_stateChanged(int state);
    void on_intakesFromSpin_valueChanged(double d);
    void on_durationFromSpin_valueChanged(double d);
    void on_userformsButton_clicked();
    void on_dosageForAllInnCheck_stateChanged(int state);
    void on_aldCheck_stateChanged(int state);

private:
    mfDosageViewerPrivate *d;
};

#endif // MFDOSAGEVIEWER_H
