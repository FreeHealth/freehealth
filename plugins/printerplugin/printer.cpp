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
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Filipe Azevedo <>                                                 *
 *       Jourdan Romain <>                                                 *
 ***************************************************************************/
#include "printer.h"
#include "printerpreviewer_p.h"
#include "textdocumentextra.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QSizeF>
#include <QRectF>
#include <QRect>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QSizeF>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QTextLayout>
#include <QTextFrame>
#include <QTextTable>
#include <QPointer>
#include <QFileInfo>

// For test
#include <QTextBrowser>
#include <QDialog>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
// End

#include <math.h>

/**
  \class PrinterPreviewer
  \brief QWidget for printing previewing. Must be instanciate by Printer::previewer().
  \ingroup toolkit widget_toolkit
 */

/**
  \class Printer
  \todo Write Documentation
*/

/**
  \enum Printer::Presence
  \brief This enum is used to define the presence of headers, footers, watermarks
*/

/**
  \var Printer::Presence Printer::EachPages
  The document is present on each pages to print
*/

/**
  \var Printer::Presence Printer::FirstPageOnly
  The document is present on the first page only
*/

/**
  \var Printer::Presence Printer::SecondPageOnly
  The document is present on the second page only
*/

/**
  \var Printer::Presence Printer::LastPageOnly
  The document is present on the last page only
*/

/**
  \var Printer::Presence Printer::ButFirstPage
  The document is present on all pages but the first
*/

/**
  \var Printer::Presence Printer::OddPages
*/

/**
  \var Printer::Presence Printer::EvenPages
*/

/**
  \var Printer::Presence Printer::DuplicataOnly
  The document is present on the duplicatas only
  \sa printDuplicata()
*/

/**
  \enum Printer::Priority
  \brief Not used for now
*/

/**
  \fn static QStringList Printer::presencesAvailable()
  \brief Returns the QStringList of the availables Presence in the order of the enum QPrinterEasy::Presence
  Usefull for populating a QComboBox for example.
*/

static const int FOOTER_BOTTOM_MARGIN = 10;

