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
#ifndef TKDEBUGDIALOG_P_H
#define TKDEBUGDIALOG_P_H

// include toolkit headers
#include <tkSendMessage.h>
class tkSettings;

// include Qt headers
#include <QObject>
#include <QDialog>
#include <QMessageBox>

// include ui
#include "ui_tkDebugDialog.h"


class tkDebugDialogPrivate : public QObject, public Ui::tkDebugDialog
{
    Q_OBJECT
public:
    tkDebugDialogPrivate( QObject * parent, tkSettings * settings );
    ~tkDebugDialogPrivate() {}

    void initialize();

public Q_SLOTS:
    void on_butSend_clicked();
    void onSendMessage_done();
    bool on_butSave_clicked();
    void on_expandedMessagesButton_clicked();
    void on_chronologicMessagesButton_clicked();
    void on_expandedErrorButton_clicked();
    void on_chronologicErrorButton_clicked();

public:
    QDialog *       m_Parent;
    tkSettings *    m_settings;
    tkSendMessage   m_sender;
    bool            m_sending;
    QMessageBox *   m_infoMessageBox;
    bool            m_MessageViewExpanded, m_ErrorViewExpanded;

};

#endif // TKDEBUGDIALOG_P_H
