/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
 ***************************************************************************/
#include "printerpreviewer_p.h"
#include "textdocumentextra.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

using namespace Print;
using namespace Print::Internal;
using namespace Trans::ConstantTranslations;

static const char* const EXAMPLE_CONTENT =
        "<p align=center><b>This is a sample content for the document</b></p><p>&nbsp;</p>"
        "<p align=justify><span style=\"font-size:10pt\">Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse dapibus rhoncus vehicula. Praesent vel eros id dolor malesuada sollicitudin. Nam eros justo, dignissim a adipiscing et, porta vehicula odio. Vivamus et dolor at arcu laoreet pharetra et at nibh. Vestibulum suscipit, eros vitae mollis porttitor, sapien nisl dictum massa, quis volutpat massa nisl ac urna. Proin vulputate sapien at tellus aliquet ultrices. Mauris urna leo, porttitor vitae tincidunt eleifend, congue egestas massa. Aenean vitae metus euismod ipsum ultricies sagittis non laoreet risus. Morbi nec tellus purus, at vestibulum mi. Fusce auctor, sapien eget sodales pulvinar, tellus turpis congue nibh, eu fringilla augue magna nec nisi. Vestibulum rutrum commodo diam nec elementum. Nullam turpis dolor, scelerisque id porttitor a, iaculis porttitor felis. Aliquam et est dui. Fusce lobortis rutrum quam. Cras vitae nisl tellus. Aliquam quis varius turpis. Etiam at lorem turpis. Quisque bibendum malesuada erat id dignissim.</span></p>"
        "<p align=justify><span style=\"font-size:10pt\">Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse dapibus rhoncus vehicula. Praesent vel eros id dolor malesuada sollicitudin. Nam eros justo, dignissim a adipiscing et, porta vehicula odio. Vivamus et dolor at arcu laoreet pharetra et at nibh. Vestibulum suscipit, eros vitae mollis porttitor, sapien nisl dictum massa, quis volutpat massa nisl ac urna. Proin vulputate sapien at tellus aliquet ultrices. Mauris urna leo, porttitor vitae tincidunt eleifend, congue egestas massa. Aenean vitae metus euismod ipsum ultricies sagittis non laoreet risus. Morbi nec tellus purus, at vestibulum mi. Fusce auctor, sapien eget sodales pulvinar, tellus turpis congue nibh, eu fringilla augue magna nec nisi. Vestibulum rutrum commodo diam nec elementum. Nullam turpis dolor, scelerisque id porttitor a, iaculis porttitor felis. Aliquam et est dui. Fusce lobortis rutrum quam. Cras vitae nisl tellus. Aliquam quis varius turpis. Etiam at lorem turpis. Quisque bibendum malesuada erat id dignissim.</span></p>";

static QWidget *createEditor(QWidget *parent, Editor::TextEditor *t, const QString &title, const int defaultPresence = Printer::EachPages)
{
    Q_UNUSED(defaultPresence);
    QWidget *w = new QWidget(parent);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout *grid = new QGridLayout(w);
    QLabel *l = new QLabel( "<b>"+QApplication::translate("PrinterPreviewer", title.toAscii() )+"</b>", w);
    QComboBox *c = new QComboBox(w);
    c->setObjectName( title );
    c->insertItems( 0, Printer::presencesAvailable() );
    grid->addWidget( l, 0, 0 );
    grid->addWidget( c, 0, 1 );
    grid->addWidget( t, 1, 0, 2, 2);
    grid->setMargin(2);
    grid->setSpacing(2);
    return w;
}

/**
  \brief For internal use
  \internal
  \todo Manage printer page magins...
  \todo Manage the page changer for the preview (spinbox)
  \todo pageNumberSpinBox
*/
PrinterPreviewerPrivate::PrinterPreviewerPrivate(QWidget *parent) :
    PrinterPreviewer(parent), m_EditorHeader(0), m_EditorFooter(0), m_EditorWatermark(0), m_AutoCheck(false)
{
    printer.setContent(EXAMPLE_CONTENT);
    printer.setPrinter( new QPrinter(QPrinter::ScreenResolution) );
    printer.printer()->setPaperSize( QPrinter::A4 );
    printer.printer()->setPageMargins( 10., 10., 10., 10., QPrinter::Millimeter );
}

QTextEdit *PrinterPreviewerPrivate::headerEditor()
{
    if (!m_EditorHeader)
        setHeader("");
    return m_EditorHeader->textEdit();
}
QTextEdit *PrinterPreviewerPrivate::footerEditor()
{
    if (!m_EditorFooter)
        setFooter("");
    return m_EditorFooter->textEdit();
}
QTextEdit *PrinterPreviewerPrivate::watermarkEditor()
{
    if (!m_EditorWatermark)
        setWatermark("");
    return m_EditorWatermark->textEdit();
}

