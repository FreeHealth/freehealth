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
 ***************************************************************************/
#ifndef TKUSERVIEWER_P_H
#define TKUSERVIEWER_P_H

// include Qt headers
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QDataWidgetMapper>

// include uis
#include "ui_userviewer.h"
//#include "ui_tkHeadersFootersViewerPrivate.h"

/**
 \brief Private part.
 \internal
 \ingroup usertoolkit widget_usertoolkit usermanager
*/

namespace Print {
class Printer;
class PrinterPreviewer;
}

namespace UserPlugin {
namespace Internal {

class UserViewerPrivate : public QObject, public Ui::UserViewer
{
    Q_OBJECT    
public:
    explicit UserViewerPrivate( QObject * parent ); // work with usermodel
    virtual ~UserViewerPrivate() {}

    void initialize();

public:
    void checkUserRights();
    void changeUserIndex( const int modelRow );

public Q_SLOTS:
    void on_modelReseted();
    void on_languageCombo_activated( int );
    void on_but_changePassword_clicked();
    void on_but_viewHistory_clicked();

private:
    void prepareMapper();

public:
    bool m_CanModify, m_CanRead;
    QWidget *m_Parent;
    int m_Row;
    QDataWidgetMapper *m_Mapper;
    Print::PrinterPreviewer *genericPreview, *adminPreview, *prescriptionPreview;
};

}  // End Internal
}  // End UserPlugin

#endif // TKUSERVIEWER_P_H