using namespace Print;
using namespace Print::Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Print {
namespace Internal {

/** \brief Only keeps some private members and private datas */
class PrinterPrivate
{
public:
    PrinterPrivate() : m_Printer(0), m_Content(0), m_WithDuplicata(false), m_PrintingDuplicata(false) {}
    ~PrinterPrivate()
    {
        if (m_Printer)
            delete m_Printer;
        m_Printer=0;
        if (m_Content)
            delete m_Content;
        m_Content = 0;
    }

    /**  */
    QTextDocument *content() { return m_Content; }
    /** \brief Destroy the content document pointer and recrete a brand new one */
    void renewContent()
    {
        if (m_Content) delete m_Content;
        m_Content = 0;
        m_Content = new QTextDocument();
    }

    /** \brief Return all known headers \sa complexDraw() */
    QList<QTextDocument*> headers(int pageNumber)
    {
        // TODO returns a QList< TextDocumentExtra *> sorted by priority
        QList<QTextDocument *> list;
        foreach( const TextDocumentExtra *doc, m_Headers ) {
            if (presenceIsRequiredAtPage(pageNumber, doc->presence()))
                list << doc->document();
        }
        return list;
    }

    /**  \brief Return the first know header of the list. \sa simpleDraw() */
    QTextDocument *header(Printer::Presence p) // Returns 0 if there is no header for the presence
    {
        Q_UNUSED(p);
        // this function should only be used by simpleDraw
        if (m_Headers.count() < 1)
            return 0;
        return m_Headers.at(0)->document();
    }

    /** \brief Returns all known footers \sa complexDraw() */
    QList<QTextDocument*> footers(int pageNumber)
    {
        // TODO returns a QList< TextDocumentExtra *> sorted by priority
        QList< QTextDocument *> list;
        foreach( const TextDocumentExtra *doc, m_Footers ) {
//            qWarning() << "tkPrinterPrivate::footers" << doc->presence();
            if (presenceIsRequiredAtPage(pageNumber, doc->presence()))
                list << doc->document();
        }
        return list;
    }

    /** \brief Return the first known footer \sa simpleDraw() */
    QTextDocument *footer(Printer::Presence p) // Returns 0 if there is no footer for the presence
    {
        Q_UNUSED(p);
        // this function should only be used by simpleDraw
        if (m_Footers.count() < 1)
            return 0;
        return m_Footers.at(0)->document();
    }

    /** \brief Return the pagewidth according to the printer. */
    int pageWidth()
    {
        if (m_Printer)
            return m_Printer->pageRect().width() - 20;
        return 0;
    }

    /** \brief Affect the width in argument to all QTextDocument. */
    void setTextWidth(int width)
    {
        if (m_Content)
            m_Content->setTextWidth(width);
        foreach (TextDocumentExtra *doc, m_Headers)
            doc->setTextWidth(width);
        foreach (TextDocumentExtra *doc, m_Footers)
            doc->setTextWidth(width);
    }

    /**  \brief Destroy the current printer and recrete a brand new one. */
    void renewPrinter()
    {
        if (m_Printer) {
            delete m_Printer;
            m_Printer=0;
        }
        m_Printer = new QPrinter;
    }

    // used by complexDraw()
    // creates a new page into the painter, recalculates all sizes and return the pageNumber of the created one.
    // all its params will be modified
    int complexDrawNewPage( QPainter &p, QSizeF & headerSize, QSizeF & footerSize,
                            QSizeF & pageSize, int & correctedY, QSizeF & drawnedSize,
                            int currentPageNumber );

    /** \brief Used by Watermark drawing functions. */
    static QRectF rotatedBoundingRect(const QRectF &rect, const int rotation)
    {
        QRectF centeredRect = rect.translated( - rect.center() );
        QPolygonF polygon(centeredRect);
        QTransform transform;
        transform.rotate(rotation);
        polygon = polygon * transform;
        return polygon.boundingRect().translated(rect.center());
    }
    /** \brief Calculate the median angle of the page */
    static double medianAngle( const QRectF & rect )
    {
        double pi = 3.14159265;
        double calculatedTang = rect.height() / rect.width();
        return -atan( calculatedTang ) * 180.0 / pi;
    }
    /** \brief calculate rotation angle of watermark using the alignment (return the angle). */
    static int calculateWatermarkRotation( const QRectF pageRect, const Qt::Alignment watermarkAlignment )
    {
        int angle = 0;
        if ( ( watermarkAlignment == (Qt::AlignHCenter | Qt::AlignVCenter) ) || (watermarkAlignment == Qt::AlignCenter ) ) {
            angle = medianAngle( pageRect );
        } else if (watermarkAlignment == Qt::AlignBottom) {
            angle = 0;
        } else if (watermarkAlignment == Qt::AlignTop) {
            angle = 0;
        } else if (watermarkAlignment == Qt::AlignRight) {
            angle = 90;
        } else if (watermarkAlignment == Qt::AlignLeft) {
            angle = 270;
        }
        return angle;
    }
    /** */
    static void moveTextRect( QRectF & textRect, const QRectF pageRect, const Qt::Alignment watermarkAlignment, const double scaleFactor = 1.0 )
    {
        double textHeight  = textRect.height() * scaleFactor;
        if ( ( watermarkAlignment == (Qt::AlignHCenter | Qt::AlignVCenter) ) || (watermarkAlignment == Qt::AlignCenter ) ) {
            textRect.moveCenter( pageRect.center() );
        } else if (watermarkAlignment == Qt::AlignBottom) {
            textRect.moveCenter( QPointF( pageRect.center().x(), (pageRect.height() - (textHeight/2)) ) );
        } else if (watermarkAlignment == Qt::AlignTop) {
            textRect.moveCenter( QPointF( pageRect.center().x(), textHeight/2 ) );
        } else if (watermarkAlignment == Qt::AlignRight) {
            textRect.moveCenter( QPointF(pageRect.width() - (textHeight/2), pageRect.center().y()) );
        } else if (watermarkAlignment == Qt::AlignLeft) {
            textRect.moveCenter( QPointF( (textHeight/2), pageRect.center().y()) );
        }
    }

    /** \brief Return true if the document must be added at page \e page assuming that document as a Presence like \e presence. */
    bool presenceIsRequiredAtPage( const int page, const int presence)
    {
        if ((presence == Printer::DuplicataOnly) && (m_PrintingDuplicata))
            return true;
        if (presence == Printer::EachPages)
            return true;
        if ( ( presence == Printer::OddPages ) && ((page % 2) == 1) )
            return true;
        if ( ( presence == Printer::EvenPages ) && ((page % 2) == 0) )
            return true;
        if ( (presence == Printer::FirstPageOnly) && (page==1) )
            return true;
        if ( (presence == Printer::SecondPageOnly) && (page==2) )
            return true;
        if ( (presence == Printer::ButFirstPage) && (page!=1) )
            return true;
        // TODO LastPageOnly need to know the pageCount of the doc too
        return false;

        return true;
    }

    /** \brief Use simpleDraw or complexDraw method ? \sa simpleDraw(), complexDraw() */
    bool isSimple() const
    {
        return ((m_Headers.count()==1) && (m_Footers.count()==1) );//&& m_Watermark.isNull());
    }

    bool simpleDraw();
    bool simpleDrawToPainter( QPainter &painter, QRect &contentRect );

    bool complexDraw();

    /** \brief Calculate the space left for the content printing when adding all headers / footers. */
    QSizeF getSimpleDrawContentPageSize()
    {
        int height = m_Printer->pageRect().height();
        bool footerMargin = false;
        foreach( QTextDocument *doc, headers(1) ) {
            height -= doc->size().height();
        }
        foreach( QTextDocument *doc, footers(1) ) {
            footerMargin = true;
            height -= doc->size().height();
        }
        if (footerMargin)
            height -= FOOTER_BOTTOM_MARGIN;
        return QSizeF( this->pageWidth(), height );
    }

    /** \brief Returns the current rectangle for the simple drawing (current rect is the rectangle left for printing the content document). */
    QRect getSimpleDrawCurrentRectangle(int pageNumber)
    {
        int headerHeight = 0;
        foreach( QTextDocument *doc, headers(pageNumber) ) {
            headerHeight += doc->size().height();
        }
        int footerHeight = FOOTER_BOTTOM_MARGIN;
        foreach( QTextDocument *doc, footers(pageNumber) ) {
            footerHeight += doc->size().height();
        }
        if (footerHeight==FOOTER_BOTTOM_MARGIN)
            footerHeight=0;
        int currentHeight = m_Printer->pageRect().height() - headerHeight - footerHeight;
        return QRect(QPoint(0,0), QSize( pageWidth(), currentHeight ) );
    }

    /** \brief Draws the headers and footers for the simpleDraw method. Painter must be translated to the beginning of the paperPage.*/
    void simpleDrawHeaderFooter( QPainter &painter,
                                  QSizeF &headerSize, QSizeF &footerSize, const int currentPageNumber )
    {
        int headerHeight = 0;
        painter.save();         // keep page beginning location
        foreach( QTextDocument *doc, headers(currentPageNumber) ) {
            headerSize = doc->size();
            headerHeight += doc->size().height();
            QRectF headRect = QRectF(QPoint(0,0), headerSize );
            doc->drawContents( &painter, headRect );
            painter.translate( 0, doc->size().height() );
        }
        headerSize.setHeight( headerHeight );
        painter.restore();    // return to page beginning

        // do we have to include the footer ?
        int footerHeight = FOOTER_BOTTOM_MARGIN;
        foreach( QTextDocument *doc, footers(currentPageNumber) ) {
            footerSize = doc->size();
            footerHeight += doc->size().height();
            painter.save();
            painter.translate(0, m_Printer->pageRect().bottom() - footerHeight);
            QRectF footRect = QRectF(QPoint(0,0), QSizeF( doc->size().width(), footerHeight) );
            doc->drawContents(&painter, footRect);
            painter.restore();
        }
        if (footerHeight==FOOTER_BOTTOM_MARGIN)
            footerHeight=0;
        footerSize.setHeight( footerHeight );
    }

private:
    /** \brief Draws the watermark for the simpleDraw method. Painter must be translated to the beginning of the paperPage. */
    void simpleDrawWatermark( QPainter &painter, const int pageNumber )
    {
        // Add watermark, painter must be translated to the beginning of the page before
        if (presenceIsRequiredAtPage(pageNumber, m_WatermarkPresence)) {
            painter.save();
            painter.drawPixmap( m_Printer->pageRect().topLeft(), m_Watermark );
            painter.restore();
        }
    }

    /**
         \brief Draws the content for the simpleDraw method.
                Painter must be translated to the beginning of the paperPage.
                Content must be sized before, otherwise pagebreaks can cut text/tables... inside a line.
                \sa getSimpleDrawContentPageSize()
    */
    void simpleDrawContent( QPainter &painter, const QSizeF &headerSize, QRect &currentRect, const int drawnedHeight)
    {
        painter.save();
        // go under the header
        painter.translate(0, headerSize.height());
        // negative translate in order to print the currentRect in the current page
        painter.translate(0, -drawnedHeight );
        currentRect.translate(0, drawnedHeight);
        m_Content->drawContents(&painter, currentRect);
        painter.restore();
    }


public:
    QPixmap m_Watermark; // null watermark at constructor time
    int m_WatermarkPresence;
    QPrinter *m_Printer;
    //    QPointer<QPrinter> m_Printer;  // QPrinter is not a QObject so it cannot be protected by QPointer....
    QList<TextDocumentExtra*> m_Headers;
    QList<TextDocumentExtra*> m_Footers;
    QTextDocument *m_Content;                             // TODO transform to QPointer<QTextDocument> ?
    bool m_WithDuplicata, m_PrintingDuplicata;
};

}  // End Internal
}  // End Print

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  PRIVATE PART  ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    \brief This drawing funtion assumes that there isn't the same number of headers and footers on each pages.
    Watermark are printed first, text lies over the watermark. Then headers and footers are added, and finally
    the content of the document to print is added page by page.
    This function is buggy : pagebreaks can cut text inside a line, idem for tables...
    \todo Needs corrections
