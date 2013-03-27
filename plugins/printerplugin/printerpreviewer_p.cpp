/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#include "printerpreviewer_p.h"
#include "textdocumentextra.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <utils/log.h>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPainter>
#include <QPixmap>

using namespace Print;
using namespace Print::Internal;
using namespace Trans::ConstantTranslations;

namespace {

const char* const EXAMPLE_CONTENT =
        "<p align=center><b>This is a sample content for the document</b></p><p>&nbsp;</p>"
        "<p align=justify><span style=\"font-size:10pt\">Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse dapibus rhoncus vehicula. Praesent vel eros id dolor malesuada sollicitudin. Nam eros justo, dignissim a adipiscing et, porta vehicula odio. Vivamus et dolor at arcu laoreet pharetra et at nibh. Vestibulum suscipit, eros vitae mollis porttitor, sapien nisl dictum massa, quis volutpat massa nisl ac urna. Proin vulputate sapien at tellus aliquet ultrices. Mauris urna leo, porttitor vitae tincidunt eleifend, congue egestas massa. Aenean vitae metus euismod ipsum ultricies sagittis non laoreet risus. Morbi nec tellus purus, at vestibulum mi. Fusce auctor, sapien eget sodales pulvinar, tellus turpis congue nibh, eu fringilla augue magna nec nisi. Vestibulum rutrum commodo diam nec elementum. Nullam turpis dolor, scelerisque id porttitor a, iaculis porttitor felis. Aliquam et est dui. Fusce lobortis rutrum quam. Cras vitae nisl tellus. Aliquam quis varius turpis. Etiam at lorem turpis. Quisque bibendum malesuada erat id dignissim.</span></p>"
        "<p align=justify><span style=\"font-size:10pt\">Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse dapibus rhoncus vehicula. Praesent vel eros id dolor malesuada sollicitudin. Nam eros justo, dignissim a adipiscing et, porta vehicula odio. Vivamus et dolor at arcu laoreet pharetra et at nibh. Vestibulum suscipit, eros vitae mollis porttitor, sapien nisl dictum massa, quis volutpat massa nisl ac urna. Proin vulputate sapien at tellus aliquet ultrices. Mauris urna leo, porttitor vitae tincidunt eleifend, congue egestas massa. Aenean vitae metus euismod ipsum ultricies sagittis non laoreet risus. Morbi nec tellus purus, at vestibulum mi. Fusce auctor, sapien eget sodales pulvinar, tellus turpis congue nibh, eu fringilla augue magna nec nisi. Vestibulum rutrum commodo diam nec elementum. Nullam turpis dolor, scelerisque id porttitor a, iaculis porttitor felis. Aliquam et est dui. Fusce lobortis rutrum quam. Cras vitae nisl tellus. Aliquam quis varius turpis. Etiam at lorem turpis. Quisque bibendum malesuada erat id dignissim.</span></p>";

QWidget *createEditor(QWidget *parent, Editor::TextEditor *t, const QString &trTitle, const QString &id, const int defaultPresence = Printer::EachPages)
{
    Q_UNUSED(defaultPresence);
    QWidget *w = new QWidget(parent);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout *grid = new QGridLayout(w);
    QLabel *l = new QLabel(trTitle, w);
    QFont bold;
    bold.setBold(true);
    l->setFont(bold);
    l->setObjectName(id);
    QComboBox *c = new QComboBox(w);
    c->setObjectName(id);
    c->addItems(Printer::presencesAvailable());
    c->setFocusPolicy(Qt::ClickFocus);
    grid->addWidget(l, 0, 0);
    grid->addWidget(c, 0, 1);
    grid->addWidget(t, 1, 0, 2, 2);
    grid->setMargin(2);
    grid->setSpacing(2);
    return w;
}

}

