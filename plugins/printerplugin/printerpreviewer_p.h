/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL>                                                       *
 ***************************************************************************/
#ifndef PRINTERPREVIEWER_PRIVATE_H
#define PRINTERPREVIEWER_PRIVATE_H

#include <printerplugin/printer.h>

#include <texteditorplugin/texteditor.h>

#include <QWidget>
#include <QPixmap>

#include "ui_printerpreviewer_p.h"

/**
 * \file printerpreviewer_p.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.4
 * \date 15 Oct 2009
*/

namespace Print {
namespace Internal {
class PrinterPreviewerPrivate : public PrinterPreviewer, private Ui::PrinterPreviewerPrivate
{
    Q_OBJECT

public:
    explicit PrinterPreviewerPrivate(QWidget *parent = 0);
    ~PrinterPreviewerPrivate() {}

    void initialize();

    QTextEdit *headerEditor() const;
    QTextEdit *footerEditor() const;
    QTextEdit *watermarkEditor() const;

    QComboBox *headerPresenceCombo() const;
    QComboBox *footerPresenceCombo() const;
    QComboBox *watermarkPresenceCombo() const;

    void setHeaderHtml(const QString &html);
    void setFooterHtml(const QString &html);
    void setWatermarkHtml(const QString &html);

    QString headerToHtml() const;
    QString footerToHtml() const;
    QString watermarkToHtml() const;

    void setHeaderPresence(const int presence);
    void setFooterPresence(const int presence);
    void setWatermarkPresence(const int presence);

    int headerPresence() const;
    int footerPresence() const;
    int watermarkPresence() const;

    void setHeader(const TextDocumentExtra *extra);
    void setFooter(const TextDocumentExtra *extra);
    void setWatermark(const TextDocumentExtra *extra);

    void headerToPointer(TextDocumentExtra *extra);
    void footerToPointer(TextDocumentExtra *extra);
    void watermarkToPointer(TextDocumentExtra *extra);


    void setExtraDocument(const QVariant &doc);
    QVariant extraDocument() const;


private Q_SLOTS:
    void on_updatePreviewButton_clicked();
    void on_automaticUpdateCheck_stateChanged( int checkstate );
    void on_duplicataCheck_stateChanged( int state );
    void on_pageNumberSpinBox_valueChanged( int value );
//    void on_tooglePreviewButton_clicked();

private:
    void connectPreview(Editor::TextEditor *t);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    mutable Editor::TextEditor *m_EditorHeader;
    mutable Editor::TextEditor *m_EditorFooter;
    mutable Editor::TextEditor *m_EditorWatermark;
    bool m_AutoCheck;
    Printer printer;
    QPixmap m_PreviewPixmap;
};

}  // End Internal
}  // End Print
#endif // PRINTERPREVIEWER_PRIVATE_H