*/
bool PrinterPrivate::complexDraw()
{
//    qWarning() << "complexDraw";
    QPainter painter(m_Printer);
    QTextFrame *frame = m_Content->rootFrame();

    // Here we have to print different documents :
    // 'content' is the main document to print
    // 'header' the header, see m_HeaderPresence to know when to add the header
    // 'footer' the footer, see m_FooterPresence to know when to add the footer

    // This function draw the content block by block
    // These values do not change during the printing process of blocks
    int _pageWidth = pageWidth();                     //TODO add margins
    setTextWidth(_pageWidth);

    // These values change during the printing process of blocks
    QSizeF pageSize, headerSize, footerSize, blockSize, actualSize, drawnedSize;
    QRectF lastDrawnedRect, blockRect;
    int correctedY = 0;
    int pageNumber = 0;

    QTextBlock block;
    painter.save();

    QTextFrame::iterator it;

    for (it = frame->begin(); !(it.atEnd()); ++it) {
        QTextFrame *table = qobject_cast<QTextTable*>( it.currentFrame() );
        block = it.currentBlock();

        if (table) {
            // calculate table height
            QRectF tableRect = m_Content->documentLayout()->frameBoundingRect(it.currentFrame());
            painter.drawRect(tableRect);
            painter.drawText(tableRect, QString("\n Tables are not yet supported in complex drawing.") );

            // need new page ?
            if ( tableRect.height() + drawnedSize.height() > pageSize.height() )
                pageNumber = complexDrawNewPage( painter, headerSize, footerSize, pageSize,
                                                 correctedY, drawnedSize, pageNumber );

            QPointF TablePos = QPointF( tableRect.x(), drawnedSize.height() );
            // get position of the table into the painter
            // draw all frames/blocks of the table
            // modify drawnedRect / actualRect...
            drawnedSize.setHeight( drawnedSize.height() + tableRect.size().height() +
                                   (tableRect.top() - lastDrawnedRect.bottom() ) );
            lastDrawnedRect = tableRect;

        } else if ( block.isValid() ) {

            blockRect = m_Content->documentLayout()->blockBoundingRect(block);

            // need new page ?
            if ( (drawnedSize.height() + blockRect.size().height()) > pageSize.height() ) {

                int i = 0;
                QTextLayout *layout = block.layout();
                if ( layout->lineCount() > 1 ) {


                    // TODO --> draw line by line


//                    qWarning() << "lines in block" << block.layout()->lineCount();
                    int heightSave = drawnedSize.height();
                    // draw the maximum lines into the page before creating a new one
                    while (layout->lineAt(i).height() + drawnedSize.height() < pageSize.height()) {
                        //                        layout->lineAt(i).draw( &painter, layout->lineAt(i).position() );
                        drawnedSize.setHeight( drawnedSize.height() + layout->lineAt(i).height());
//                        qWarning() << "draw line" << i;
                        ++i;
                    }
                    drawnedSize.setHeight( heightSave );


                    // END TODO

                }
                pageNumber = complexDrawNewPage( painter, headerSize, footerSize, pageSize,
                                                 correctedY, drawnedSize, pageNumber );
            }

            block.layout()->draw( &painter, QPointF(0,0) );

            drawnedSize.setHeight( drawnedSize.height() + blockRect.size().height() +
                                   ( blockRect.top() - lastDrawnedRect.bottom() ) );
            lastDrawnedRect = blockRect;
        }
    }
    painter.restore();
    painter.end();
    return true;
}