void PrinterPreviewerPrivate::setHeader(const QString &html, Printer::Presence p)
{
    if (!m_EditorHeader) {
        m_EditorHeader = new Editor::TextEditor(this, Editor::TextEditor::Full);
        editorLayout->insertWidget(0, createEditor( this, m_EditorHeader, tkTr(Trans::Constants::HEADER) ) );
    }
//    qWarning() << html;
    printer.setHeader(html,p);
    m_EditorHeader->textEdit()->setHtml(html);
    QComboBox *c = this->findChild<QComboBox *>( tkTr(Trans::Constants::HEADER) );
    if (c)
        return c->setCurrentIndex(p);
    connectPreview(m_EditorHeader);
}

void PrinterPreviewerPrivate::setFooter(const QString &html, Printer::Presence p)
{
    if (!m_EditorFooter) {
        m_EditorFooter = new Editor::TextEditor(this, Editor::TextEditor::Full);
        editorLayout->insertWidget(1, createEditor( this, m_EditorFooter, tkTr(Trans::Constants::FOOTER) ) );
    }
    printer.setFooter(html,p);
    m_EditorFooter->textEdit()->setHtml(html);
    QComboBox *c = this->findChild<QComboBox *>( tkTr(Trans::Constants::FOOTER) );
    if (c)
        return c->setCurrentIndex(p);
    connectPreview(m_EditorFooter);
}

void PrinterPreviewerPrivate::setWatermark(const QString &html, Printer::Presence p)
{
    if (!m_EditorWatermark) {
        m_EditorWatermark = new Editor::TextEditor(this, Editor::TextEditor::Full);
        editorLayout->insertWidget(2, createEditor( this, m_EditorWatermark, tkTr(Trans::Constants::WATERMARK) ) );
    }
    printer.addHtmlWatermark(html,p);
    m_EditorWatermark->textEdit()->setHtml(html);
    QComboBox *c = this->findChild<QComboBox *>( tkTr(Trans::Constants::WATERMARK) );
    if (c)
        return c->setCurrentIndex(p);
    connectPreview(m_EditorWatermark);
}

QString PrinterPreviewerPrivate::headerToHtml()
{
    if (m_EditorHeader)
        return m_EditorHeader->textEdit()->toHtml();
    return QString();
}

QString PrinterPreviewerPrivate::footerToHtml()
{
    if (m_EditorFooter)
        return m_EditorFooter->textEdit()->toHtml();
    return QString();
}

QString PrinterPreviewerPrivate::watermarkToHtml()
{
    if (m_EditorWatermark)
        return m_EditorWatermark->textEdit()->toHtml();
    return QString();
}


void PrinterPreviewerPrivate::setHeader(const TextDocumentExtra *extra)
{
    setHeader( extra->toHtml(), extra->presence() );
}

void PrinterPreviewerPrivate::setFooter(const TextDocumentExtra *extra)
{
    setFooter( extra->toHtml(), extra->presence() );
}

void PrinterPreviewerPrivate::setWatermark(const TextDocumentExtra *extra)
{
    setWatermark( extra->toHtml(), extra->presence() );
}

void PrinterPreviewerPrivate::headerToPointer(TextDocumentExtra *extra)
{
    if (m_EditorHeader) {
        extra->setHtml( m_EditorHeader->textEdit()->toHtml() );
        extra->setPresence( Printer::Presence(headerPresence()) );
//        extra->setPriority( Printer::Priority(headerPriority()) );
    } else {
        delete extra;
        extra = 0;
        extra = new TextDocumentExtra();
    }
}

void PrinterPreviewerPrivate::footerToPointer(TextDocumentExtra *extra)
{
    if (m_EditorFooter) {
        extra->setHtml( m_EditorFooter->textEdit()->toHtml() );
        extra->setPresence( Printer::Presence(footerPresence()) );
//        extra->setPriority( Printer::Priority(hfooterPriority() ));
    } else {
        delete extra;
        extra = 0;
        extra = new TextDocumentExtra();
    }
}

void PrinterPreviewerPrivate::watermarkToPointer(TextDocumentExtra *extra)
{
    if (m_EditorWatermark) {
        extra->setHtml( m_EditorWatermark->textEdit()->toHtml() );
        extra->setPresence( Printer::Presence(watermarkPresence()) );
//        extra->setPriority(Printer::Priority( watermarkPriority()) );
    } else {
        delete extra;
        extra = 0;
        extra = new TextDocumentExtra();
    }
}

