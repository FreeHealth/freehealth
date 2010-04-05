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
 ***************************************************************************/
#ifndef COREDEBUGDIALOG_H
#define COREDEBUGDIALOG_H

#include <QDialog>
#include <QHash>
#include <QMessageBox>
#include <utils/messagesender.h>

/**
 * \file debugdialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.9
 * \date 09 Sept 2009
*/

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QStackedLayout;
QT_END_NAMESPACE

namespace Core {
class IDebugPage;
}

namespace Core {
namespace Internal {

namespace Ui {
    class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(DebugDialog)

public:
    explicit DebugDialog(QWidget * parent);
    ~DebugDialog();
    void showDialog();

private Q_SLOTS:
//    void done( int code );
    void currentItemChanged(QTreeWidgetItem *cat);

    void setPages(const QList<IDebugPage*> pages);

private Q_SLOTS:
    void on_fullScreenButton_clicked();
    void on_butSend_clicked();
    void onSendMessage_done();
    bool on_butSave_clicked();
//    void on_expandedMessagesButton_clicked();
//    void on_chronologicMessagesButton_clicked();
//    void on_expandedErrorButton_clicked();
//    void on_chronologicErrorButton_clicked();

private:
    Ui::DebugDialog *m_ui;
    QStackedLayout  *m_slayout;
    QHash<QTreeWidgetItem *, QWidget *> m_Widgets;
    Utils::MessageSender  m_sender;
    bool            m_sending;
    QMessageBox *   m_infoMessageBox;
    bool            m_MessageViewExpanded, m_ErrorViewExpanded;
};

} // End Internal
} // End Core

#endif // COREDEBUGDIALOG_H