int PrinterPrivate::complexDrawNewPage( QPainter &p, QSizeF & headerSize, QSizeF & footerSize,
                                          QSizeF & pageSize, int & correctedY, QSizeF & drawnedSize,
                                          int currentPageNumber )
{
    bool headerDrawned = false;
    bool footerDrawned = false;

    // correctedY --> translate painter to  (0, correctedY)  in order to paint with pageRect coordonnates

    // do we have to create a newpage into printer ?
    if ( currentPageNumber != 0 ) {
        m_Printer->newPage();
        p.restore();
        int previousHeaderHeight = 0;
        foreach( QTextDocument *doc, headers(currentPageNumber) ) {
            previousHeaderHeight += doc->size().height();
        }
        p.translate( 0, -drawnedSize.height() - previousHeaderHeight );
        correctedY += drawnedSize.height();
        p.save();
        // painter points at the beginning of the page
    }

    // do we have to include a watermark ?
    if ( presenceIsRequiredAtPage( currentPageNumber+1 , m_WatermarkPresence ) ) {
        p.save();
        p.translate(0, correctedY );
        p.drawPixmap( m_Printer->pageRect(), m_Watermark );
        p.restore();
    }

    // do we have to include the header ?
    int specialY = correctedY;
    int headerHeight = 0;
    foreach( QTextDocument *doc, headers(currentPageNumber + 1) ) {
        headerSize = doc->size();
        // draw all headers
        p.save();
        p.translate(0, specialY );
        specialY = 0;
        headerHeight += doc->size().height();
        QRectF headRect = QRectF(QPoint(0,0), headerSize );
        doc->drawContents( &p, headRect );
        p.restore();
        // translate painter under the header
        p.restore();
        p.translate( 0, doc->size().height() );
        p.save();
        headerDrawned = true;
    }
    headerSize.setHeight( headerHeight );


    // do we have to include the footer ?
    int footHeight = 0;
    foreach( QTextDocument *doc, footers(currentPageNumber + 1) ) {
        footerSize = QSizeF(doc->size().width(),0);
        footHeight += doc->size().height();
        p.save();
        p.translate(0, m_Printer->pageRect().bottom() + correctedY - footHeight - headerSize.height());
        QRectF footRect = QRectF(QPoint(0,0), QSizeF( doc->size().width(), footHeight) );
        doc->drawContents(&p, footRect);
        p.restore();
        footerDrawned = true;
    }
    footerSize.setHeight( footHeight );

    // recalculate the content size of the content page
    pageSize = QSizeF( pageWidth(),
                       m_Printer->pageRect().height() - headerSize.height() - footerSize.height());

    // reset drawnedSize (nothing is drawned into the new page)
    drawnedSize = QSizeF(0,0);

    return currentPageNumber + 1;
}

/**
    \brief This drawing funtion assumes that there is the same number of headers and footers on each pages.
    Watermark are printed first, text lies over the watermark. Then headers and footers are added, and finally
    the content of the document to print is added page by page.
*/
bool PrinterPrivate::simpleDraw()
{
    if (!m_Content) {
        Utils::Log::addError( "tkPrinter", QCoreApplication::translate("tkPrinter", "No content to preview (simpleDraw).") );
        return false;
    }
    m_PrintingDuplicata = false;
    // All pages measures
    // pageWidth, margins of document, margins of printer
    int _pageWidth = pageWidth();                     //TODO add margins
    this->setTextWidth(_pageWidth);

    m_Content->setPageSize( getSimpleDrawContentPageSize() );
//    qWarning() << getSimpleDrawContentPageSize();
    m_Content->setUseDesignMetrics(true);

    // prepare drawing areas
    QRect contentRect = QRect(QPoint(0,0), m_Content->size().toSize() );     // whole document drawing rectangle

    // prepare painter then draw
    QPainter painter(m_Printer);
    return simpleDrawToPainter(painter, contentRect);
}

