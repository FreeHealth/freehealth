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
#ifndef MFDRUGSCENTRALWIDGET_H
#define MFDRUGSCENTRALWIDGET_H

class mfDrugsContext;
class mfPrescriptionViewer;

#include "ui_mfDrugsCentralWidget.h"

/**
 * \file mfDrugsCentralWidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.2
 * \date 15 July 2009
 * \brief Includes in the same widget : drugselector, prescriptionviewer. Connections are made easy.
   \ingroup freediams
*/

class mfDrugsCentralWidget : public QWidget, private Ui::mfDrugsCentralWidget
{
    Q_OBJECT
    friend class mfDrugsActionHandler;

#ifdef DRUGS_INTERACTIONS_STANDALONE
    friend class diMainWindow;
#endif

public:
    mfDrugsCentralWidget(QWidget *parent = 0);
    bool initialize();

    void changeFontTo( const QFont &font );
    mfDrugsModel *currentDrugsModel() const;

    QListView *prescriptionListView();
    mfPrescriptionViewer *prescriptionView();

    void setCurrentSearchMethod(int method);

protected:
    void createConnections();
    void disconnect();
    bool printPrescription();

private Q_SLOTS:
    // drugs slots
    void selector_drugSelected( const int CIS );

private:
    void focusInEvent(QFocusEvent *event);

private:
    mfDrugsModel   *m_CurrentDrugModel;
    mfDrugsContext *m_Context;
};

#endif // MFDRUGSCENTRALWIDGET_H
