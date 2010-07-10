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
#ifndef SIMPLETEXTDIALOG_H
#define SIMPLETEXTDIALOG_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/idocumentprinter.h>

#include <QDialog>
#include <QString>

/**
 * \file simpletextdialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.4
 * \date 10 July 2010
*/


namespace Core {
namespace Internal {
namespace Ui {
class SimpleTextDialog;
}
}  // End Internal

class CORE_EXPORT SimpleTextDialog : public QDialog
{
    Q_OBJECT

public:
    SimpleTextDialog(const QString &title, const QString &zoomSettingKey = QString::null, QWidget *parent = 0);
    ~SimpleTextDialog();

    void setHtml(const QString &html);
    void setPlainText(const QString &text);
    void setHelpPageUrl(const QString &url) {m_HelpUrl = url;}
    void setUserPaper(const int iDocumentPaper) {m_Papers = iDocumentPaper;}
    void setPrintDuplicata(const bool state) {m_Duplicata = state;}

protected:
    void changeEvent(QEvent *e);

protected Q_SLOTS:
    void print();
    void showHelp();
    void zoomIn();
    void zoomOut();

private:
    Internal::Ui::SimpleTextDialog *ui;
    int m_Zoom;
    QString m_Key, m_HelpUrl;
    int m_Papers;
    bool m_Duplicata;
};

}  // End namespace Core

#endif // SIMPLETEXTDIALOG_H