/**
  \brief Draws to the painter the headers/footers/watermarks and the content to the QPainter \e painter using the simpleDraw method.
  \sa isSimple(), simpleDraw(), Printer::previewToPixmap()
*/
bool PrinterPrivate::simpleDrawToPainter( QPainter &painter, QRect &contentRect )
{
    int _pageWidth = contentRect.size().width();
    QSizeF headerSize(_pageWidth, 0);
    QSizeF footerSize(_pageWidth, 0);
    int pageNumber = 1;
    int drawnedHeight = 0;
    QRectF headRect = QRectF(QPoint(0,0), headerSize );
    QRect currentRect = contentRect;
    int fromPage = m_Printer->fromPage();
    int toPage = m_Printer->toPage();
    bool fromToPage = ((fromPage>0) || (toPage>0));
    while (currentRect.intersects(contentRect)) {
        currentRect = getSimpleDrawCurrentRectangle(pageNumber);
        if (fromToPage) {
            if (pageNumber>toPage)
                break;
            if (pageNumber<fromPage) {
                drawnedHeight += currentRect.height();
                pageNumber++;
                continue;
            }
        }
        // at the beginning of the while, painter is translated to the 0,0 position of the new page
        simpleDrawWatermark( painter, pageNumber );
        simpleDrawHeaderFooter( painter, headerSize, footerSize, pageNumber );

        // draw content for this page
        simpleDrawContent(painter, headerSize, currentRect, drawnedHeight);

        // calculate new page
        // go to content next page
        if (m_WithDuplicata) {
            if (m_PrintingDuplicata) {
                drawnedHeight += currentRect.height();
                // duplicata is drawn --> translate the currentRect to the beginning of the next page
                currentRect.translate(0, currentRect.height() );
                pageNumber++;
            }
            m_PrintingDuplicata=!m_PrintingDuplicata;
        } else {
            drawnedHeight += currentRect.height();
            // translate the currentRect to the beginning of the next page
            currentRect.translate(0, currentRect.height());
            pageNumber++;
        }

        // if there is still something to print --> create a newpage to the printer
        if (currentRect.intersects(contentRect)) {
            if (fromToPage) {
                if ((pageNumber>=fromToPage) && (pageNumber<=toPage))
                    m_Printer->newPage();
            } else
                    m_Printer->newPage();
        }
    }
    painter.end();
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  PUBLIC PART  ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
Printer::Printer( QObject * parent )
    : QObject(parent),
    d(0)
{
    setObjectName("Printer");
    d = new PrinterPrivate;
    Q_ASSERT(d);
}

Printer::~Printer()
{
    clearHeaders();
    clearFooters();
    if (d) delete d;
    d = 0;
}

/** \brief Deletes all headers. */
void Printer::clearHeaders()
{
    qDeleteAll(d->m_Headers);
    d->m_Headers.clear();
}

/** \brief Deletes all footers. */
void Printer::clearFooters()
{
    qDeleteAll(d->m_Footers);
    d->m_Footers.clear();
}

/** \brief Clear all watermark already passed. */
void Printer::clearWatermark()
{
    d->m_Watermark = QPixmap();
    d->m_WatermarkPresence = Printer::EachPages;
}

/** \brief Get the printer as setted in application's settings */
bool Printer::getUserPrinter()
{
    delete d->m_Printer;
    d->m_Printer = 0;
    QString name = settings()->value(Constants::S_DEFAULT_PRINTER).toString();
    if (name == "System") {
        d->m_Printer = new QPrinter(QPrinterInfo::defaultPrinter(),
                                    QPrinter::PrinterMode(settings()->value(Constants::S_RESOLUTION).toInt()));
    } else if (name == "User") {
        askForPrinter(qApp->activeWindow());
    } else {
        foreach(const QPrinterInfo &info, QPrinterInfo::availablePrinters()) {
            if (info.printerName() == name) {
                d->m_Printer = new QPrinter(info,
                                            QPrinter::PrinterMode(settings()->value(Constants::S_RESOLUTION).toInt()));
                break;
            }
        }
    }
    if (d->m_Printer) {
        d->m_Printer->setColorMode(QPrinter::ColorMode(settings()->value(Constants::S_COLOR_PRINT).toInt()));
        return true;
    }
    return false;
}

/**
  \brief Shows the print dialog, and ask user to select the printer to use.
  The printing process is not started.
*/
bool Printer::askForPrinter(QWidget *parent)
{
    d->renewPrinter();
    QPrintDialog dialog(d->m_Printer, parent);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() == QDialog::Accepted)
        return true;
    return false;
}

/** \brief Defines the printer to use. */
void Printer::setPrinter( QPrinter * printer )
{
    if (printer)
        d->m_Printer = printer;
    else
        d->renewPrinter();
}

/** \brief Returns the printer in use. */
QPrinter *Printer::printer()
{
    return d->m_Printer;
}

/**
  \brief Set a header content, presence and priority.
*/
void Printer::setHeader( const QString & html, Presence presence, Printer::Priority prior )
{
    d->m_Headers.append(new TextDocumentExtra(html, presence, prior));
}

