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
 *       NAME <MAIL>                                                       *
 ***************************************************************************/
#ifndef TKPRINTERPRIVATE_H
#define TKPRINTERPRIVATE_H

#include <printerplugin/printer.h>

#include <texteditorplugin/texteditor.h>

#include <QWidget>
#include <QPixmap>

#include "ui_printerpreviewer_p.h"

namespace Print {
namespace Internal {
class PrinterPreviewerPrivate : public PrinterPreviewer, private Ui::PrinterPreviewerPrivate
{
    /** \todo create a tkPrinterEditor including the texteditor, the combo and the label with automatic translations */
    Q_OBJECT

public:
    explicit PrinterPreviewerPrivate(QWidget *parent = 0);
    ~PrinterPreviewerPrivate() {}

    void initialize();

    QTextEdit *headerEditor();
    QTextEdit *footerEditor();
    QTextEdit *watermarkEditor();

    void setHeader(const QString &html, Printer::Presence p = Printer::EachPages);
    void setFooter(const QString &html, Printer::Presence p = Printer::EachPages);
    void setWatermark(const QString &html, Printer::Presence p = Printer::EachPages);

    void setHeader(const TextDocumentExtra *extra);
    void setFooter(const TextDocumentExtra *extra);
    void setWatermark(const TextDocumentExtra *extra);

    void headerToPointer(TextDocumentExtra *extra);
    void footerToPointer(TextDocumentExtra *extra);
    void watermarkToPointer(TextDocumentExtra *extra);

    QString headerToHtml();
    QString footerToHtml();
    QString watermarkToHtml();
    int headerPresence();
    int footerPresence();
    int watermarkPresence();

    void setExtraDocument(const QVariant &doc);
    QVariant extraDocument();


private Q_SLOTS:
    void on_updatePreviewButton_clicked();
    void on_automaticUpdateCheck_stateChanged( int checkstate );
    void on_duplicataCheck_stateChanged( int state );
    void on_pageNumberSpinBox_valueChanged( int value );

private:
    void connectPreview(Editor::TextEditor *t);
    void changeEvent(QEvent *e);

private:
    Editor::TextEditor *m_EditorHeader, *m_EditorFooter, *m_EditorWatermark;
    bool m_AutoCheck;
    Printer printer;
    QPixmap pixmap;
};

}  // End Internal
}  // End Print
#endif // TKPRINTERPRIVATE_H