/**
  \brief For internal use
  \internal
  \todo Manage printer page magins...
  \todo Manage the page changer for the preview (spinbox)
  \todo pageNumberSpinBox
*/
PrinterPreviewerPrivate::PrinterPreviewerPrivate(QWidget *parent) :
    PrinterPreviewer(parent),
    m_EditorHeader(0), m_EditorFooter(0), m_EditorWatermark(0),
    m_AutoCheck(false)
{
    printer.setContent(EXAMPLE_CONTENT);
    printer.setPrinter(new QPrinter(QPrinter::ScreenResolution));
    printer.printer()->setPaperSize(QPrinter::A4);
}

void PrinterPreviewerPrivate::initialize()
{
    setupUi(this);
    if (!m_EditorHeader) {
        m_EditorHeader = new Editor::TextEditor(this, Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
        editorLayout->insertWidget(0, createEditor(this, m_EditorHeader, tkTr(Trans::Constants::HEADER), Trans::Constants::HEADER));
    }
    if (!m_EditorFooter) {
        m_EditorFooter = new Editor::TextEditor(this, Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
        editorLayout->insertWidget(1, createEditor(this, m_EditorFooter, tkTr(Trans::Constants::FOOTER), Trans::Constants::FOOTER));
    }
    if (!m_EditorWatermark) {
        m_EditorWatermark = new Editor::TextEditor(this, Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
        editorLayout->insertWidget(2, createEditor(this, m_EditorWatermark, tkTr(Trans::Constants::WATERMARK), Trans::Constants::WATERMARK));
    }
}

QTextEdit *PrinterPreviewerPrivate::headerEditor() const
{
    return m_EditorHeader->textEdit();
}
QTextEdit *PrinterPreviewerPrivate::footerEditor() const
{
    return m_EditorFooter->textEdit();
}
QTextEdit *PrinterPreviewerPrivate::watermarkEditor() const
{
    return m_EditorWatermark->textEdit();
}
QComboBox *PrinterPreviewerPrivate::headerPresenceCombo() const
{
    return this->findChild<QComboBox *>(Trans::Constants::HEADER);
}

QComboBox *PrinterPreviewerPrivate::footerPresenceCombo() const
{
    return this->findChild<QComboBox *>(Trans::Constants::FOOTER);
}

QComboBox *PrinterPreviewerPrivate::watermarkPresenceCombo() const
{
    QComboBox *c = this->findChild<QComboBox *>(Trans::Constants::WATERMARK);
    return c;
}

void PrinterPreviewerPrivate::setHeaderHtml(const QString &html)
{
    if (!m_EditorHeader) {
        m_EditorHeader = new Editor::TextEditor(this, Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
        editorLayout->insertWidget(0, createEditor(this, m_EditorHeader, tkTr(Trans::Constants::HEADER), Trans::Constants::HEADER));
    }
//    qWarning() << html;
//    printer.setHeader(html,p);
    m_EditorHeader->textEdit()->setHtml(html);
//    QComboBox *c = this->findChild<QComboBox *>(tkTr(Trans::Constants::HEADER));
//    if (c)
//        return c->setCurrentIndex(p);
    connectPreview(m_EditorHeader);
}

void PrinterPreviewerPrivate::setFooterHtml(const QString &html)
{
    if (!m_EditorFooter) {
        m_EditorFooter = new Editor::TextEditor(this, Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
        editorLayout->insertWidget(1, createEditor(this, m_EditorFooter, tkTr(Trans::Constants::FOOTER), Trans::Constants::FOOTER));
    }
//    printer.setFooter(html,p);
    m_EditorFooter->textEdit()->setHtml(html);
    connectPreview(m_EditorFooter);
}

void PrinterPreviewerPrivate::setWatermarkHtml(const QString &html)
{
    if (!m_EditorWatermark) {
        m_EditorWatermark = new Editor::TextEditor(this, Editor::TextEditor::Simple | Editor::TextEditor::WithIO | Editor::TextEditor::WithTables);
        editorLayout->insertWidget(2, createEditor(this, m_EditorWatermark, tkTr(Trans::Constants::WATERMARK), Trans::Constants::WATERMARK));
    }
//    printer.addHtmlWatermark(html,p);
    m_EditorWatermark->textEdit()->setHtml(html);
//    QComboBox *c = this->findChild<QComboBox *>(tkTr(Trans::Constants::WATERMARK));
//    if (c)
//        return c->setCurrentIndex(p);
    connectPreview(m_EditorWatermark);
}

QString PrinterPreviewerPrivate::headerToHtml() const
{
    if (m_EditorHeader)
        return m_EditorHeader->textEdit()->toHtml();
    return QString();
}

QString PrinterPreviewerPrivate::footerToHtml() const
{
    if (m_EditorFooter)
        return m_EditorFooter->textEdit()->toHtml();
    return QString();
}

QString PrinterPreviewerPrivate::watermarkToHtml() const
{
    if (m_EditorWatermark)
        return m_EditorWatermark->textEdit()->toHtml();
    return QString();
}


void PrinterPreviewerPrivate::setHeader(const TextDocumentExtra *extra)
{
    if (extra) {
        setHeaderHtml(extra->toHtml());
        setHeaderPresence(extra->presence());
    }
}

void PrinterPreviewerPrivate::setFooter(const TextDocumentExtra *extra)
{
    if (extra) {
        setFooterHtml(extra->toHtml());
        setFooterPresence(extra->presence());
    }
}

void PrinterPreviewerPrivate::setWatermark(const TextDocumentExtra *extra)
{
    if (extra) {
        setWatermarkHtml(extra->toHtml());
        setWatermarkPresence(extra->presence());
    }
}

void PrinterPreviewerPrivate::headerToPointer(TextDocumentExtra *extra)
{
    if (m_EditorHeader) {
        extra->setHtml(m_EditorHeader->textEdit()->toHtml());
        extra->setPresence(Printer::Presence(headerPresence()));
//        extra->setPriority(Printer::Priority(headerPriority()));
    } else {
        delete extra;
        extra = 0;
        extra = new TextDocumentExtra();
    }
}

void PrinterPreviewerPrivate::footerToPointer(TextDocumentExtra *extra)
{
    if (m_EditorFooter) {
        extra->setHtml(m_EditorFooter->textEdit()->toHtml());
        extra->setPresence(Printer::Presence(footerPresence()));
//        extra->setPriority(Printer::Priority(hfooterPriority()));
    } else {
        delete extra;
        extra = 0;
        extra = new TextDocumentExtra();
    }
}

void PrinterPreviewerPrivate::watermarkToPointer(TextDocumentExtra *extra)
{
    if (m_EditorWatermark) {
        extra->setHtml(m_EditorWatermark->textEdit()->toHtml());
        extra->setPresence(Printer::Presence(watermarkPresence()));
//        extra->setPriority(Printer::Priority(watermarkPriority()));
    } else {
        delete extra;
        extra = 0;
        extra = new TextDocumentExtra();
    }
}

QVariant PrinterPreviewerPrivate::extraDocument() const
{
//    qWarning() << "PrinterPreviewerPrivate::extraDocument()";
    QVariantList list;
    QVariant q;
    TextDocumentExtra(headerToHtml(), headerPresence(), Printer::First);
    q.setValue(TextDocumentExtra(headerToHtml(), headerPresence(), Printer::First));
    list << q;
    q.setValue(TextDocumentExtra(footerToHtml(), footerPresence(), Printer::First));
    list << q;
    q.setValue(TextDocumentExtra(watermarkToHtml(), watermarkPresence(), Printer::First));
    list << q;
    return list;
}

/** \brief Connect or disconnect editor \e t to update previewer, according to m_AutoCheck. */
void PrinterPreviewerPrivate::connectPreview(Editor::TextEditor * t)
{
    if (!t)
        return;
    if (m_AutoCheck)
        connect(t->textEdit(), SIGNAL(textChanged()), this, SLOT(on_updatePreviewButton_clicked()));
    else
        disconnect(t->textEdit(), SIGNAL(textChanged()), this, SLOT(on_updatePreviewButton_clicked()));
}

void PrinterPreviewerPrivate::setHeaderPresence(const int presence)
{
    QComboBox *c = this->headerPresenceCombo();
    if (c)
        c->setCurrentIndex(presence);
}

void PrinterPreviewerPrivate::setFooterPresence(const int presence)
{
    QComboBox *c = this->footerPresenceCombo();
    if (c)
        c->setCurrentIndex(presence);
}

void PrinterPreviewerPrivate::setWatermarkPresence(const int presence)
{
    QComboBox *c = this->watermarkPresenceCombo();
//    qWarning() << "setWatermarkPresence" << presence << c;
    if (c)
        c->setCurrentIndex(presence);
}

int PrinterPreviewerPrivate::headerPresence() const
{
    QComboBox *c = this->headerPresenceCombo();
    if (c)
        return c->currentIndex();
    return 0;

}

int PrinterPreviewerPrivate::footerPresence() const
{
    QComboBox *c = this->footerPresenceCombo();
    if (c)
        return c->currentIndex();
    return 0;

}

int PrinterPreviewerPrivate::watermarkPresence() const
{
//    qWarning() << "watermarkPresence";
    QComboBox *c = this->watermarkPresenceCombo();
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
        printer.setHeader(m_EditorHeader->textEdit()->toHtml(), Printer::Presence(headerPresence()));
    }
    if (m_EditorFooter) {
        printer.setFooter(m_EditorFooter->textEdit()->toHtml(), Printer::Presence(footerPresence()));
    }
    if (m_EditorWatermark) {
        printer.addHtmlWatermark(m_EditorWatermark->textEdit()->toHtml(), Printer::Presence(watermarkPresence()));
    }

    printer.preparePages();
    resizeEvent(0);
}

void PrinterPreviewerPrivate::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    QSizeF s = printer.printer()->paperRect(QPrinter::DevicePixel).size();
    QPixmap pix(s.width(), s.height());
    pix.fill();
    QPainter paint;
    paint.begin(&pix);
    if (printer.printWithDuplicatas())
        printer.pageToPainter(&paint, 2, false, true);
    else
        printer.pageToPainter(&paint, 1, false, true);
    paint.end();

    if (previewLabel->size().height() < pix.size().height()) {
        pix = pix.scaled(previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    previewLabel->setPixmap(pix);
}

void PrinterPreviewerPrivate::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
    {
        retranslateUi(this);
        QStringList ids;
        ids << Trans::Constants::HEADER << Trans::Constants::FOOTER << Trans::Constants::WATERMARK;
        foreach(const QString &id, ids) {
            // Retranslate all combo contents
            QComboBox *c = this->findChild<QComboBox*>(id);
            if (c) {
                c->blockSignals(true);
                int current = c->currentIndex();
                c->clear();
                c->addItems(Printer::presencesAvailable());
                c->setCurrentIndex(current);
                c->blockSignals(false);
            }
            QLabel *l = this->findChild<QLabel*>(id);
            if (l) {
                l->setText(tkTr(id.toUtf8()));
            }
        }
        break;
    }
    default:
        break;
    }
}

void PrinterPreviewerPrivate::on_automaticUpdateCheck_stateChanged(int checkstate)
{
    if (checkstate == Qt::Unchecked) {
        m_AutoCheck=false;
    } else {
        m_AutoCheck=true;
        on_updatePreviewButton_clicked();
    }
    connectPreview(m_EditorHeader);
    connectPreview(m_EditorFooter);
    connectPreview(m_EditorWatermark);
}

void PrinterPreviewerPrivate::on_duplicataCheck_stateChanged(int state)
{
    if (state == Qt::Checked)
        printer.setPrintWithDuplicata(true);
    else
        printer.setPrintWithDuplicata(false);
    on_updatePreviewButton_clicked();
}

void PrinterPreviewerPrivate::on_pageNumberSpinBox_valueChanged(int value)
{
    // TODO: needs modification of Printer::previewToPixmap(QPixmap &drawTo, QPrinter *printer) to
    //          Printer::previewToPixmap(QPixmap &drawTo, QPrinter *printer, int pageNumber)
    Q_UNUSED(value);
}

//void PrinterPreviewerPrivate::on_tooglePreviewButton_clicked()
//{
//    previewWidget->setVisible(!previewWidget->isVisible());
//}