/**
  \brief Set a footer content, presence and priority.
*/
void Printer::setFooter( const QString & html, Presence presence, Printer::Priority prior )
{
    d->m_Footers.append(new TextDocumentExtra(html, presence, prior));
}

/** \brief Set the main text to print/ */
void Printer::setContent( const QString & html )
{
    d->renewContent();
    d->content()->setHtml( html );
}

/** \brief Try to find the default system printer and use it without annoying user. */
bool Printer::useDefaultPrinter()
{
    // TODO : to test
    QPrinterInfo def = QPrinterInfo::defaultPrinter();
    if (def.isNull())
        return false;
    if (d->m_Printer) {
        delete d->m_Printer;
        d->m_Printer = 0;
    }
    d->m_Printer = new QPrinter(def, QPrinter::PrinterResolution);
    return true;
}

void Printer::setOrientation(QPrinter::Orientation orientation)
{
    if (!d->m_Printer)
        d->renewPrinter();
    d->m_Printer->setOrientation(orientation);
    // TODO Manage Margins
    d->setTextWidth( d->pageWidth() );
}

void Printer::setPaperSize(QPrinter::PaperSize size)
{
    if (!d->m_Printer)
        d->renewPrinter();
    d->m_Printer->setPaperSize(size);
    // TODO Manage Margins
    d->setTextWidth(d->pageWidth());
}

