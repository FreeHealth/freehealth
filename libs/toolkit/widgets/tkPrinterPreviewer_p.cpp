#include "tkPrinterPreviewer_p.h"

// including toolkit headers
#include <tkLog.h>
#include <tkRichTextEditor.h>
#include <tkTextDocumentExtra.h>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

namespace tkPrinterPreviewerPrivateConstants {

    const char* const HEADER = QT_TRANSLATE_NOOP( "tkPrinterPreviewer", "Header");
    const char* const FOOTER = QT_TRANSLATE_NOOP( "tkPrinterPreviewer", "Footer");
    const char* const WATERMARK = QT_TRANSLATE_NOOP( "tkPrinterPreviewer", "Watermark");
    const char* const EXAMPLE_CONTENT =
            "";

QWidget *createEditor( QWidget *parent, tkRichTextEditor *t, const QString &title, const int defaultPresence = tkPrinter::EachPages)
{
    QWidget *w = new QWidget(parent);
    QGridLayout *grid = new QGridLayout(w);
    QLabel *l = new QLabel( "<b>"+QApplication::translate("tkPrinterPreviewer", title.toAscii() )+"</b>", w);
    QComboBox *c = new QComboBox(w);
    c->setObjectName( title );
    c->insertItems( 0, tkPrinter::presencesAvailable() );
    grid->addWidget( l, 0, 0 );
    grid->addWidget( c, 0, 1 );
    grid->addWidget( t, 1, 0, 2, 2);
    grid->setMargin(2);
    grid->setSpacing(2);
    return w;
}

}

using namespace tkPrinterPreviewerPrivateConstants;

/**
  \brief For internal use
  \internal
  \todo Manage printer page magins...
  \todo Manage the page changer for the preview (spinbox)
  \todo pageNumberSpinBox
*/
tkPrinterPreviewerPrivate::tkPrinterPreviewerPrivate(QWidget *parent) :
    tkPrinterPreviewer(parent), m_EditorHeader(0), m_EditorFooter(0), m_EditorWatermark(0), m_AutoCheck(false)
{
    printer.setContent("<p>This is the previewing mode</p><p&nbsp;</p><p>This is the previewing mode</p><p&nbsp;</p><p>This is the previewing mode</p><p&nbsp;</p>");
    printer.setPrinter( new QPrinter(QPrinter::ScreenResolution) );
    printer.printer()->setPaperSize( QPrinter::A4 );
    printer.printer()->setPageMargins( 10., 10., 10., 10., QPrinter::Millimeter );
}

QTextEdit *tkPrinterPreviewerPrivate::headerEditor()
{
    if (!m_EditorHeader)
        setHeader("");
    return m_EditorHeader->textEdit();
}
QTextEdit *tkPrinterPreviewerPrivate::footerEditor()
{
    if (!m_EditorFooter)
        setFooter("");
    return m_EditorFooter->textEdit();
}
QTextEdit *tkPrinterPreviewerPrivate::watermarkEditor()
{
    if (!m_EditorWatermark)
        setWatermark("");
    return m_EditorWatermark->textEdit();
}

void tkPrinterPreviewerPrivate::setHeader(const QString &html, tkPrinter::Presence p)
{
    if (!m_EditorHeader) {
        m_EditorHeader = new tkRichTextEditor(this, tkRichTextEditor::Full);
        editorLayout->insertWidget(0, createEditor( this, m_EditorHeader, HEADER ) );
    }
//    qWarning() << html;
    printer.setHeader(html,p);
    m_EditorHeader->textEdit()->setHtml(html);
    QComboBox *c = this->findChild<QComboBox *>( HEADER );
    if (c)
        return c->setCurrentIndex(p);
    connectPreview(m_EditorHeader);
}

void tkPrinterPreviewerPrivate::setFooter(const QString &html, tkPrinter::Presence p)
{
    if (!m_EditorFooter) {
        m_EditorFooter = new tkRichTextEditor(this, tkRichTextEditor::Full);
        editorLayout->insertWidget(1, createEditor( this, m_EditorFooter, FOOTER ) );
    }
    printer.setFooter(html,p);
    m_EditorFooter->textEdit()->setHtml(html);
    QComboBox *c = this->findChild<QComboBox *>( FOOTER );
    if (c)
        return c->setCurrentIndex(p);
    connectPreview(m_EditorFooter);
}

void tkPrinterPreviewerPrivate::setWatermark(const QString &html, tkPrinter::Presence p)
{
    if (!m_EditorWatermark) {
        m_EditorWatermark = new tkRichTextEditor(this, tkRichTextEditor::Full);
        editorLayout->insertWidget(2, createEditor( this, m_EditorWatermark, WATERMARK ) );
    }
    printer.addHtmlWatermark(html,p);
    m_EditorWatermark->textEdit()->setHtml(html);
    QComboBox *c = this->findChild<QComboBox *>( WATERMARK );
    if (c)
        return c->setCurrentIndex(p);
    connectPreview(m_EditorWatermark);
}

QString tkPrinterPreviewerPrivate::headerToHtml()
{
    if (m_EditorHeader)
        return m_EditorHeader->textEdit()->toHtml();
    return QString();
}

QString tkPrinterPreviewerPrivate::footerToHtml()
{
    if (m_EditorFooter)
        return m_EditorFooter->textEdit()->toHtml();
    return QString();
}

QString tkPrinterPreviewerPrivate::watermarkToHtml()
{
    if (m_EditorWatermark)
        return m_EditorWatermark->textEdit()->toHtml();
    return QString();
}