QVariant PrinterPreviewerPrivate::extraDocument()
{
//    qWarning() << "PrinterPreviewerPrivate::extraDocument()";
    QVariantList list;
    QVariant q;
    TextDocumentExtra(headerToHtml(), headerPresence(), Printer::First );
    q.setValue(TextDocumentExtra(headerToHtml(), headerPresence(), Printer::First ));
    list << q;
    q.setValue(TextDocumentExtra(footerToHtml(), footerPresence(), Printer::First ));
    list << q;
    q.setValue(TextDocumentExtra(watermarkToHtml(), watermarkPresence(), Printer::First ));
    list << q;
    return list;
}

void PrinterPreviewerPrivate::initialize()
{
    setupUi(this);
}


void PrinterPreviewerPrivate::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

/** \brief Connect or disconnect editor \e t to update previewer, according to m_AutoCheck. */
void PrinterPreviewerPrivate::connectPreview( Editor::TextEditor * t )
{
    if (!t)
        return;
    if (m_AutoCheck)
        connect(t->textEdit(), SIGNAL(textChanged()), this, SLOT( on_updatePreviewButton_clicked() ));
    else
        disconnect(t->textEdit(), SIGNAL(textChanged()), this, SLOT( on_updatePreviewButton_clicked() ));
}

int PrinterPreviewerPrivate::headerPresence()
{
    QComboBox *c = this->findChild<QComboBox *>( tkTr(Trans::Constants::HEADER) );
    if (c)
        return c->currentIndex();
    return 0;

}

int PrinterPreviewerPrivate::footerPresence()
{
    QComboBox *c = this->findChild<QComboBox *>( tkTr(Trans::Constants::FOOTER) );
    if (c)
        return c->currentIndex();
    return 0;

}

int PrinterPreviewerPrivate::watermarkPresence()
{
    QComboBox *c = this->findChild<QComboBox *>( tkTr(Trans::Constants::WATERMARK) );
    if (c)
        return c->currentIndex();
    return 0;

}

void PrinterPreviewerPrivate::on_updatePreviewButton_clicked()
{
    printer.clearHeaders();
    printer.clearFooters();
    printer.clearWatermark();
    if (m_EditorHeader) {
        printer.setHeader( m_EditorHeader->textEdit()->toHtml(), Printer::Presence(headerPresence()) );
    }
    if (m_EditorFooter) {
        printer.setFooter( m_EditorFooter->textEdit()->toHtml(), Printer::Presence(footerPresence() ));
    }
    if (m_EditorWatermark) {
        printer.addHtmlWatermark( m_EditorWatermark->textEdit()->toHtml(), Printer::Presence(watermarkPresence()) );
    }
    printer.previewToPixmap(m_PreviewPixmap, printer.printer());
    if (this->previewLabel->size().height() < m_PreviewPixmap.size().height()) {
        m_PreviewPixmap = m_PreviewPixmap.scaled(this->previewLabel->size(),Qt::KeepAspectRatio);
    }
    this->previewLabel->setPixmap(m_PreviewPixmap);
}

void PrinterPreviewerPrivate::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    if (!m_PreviewPixmap.isNull()) {
        this->previewLabel->setPixmap(m_PreviewPixmap.scaled(this->previewLabel->size(),Qt::KeepAspectRatio));
    }
}

void PrinterPreviewerPrivate::on_automaticUpdateCheck_stateChanged( int checkstate )
{
    if (checkstate==Qt::Unchecked) {
        m_AutoCheck=false;
    } else {
        m_AutoCheck=true;
    }
    connectPreview(m_EditorHeader);
    connectPreview(m_EditorFooter);
    connectPreview(m_EditorWatermark);
}

void PrinterPreviewerPrivate::on_duplicataCheck_stateChanged( int state )
{
    if (state == Qt::Checked)
        printer.printWithDuplicata(true);
    else
        printer.printWithDuplicata(false);
    on_updatePreviewButton_clicked();
}

void PrinterPreviewerPrivate::on_pageNumberSpinBox_valueChanged(int value)
{
    /** \todo needs modification of Printer::previewToPixmap( QPixmap &drawTo, QPrinter *printer ) to
              Printer::previewToPixmap( QPixmap &drawTo, QPrinter *printer, int pageNumber )
    */
}

void PrinterPreviewerPrivate::on_tooglePreviewButton_clicked()
{
    previewWidget->setVisible(!previewWidget->isVisible());
}