/** \brief Shows the print preview dialog. \e test param should only be used for debugging. */
bool Printer::previewDialog(QWidget *parent, bool test)
{
    if (!d->m_Printer)
        d->m_Printer = new QPrinter(QPrinter::ScreenResolution);

    if (Utils::isDebugCompilation()) {
        // For test
        if (test) {
            QStringList list;
            list << d->content()->toHtml() << d->header(EachPages)->toHtml() <<  d->footer(EachPages)->toHtml();
            Utils::quickDebugDialog(list);
        }
    } else {
        Q_UNUSED(test);
    }

    QPrintPreviewDialog dialog(d->m_Printer, parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    connect(&dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(print(QPrinter *)));
    dialog.exec();
    return true;
}

/**
  \brief Print the documents as the content (headers, footers and watermark must be defined before).
*/
bool Printer::print( const QTextDocument & docToPrint )
{
    d->renewContent();
    d->m_Content = docToPrint.clone();
    print( d->m_Printer );
    return true;
}

/**
  \brief Print the documents as the content (headers, footers and watermark must be defined before).
*/
bool Printer::print( const QString &htmlToPrint )
{
    QTextDocument t;
    t.setHtml( htmlToPrint );
    return print( t );
}

/**
  \brief Inform the printing engine that it must print duplicatas too.
  If set to true, the duplicatas are printed just after the page, not at the end of the whole printing.\n
  Setting your printer driver to 2n-up should be fine to print one the left the original and on the right the duplicata.
*/
bool Printer::printWithDuplicata( bool state )
{
    d->m_WithDuplicata=state;
    return true;
}

/**
  \brief Print all documents (headers, footers, watermark and content) to the printer \e printer
  This member is used by QPrintDialogPreview in order to refresh the preview.\n
  You should not use this function directly.
  \sa print( const QTextDocument &), print( const QString &)
*/
bool Printer::print( QPrinter *printer )
{
    if (!printer)
        printer = d->m_Printer;

    if (!printer)
        return false;

    if (d->isSimple()) {
        Utils::Log::addMessage(this, "Printing using simpleDraw method.");
        return d->simpleDraw();
    }
    else {
        Utils::Log::addMessage(this, "WARNING : Printing using complexDraw method (should be buggy).");
        return d->complexDraw();
    }
}

/** \brief Defines the QPixmap to use for the Watermark */
void Printer::addPixmapWatermark( const QPixmap & pix, const Presence p , const Qt::AlignmentFlag watermarkAlignment )
{
    if ( ! d->m_Printer )
        return;
    d->m_WatermarkPresence = p;
    QRectF pageRect = d->m_Printer->pageRect();

    // prepare watermark pixmap
    if (d->m_Watermark.isNull()) {
        d->m_Watermark = QPixmap( pageRect.width(), pageRect.height() );
        d->m_Watermark.fill();
    }

    // TODO page margins
    // TODO manageDPI of pixmap
    QRectF pixRect = pix.rect();
    int rotationAngle = d->calculateWatermarkRotation( pageRect, watermarkAlignment );

    // Prepare painter
    QPainter painter;
    painter.begin( &d->m_Watermark );
    painter.translate( -pageRect.topLeft() );  // TODO : this is wrong because we loose the margins
    painter.save();
    // rotate the painter from its middle
    if ( rotationAngle != 0 ) {
        painter.translate( pixRect.center() );
        painter.rotate( rotationAngle );
        // scale textRect to feet inside the pageRect - margins
        QRectF boundingRect = d->rotatedBoundingRect(pixRect, rotationAngle);
        double scale = qMin( pageRect.width() / boundingRect.width(), pageRect.height() / boundingRect.height() );
        painter.scale( scale, scale );
        painter.translate( -pixRect.center() );
    }
    painter.drawRect( pixRect );
    painter.drawPixmap( pixRect, pix, QRectF() );
    painter.restore();
    painter.end();
}

/** \brief Defines the html text to use for the Watermark */
void Printer::addHtmlWatermark( const QString & html,
                                  const Presence p,
                                  const Qt::Alignment watermarkAlignment,
                                  const int orientation )
{
    if ( ! d->m_Printer )
        return;
    d->m_WatermarkPresence = p;

    // TODO Manage page margins +++

    // get some values about the printing page and prepare the pixmap
    QRectF pageRect = d->m_Printer->pageRect();

    d->m_Watermark = QPixmap( pageRect.width(), pageRect.height() );
    d->m_Watermark.fill();
    previewHtmlWatermark(d->m_Watermark, html, p, watermarkAlignment, orientation );
}

/**
  \brief Draws to the QPixmap \e drawTo the watermark defined by the QTextDocument \e doc.
  \todo When asking multi watermarks, is watermarks does not have the same presence --> Bug. Need to store d->watermarks into a hash<presence,qpixmap>
*/
void Printer::previewDocumentWatermark( QPixmap &drawTo,
                                          QTextDocument *doc,
                                          const Presence p,
                                          const Qt::Alignment watermarkAlignment,
                                          const int orientation )
{
    Q_UNUSED(p);
    Q_UNUSED(orientation);
    // TODO Manage page margins +++
    // drawTo MUST BE SIZED AS WANTED BEFORE CALLING THIS FUNCTION ++++
    // drawTo.fill();
    QSizeF docSizeSave = doc->size();
    QTextOption docOptionSave = doc->defaultTextOption();
    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    doc->setDefaultTextOption(opt);
    doc->adjustSize();
    QPointF pageCenter( drawTo.rect().center() );
    QRect pageRect = drawTo.rect();
    // Calculates the painting area for the text
    QRectF textRect = QRectF( QPointF(0,0), doc->size() );
    int rotationAngle = PrinterPrivate::calculateWatermarkRotation( pageRect, watermarkAlignment );

    // Prepare painter
    QPainter painter;
    painter.begin( &drawTo );
    painter.translate( -pageRect.topLeft() );  // TODO : this is wrong because we loose the margins
    painter.save();
    // rotate the painter from its middle
//    if ( rotationAngle != 0 ) {
    {
        QRectF boundingRect = PrinterPrivate::rotatedBoundingRect(textRect, rotationAngle);
        double scale = qMin( pageRect.width() / boundingRect.width(), pageRect.height() / boundingRect.height() );
        PrinterPrivate::moveTextRect(textRect, pageRect, watermarkAlignment, scale );
        painter.translate( textRect.center() );
        painter.rotate( rotationAngle );
        painter.scale( scale, scale );
        // scale textRect to feet inside the pageRect - margins
        painter.translate( -textRect.center() );
    }
//    else {
//        QSizeF docSize = wm.size();
////    painter.translate( textRect.topLeft() );
//        double scale = qMin( pageRect.width() / docSize.width(), pageRect.height() / docSize.height() );
//        tkPrinterPrivate::moveTextRect(textRect, pageRect, watermarkAlignment, scale );
//        painter.scale( scale, scale );
////    painter.translate( -textRect.topLeft() );
//    }

    painter.translate( textRect.topLeft() );
    doc->drawContents( &painter );//, textRect );
    painter.translate( -textRect.topLeft() );

    painter.restore();
    painter.end();
    doc->setDefaultTextOption( docOptionSave );
    doc->setPageSize( docSizeSave );
}

/** \brief Draws the \e html text watermark to QPixmap \e drawTo */
void Printer::previewHtmlWatermark( QPixmap &drawTo,
                                      const QString & html,
                                      const Presence p,
                                      const Qt::Alignment watermarkAlignment,
                                      const int orientation )
{
    QTextDocument wm;
    wm.setHtml( html );
    previewDocumentWatermark( drawTo, &wm, p, watermarkAlignment, orientation );
}

/** \brief Draws the plain text \e plainText watermark to QPixmap \e drawTo */
void Printer::previewTextWatermark( QPixmap &drawTo,
                                      const QString & plainText,
                                      const Presence p,
                                      const Qt::Alignment watermarkAlignment,
                                      const int orientation )
{
    QTextDocument wm;
    wm.setPlainText( plainText );
    previewDocumentWatermark( drawTo, &wm, p, watermarkAlignment, orientation );
}

/** \brief Defines the plain text to use for the Watermark */
void Printer::addTextWatermark( const QString & plainText,
                                  const Presence p,
                                  const Qt::Alignment watermarkAlignment,
                                  const Qt::Alignment textAlignment,
                                  const QFont & font,
                                  const QColor & color,
                                  const int orientation )
{
    if ( ! d->m_Printer )
        return;
    d->m_WatermarkPresence = p;

    // TODO Manage page margins +++

    // get some values about the printing page and prepare the pixmap
    QRectF pageRect = d->m_Printer->pageRect();

    if (d->m_Watermark.isNull()) {
        d->m_Watermark = QPixmap( pageRect.width(), pageRect.height() );
        d->m_Watermark.fill();
    }

    QString html = QString("<html><body><p %1 style=\"%2\">%3</p></body></html>")
                   .arg(Utils::textAlignmentToHtml(textAlignment))
                   .arg(Utils::fontToHtml(font, color))
                   .arg(plainText);
    html.replace("\n", "<br/>");
    previewHtmlWatermark(d->m_Watermark, html, p, watermarkAlignment, orientation );
}

/**
  \brief Returns the Previewer widget
  \sa tkPrinterPreviewer
*/
PrinterPreviewer *Printer::previewer( QWidget *parent )
{
    PrinterPreviewerPrivate *prev= new PrinterPreviewerPrivate(parent);
    prev->initialize();
    return prev;
}

/**
  \brief Draw the header, footer and watermark used by this actual class into \e drawTo.
  \e drawTo must be sized before.\n
  You must specify the printer to use for the previewing. This function does not use the class printer.
*/
void Printer::previewToPixmap( QPixmap &drawTo, QPrinter *printer )
{
    Q_ASSERT(printer);
    QSize savePixSize = drawTo.size();
    if (!d->m_Content) {
        d->m_Content = new QTextDocument(this);
        d->m_Content->setHtml( "<p>This is the previewing mode</p><p&nbsp;</p><p>This is the previewing mode</p><p&nbsp;</p><p>This is the previewing mode</p><p&nbsp;</p>" );
    }
    d->m_PrintingDuplicata = false;

    int _pageWidth = printer->pageRect().width();//d->pageWidth();
    d->setTextWidth(_pageWidth);

    d->m_Content->setPageSize( printer->pageRect().size() );//d->getSimpleDrawContentPageSize() );
    d->m_Content->setUseDesignMetrics(true);

    // prepare drawing areas
    QRect contentRect = QRect(QPoint(0,0), d->m_Content->size().toSize() );     // whole document drawing rectangle

    // prepare painter then draw
    drawTo = QPixmap( _pageWidth, printer->pageRect().height() + 30 );
    drawTo.fill();
    QPainter painter;
    painter.begin(&drawTo);
    d->simpleDrawToPainter(painter, contentRect);
    drawTo = drawTo.scaled( 250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation );
}

/** \brief Draw the header and footer passed as params into \e drawTo. \e drawTo must be sized before. */
void Printer::previewHeaderFooter( QPixmap &drawTo,
                                     const QString &headerHtml,
                                     const QString &footerHtml )
{
    QSize savePixSize = drawTo.size();
    // prepare pseudo printer
    QPrinter *printer = new QPrinter;
    printer->setPaperSize( QPrinter::A4 );
    setPrinter(printer);

    setHeader(headerHtml);
    setFooter(footerHtml);
    setContent("<html><body><p>xxxxxx xx xxxxx xxx xxxxx xxx xx xxxx<br />xxxxx xxxx xx x xxxxx xx xxxxxx x x x xxx x</p></body></html>");

    // prepare header/footer
    int _pageWidth = d->pageWidth();                     //TODO add margins
    QSizeF headerSize(_pageWidth, 0);
    QSizeF footerSize(_pageWidth, 0);

    QTextDocument *headerDoc = d->header(Printer::EachPages);
    if (headerDoc) {
        headerDoc->setTextWidth( _pageWidth );
        headerSize.setHeight(headerDoc->size().height());
    }
    QTextDocument *footerDoc = d->footer(Printer::EachPages);
    if (footerDoc) {
        footerDoc->setTextWidth( _pageWidth );
        footerSize.setHeight(footerDoc->size().height());
    }

    // prepare painter
    drawTo = QPixmap( _pageWidth, printer->pageRect().height() );
    drawTo.fill();
    QPainter painter;
    painter.begin(&drawTo);
//    painter.translate( 11, 11);
    d->simpleDrawHeaderFooter( painter, headerSize, footerSize, 1 );
    // scale painter
    // close painter
    painter.end();
    drawTo = drawTo.scaled( 250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation );
}

/**
  \brief Prints the recorded headers/footers/watermarks and content to a PDF file named \e fileName.
  \e fileName must be an absolute path file name and will be replaced without warning if it already exists.
  The QPrinter MUST BE specified FIRST !
*/
bool Printer::toPdf(const QString &fileName)
{
    Q_ASSERT(d->m_Printer);
    if (!d->m_Printer)
        return false;
    Q_ASSERT(!fileName.isEmpty());
    if (fileName.isEmpty())
        return false;
    QString tmp = fileName;
    if (QFileInfo(tmp).suffix().isEmpty())
        tmp.append(".pdf");
    QPrinter::OutputFormat out = d->m_Printer->outputFormat();
    d->m_Printer->setOutputFormat(QPrinter::PdfFormat);
    d->m_Printer->setOutputFileName(tmp);
    print();
    d->m_Printer->setOutputFormat(out);
    return true;
}

/**
  \brief Prints the \e html content with the recorded headers/footers/watermarks to a PDF file named \e fileName.
  \e fileName must be an absolute path file name and will be replaced without warning if it already exists.\n
  This function replaces the actual content of this class.
  The QPrinter MUST BE specified FIRST !
*/
bool Printer::toPdf(const QString &fileName, const QString &html)
{
    d->m_Content->setHtml(html);
    return toPdf(fileName);
}

/**
  \brief Prints the \e docToPrint document with the recorded headers/footers/watermarks to a PDF file named \e fileName.
  \e fileName must be an absolute path file name and will be replaced without warning if it already exists.
  This function replaces the actual content of this class.
  The QPrinter MUST BE specified FIRST !
*/
bool Printer::toPdf(const QString &fileName, const QTextDocument & docToPrint)
{
    d->m_Content->setHtml( docToPrint.toHtml() );
    return toPdf(fileName);
}