void tkPrinterPreviewerPrivate::setHeader(const tkTextDocumentExtra *extra)
{
    setHeader( extra->toHtml(), extra->presence() );
}

void tkPrinterPreviewerPrivate::setFooter(const tkTextDocumentExtra *extra)
{
    setFooter( extra->toHtml(), extra->presence() );
}

void tkPrinterPreviewerPrivate::setWatermark(const tkTextDocumentExtra *extra)
{
    setWatermark( extra->toHtml(), extra->presence() );
}

void tkPrinterPreviewerPrivate::headerToPointer(tkTextDocumentExtra *extra)
{
    if (m_EditorHeader) {
        extra->setHtml( m_EditorHeader->textEdit()->toHtml() );
        extra->setPresence( tkPrinter::Presence(headerPresence()) );
//        extra->setPriority( tkPrinter::Priority(headerPriority()) );
    } else {
        delete extra;
        extra = 0;
        extra = new tkTextDocumentExtra();
    }
}

void tkPrinterPreviewerPrivate::footerToPointer(tkTextDocumentExtra *extra)
{
    if (m_EditorFooter) {
        extra->setHtml( m_EditorFooter->textEdit()->toHtml() );
        extra->setPresence( tkPrinter::Presence(footerPresence()) );
//        extra->setPriority( tkPrinter::Priority(hfooterPriority() ));
    } else {
        delete extra;
        extra = 0;
        extra = new tkTextDocumentExtra();
    }
}

void tkPrinterPreviewerPrivate::watermarkToPointer(tkTextDocumentExtra *extra)
{
    if (m_EditorWatermark) {
        extra->setHtml( m_EditorWatermark->textEdit()->toHtml() );
        extra->setPresence( tkPrinter::Presence(watermarkPresence()) );
//        extra->setPriority(tkPrinter::Priority( watermarkPriority()) );
    } else {
        delete extra;
        extra = 0;
        extra = new tkTextDocumentExtra();
    }
}

QVariant tkPrinterPreviewerPrivate::extraDocument()
{
//    qWarning() << "tkPrinterPreviewerPrivate::extraDocument()";
    QVariantList list;
    QVariant q;
    tkTextDocumentExtra(headerToHtml(), headerPresence(), tkPrinter::First );
    q.setValue(tkTextDocumentExtra(headerToHtml(), headerPresence(), tkPrinter::First ));
    list << q;
    q.setValue(tkTextDocumentExtra(footerToHtml(), footerPresence(), tkPrinter::First ));
    list << q;
    q.setValue(tkTextDocumentExtra(watermarkToHtml(), watermarkPresence(), tkPrinter::First ));
    list << q;
    return list;
}

void tkPrinterPreviewerPrivate::initialize()
{
    setupUi(this);
}


void tkPrinterPreviewerPrivate::changeEvent(QEvent *e)
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
void tkPrinterPreviewerPrivate::connectPreview( tkRichTextEditor * t )
{
    if (!t)
        return;
    if (m_AutoCheck)
        connect(t->textEdit(), SIGNAL(textChanged()), this, SLOT( on_updatePreviewButton_clicked() ));
    else
        disconnect(t->textEdit(), SIGNAL(textChanged()), this, SLOT( on_updatePreviewButton_clicked() ));
}

int tkPrinterPreviewerPrivate::headerPresence()
{
    QComboBox *c = this->findChild<QComboBox *>( HEADER );
    if (c)
        return c->currentIndex();
    return 0;

}

int tkPrinterPreviewerPrivate::footerPresence()
{
    QComboBox *c = this->findChild<QComboBox *>( FOOTER );
    if (c)
        return c->currentIndex();
    return 0;

}

int tkPrinterPreviewerPrivate::watermarkPresence()
{
    QComboBox *c = this->findChild<QComboBox *>( WATERMARK );
    if (c)
        return c->currentIndex();
    return 0;

}

void tkPrinterPreviewerPrivate::on_updatePreviewButton_clicked()
{
    QPixmap pixmap;
    printer.clearHeaders();
    printer.clearFooters();
    printer.clearWatermark();
    if (m_EditorHeader) {
        printer.setHeader( m_EditorHeader->textEdit()->toHtml(), tkPrinter::Presence(headerPresence()) );
    }
    if (m_EditorFooter) {
        printer.setFooter( m_EditorFooter->textEdit()->toHtml(), tkPrinter::Presence(footerPresence() ));
    }
    if (m_EditorWatermark) {
        printer.addHtmlWatermark( m_EditorWatermark->textEdit()->toHtml(), tkPrinter::Presence(watermarkPresence()) );
    }
    printer.previewToPixmap(pixmap, printer.printer());
    this->previewLabel->setPixmap(pixmap);
}

void tkPrinterPreviewerPrivate::on_automaticUpdateCheck_stateChanged( int checkstate )
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

void tkPrinterPreviewerPrivate::on_duplicataCheck_stateChanged( int state )
{
    if (state == Qt::Checked)
        printer.printWithDuplicata(true);
    else
        printer.printWithDuplicata(false);
    on_updatePreviewButton_clicked();
}

void tkPrinterPreviewerPrivate::on_pageNumberSpinBox_valueChanged(int value)
{
    /** \todo needs modification of tkPrinter::previewToPixmap( QPixmap &drawTo, QPrinter *printer ) to
              tkPrinter::previewToPixmap( QPixmap &drawTo, QPrinter *printer, int pageNumber )
    */
}
