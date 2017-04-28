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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
#include <QPicture>
#include <QSizeF>
#include <QRectF>
#include <QRect>
#include <QSizeF>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QTextLayout>
#include <QTextFrame>
#include <QTextTable>
#include <QPointer>
#include <QFileInfo>

#include <QtPrintSupport/QPrinterInfo>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>

// For test
#include <QTextBrowser>
#include <QDialog>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
// End

#include <math.h>

/**
  \class Print::PrinterPreviewer
  \brief QWidget for printing previewing. Must be instanciate by Printer::previewer().
 */

/**
  \class Print::Printer
  \todo Write Documentation
*/

/**
  \enum Print::Printer::Presence
  \brief This enum is used to define the presence of headers, footers, watermarks
*/

/**
  \var Print::Printer::Presence Printer::EachPages
  The document is present on each page to print
*/

/**
  \var Print::Printer::Presence Printer::FirstPageOnly
  The document is present on the first page only
*/

/**
  \var Print::Printer::Presence Printer::SecondPageOnly
  The document is present on the second page only
*/

/**
  \var Print::Printer::Presence Printer::LastPageOnly
  The document is present on the last page only
*/

/**
  \var Print::Printer::Presence Printer::ButFirstPage
  The document is present on all pages but the first
*/

/**
  \var Print::Printer::Presence Printer::OddPages
*/

/**
  \var Print::Printer::Presence Printer::EvenPages
*/

/**
  \var Print::Printer::Presence Printer::DuplicataOnly
  The document is present on the duplicatas only
  \sa printDuplicata()
*/

/**
  \enum Print::Printer::Priority
  \brief Not used for now
*/

/**
  \fn static QStringList Print::Printer::presencesAvailable()
  \brief Returns the QStringList of the availables Presence in the order of the enum QPrinterEasy::Presence
  useful for populating a QComboBox for example.
*/

static const int FOOTER_BOTTOM_MARGIN = 15;

using namespace Print;
using namespace Print::Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Print {
namespace Internal {

/** \brief Only keeps some private members and private data */
class PrinterPrivate
{
public:
    PrinterPrivate(Printer */*parent*/) :
        m_WatermarkPresence(-1),
        m_TwoNUp(false),
        m_Printer(0),
        m_Content(0),
        m_WithDuplicata(false),
        m_PrintingDuplicata(false)  //, q(parent)
    {
        m_TwoNUp = settings()->value(Print::Constants::S_TWONUP).toBool();
    }
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
        foreach(const TextDocumentExtra *doc, m_Headers) {
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
        foreach(const TextDocumentExtra *doc, m_Footers) {
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
            return m_Printer->paperRect().width() - 20;
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
        m_Printer->setColorMode(QPrinter::ColorMode(settings()->value(Constants::S_COLOR_PRINT).toInt()));
        m_Printer->setPaperSize(QPrinter::A4);
    }

    // used by complexDraw()
    // creates a new page into the painter, recalculates all sizes and return the pageNumber of the created one.
    // all its params will be modified
    int complexDrawNewPage(QPainter &p, QSizeF & headerSize, QSizeF & footerSize,
                            QSizeF & pageSize, int & correctedY, QSizeF & drawnedSize,
                            int currentPageNumber);

    /** \brief Used by Watermark drawing functions. */
    static QRectF rotatedBoundingRect(const QRectF &rect, const int rotation)
    {
        QRectF centeredRect = rect.translated(- rect.center());
        QPolygonF polygon(centeredRect);
        QTransform transform;
        transform.rotate(rotation);
        polygon = polygon * transform;
        return polygon.boundingRect().translated(rect.center());
    }
    /** \brief Calculate the median angle of the page */
    static double medianAngle(const QRectF & rect)
    {
        double pi = 3.14159265;
        double calculatedTang = rect.height() / rect.width();
        return -atan(calculatedTang) * 180.0 / pi;
    }
    /** \brief calculate rotation angle of watermark using the alignment (return the angle). */
    static int calculateWatermarkRotation(const QRectF paperRect, const Qt::Alignment watermarkAlignment)
    {
        int angle = 0;
        if ((watermarkAlignment == (Qt::AlignHCenter | Qt::AlignVCenter)) || (watermarkAlignment == Qt::AlignCenter)) {
            angle = medianAngle(paperRect);
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
    static void moveTextRect(QRectF & textRect, const QRectF paperRect, const Qt::Alignment watermarkAlignment, const double scaleFactor = 1.0)
    {
        double textHeight  = textRect.height() * scaleFactor;
        if ((watermarkAlignment == (Qt::AlignHCenter | Qt::AlignVCenter)) || (watermarkAlignment == Qt::AlignCenter)) {
            textRect.moveCenter(paperRect.center());
        } else if (watermarkAlignment == Qt::AlignBottom) {
            textRect.moveCenter(QPointF(paperRect.center().x(), (paperRect.height() - (textHeight/2))));
        } else if (watermarkAlignment == Qt::AlignTop) {
            textRect.moveCenter(QPointF(paperRect.center().x(), textHeight/2));
        } else if (watermarkAlignment == Qt::AlignRight) {
            textRect.moveCenter(QPointF(paperRect.width() - (textHeight/2), paperRect.center().y()));
        } else if (watermarkAlignment == Qt::AlignLeft) {
            textRect.moveCenter(QPointF((textHeight/2), paperRect.center().y()));
        }
    }

    /** \brief Return true if the document must be added at page \e page assuming that document as a Presence like \e presence. */
    bool presenceIsRequiredAtPage(const int page, const int presence)
    {
        if ((presence == Printer::DuplicatesOnly) && (m_PrintingDuplicata))
            return true;
        if (presence == Printer::EachPages)
            return true;
        if ((presence == Printer::OddPages) && ((page % 2) == 1))
            return true;
        if ((presence == Printer::EvenPages) && ((page % 2) == 0))
            return true;
        if ((presence == Printer::FirstPageOnly) && (page==1))
            return true;
        if ((presence == Printer::SecondPageOnly) && (page==2))
            return true;
        if ((presence == Printer::ButFirstPage) && (page!=1))
            return true;
        // TODO LastPageOnly need to know the pageCount of the doc too
        return false;

        return true;
    }

    /** \brief Use simpleDraw or complexDraw method ? \sa simpleDraw(), complexDraw() */
    bool isSimple() const
    {
        return ((m_Headers.count()==1) && (m_Footers.count()==1));//&& m_Watermark.isNull());
    }

    bool simpleDraw();
    bool simpleDrawPreparePages(QRect &contentRect);

    bool complexDraw();

    /** \brief Calculate the space left for the content printing when adding all headers / footers. */
    QSizeF getSimpleDrawContentPageSize()
    {
        int height = m_Printer->paperRect().height();
        bool footerMargin = false;
        foreach(QTextDocument *doc, headers(1)) {
            height -= doc->size().height();
        }
        foreach(QTextDocument *doc, footers(1)) {
            footerMargin = true;
            height -= doc->size().height();
        }
        if (footerMargin)
            height -= FOOTER_BOTTOM_MARGIN;
        return QSizeF(this->pageWidth(), height);
    }

    /** \brief Returns the current rectangle for the simple drawing (current rect is the rectangle left for printing the content document). */
    QRect getSimpleDrawCurrentRectangle(int pageNumber)
    {
        int headerHeight = 0;
        foreach(QTextDocument *doc, headers(pageNumber)) {
            headerHeight += doc->size().height();
        }
        int footerHeight = FOOTER_BOTTOM_MARGIN;
        foreach(QTextDocument *doc, footers(pageNumber)) {
            footerHeight += doc->size().height();
        }
        if (footerHeight==FOOTER_BOTTOM_MARGIN)
            footerHeight=0;
        int currentHeight = m_Printer->paperRect().height() - headerHeight - footerHeight;
        return QRect(QPoint(0,0), QSize(pageWidth(), currentHeight));
    }

    /** \brief Draws the headers and footers for the simpleDraw method. Painter must be translated to the beginning of the paperPage.*/
    void simpleDrawHeaderFooter(QPainter &painter,
                                  QSizeF &headerSize, QSizeF &footerSize, const int currentPageNumber)
    {
        int headerHeight = 0;
        painter.save();         // keep page beginning location
        foreach(QTextDocument *doc, headers(currentPageNumber)) {
            headerSize = doc->size();
            headerHeight += doc->size().height();
            QRectF headRect = QRectF(QPoint(0,0), headerSize);
            doc->drawContents(&painter, headRect);
            painter.translate(0, doc->size().height());
        }
        headerSize.setHeight(headerHeight);
        painter.restore();    // return to page beginning

        // do we have to include the footer ?
        int footerHeight = FOOTER_BOTTOM_MARGIN;
        foreach(QTextDocument *doc, footers(currentPageNumber)) {
            footerSize = doc->size();
            footerHeight += doc->size().height();
            painter.save();
            painter.translate(0, m_Printer->paperRect().bottom() - footerHeight);
            QRectF footRect = QRectF(QPoint(0,0), QSizeF(doc->size().width(), footerHeight));
            doc->drawContents(&painter, footRect);
            painter.restore();
        }
        if (footerHeight==FOOTER_BOTTOM_MARGIN)
            footerHeight=0;
        footerSize.setHeight(footerHeight);
    }

private:
    /** \brief Draws the watermark for the simpleDraw method. Painter must be translated to the beginning of the paperPage. */
    void simpleDrawWatermark(QPainter &painter, const int pageNumber)
    {
        // Add watermark, painter must be translated to the beginning of the page before
        if (presenceIsRequiredAtPage(pageNumber, m_WatermarkPresence)) {
            painter.save();
            painter.drawPixmap(m_Printer->paperRect().topLeft(), m_Watermark);
            painter.restore();
        }
    }

    /**
         \brief Draws the content for the simpleDraw method.
                Painter must be translated to the beginning of the paperPage.
                Content must be sized before, otherwise pagebreaks can cut text/tables... inside a line.
                \sa getSimpleDrawContentPageSize()
    */
    void simpleDrawContent(QPainter &painter, const QSizeF &headerSize, QRect &currentRect, const int drawnedHeight)
    {
        painter.save();
        // go under the header
        painter.translate(0, headerSize.height());
        // negative translate in order to print the currentRect in the current page
        painter.translate(0, -drawnedHeight);
        currentRect.translate(0, drawnedHeight);
        m_Content->drawContents(&painter, currentRect);
        painter.restore();
    }


public:
    QPixmap m_Watermark; // null watermark at constructor time
    int m_WatermarkPresence;
    bool m_TwoNUp;
    QPrinter *m_Printer;
    //    QPointer<QPrinter> m_Printer;  // QPrinter is not a QObject so it cannot be protected by QPointer....
    QList<TextDocumentExtra*> m_Headers;
    QList<TextDocumentExtra*> m_Footers;
    QTextDocument *m_Content;                             // TODO transform to QPointer<QTextDocument> ?
    bool m_WithDuplicata, m_PrintingDuplicata;
    QList<QPicture *> m_Pages;

private:
//    Printer *q;
};

}  // End Internal
}  // End Print

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  PRIVATE PART  //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
    \brief This drawing funtion assumes that there isn't the same number of
    headers and footers on each pages.
    Watermark are printed first, text lies over the watermark. Then headers and
    footers are added, and finally the content of the document to print is added
    page by page.
    This function is buggy : pagebreaks can cut text inside a line, same for
    tables.
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
        QTextFrame *table = qobject_cast<QTextTable*>(it.currentFrame());
        block = it.currentBlock();

        if (table) {
            // calculate table height
            QRectF tableRect = m_Content->documentLayout()->frameBoundingRect(it.currentFrame());
            painter.drawRect(tableRect);
            painter.drawText(tableRect, QString("\n Tables are not yet supported in complex drawing."));

            // need new page ?
            if (tableRect.height() + drawnedSize.height() > pageSize.height())
                pageNumber = complexDrawNewPage(painter, headerSize, footerSize, pageSize,
                                                 correctedY, drawnedSize, pageNumber);

//            QPointF TablePos = QPointF(tableRect.x(), drawnedSize.height());
            // get position of the table into the painter
            // draw all frames/blocks of the table
            // modify drawnedRect / actualRect...
            drawnedSize.setHeight(drawnedSize.height() + tableRect.size().height() +
                                   (tableRect.top() - lastDrawnedRect.bottom()));
            lastDrawnedRect = tableRect;

        } else if (block.isValid()) {

            blockRect = m_Content->documentLayout()->blockBoundingRect(block);

            // need new page ?
            if ((drawnedSize.height() + blockRect.size().height()) > pageSize.height()) {

                QTextLayout *layout = block.layout();
                if (layout->lineCount() > 1) {
                    // TODO --> draw line by line
//                    qWarning() << "lines in block" << block.layout()->lineCount();
                    int heightSave = drawnedSize.height();
                    // draw the maximum lines into the page before creating a new one
                    int i = 0;
                    while (layout->lineAt(i).height() + drawnedSize.height() < pageSize.height()) {
                        //                        layout->lineAt(i).draw(&painter, layout->lineAt(i).position());
                        drawnedSize.setHeight(drawnedSize.height() + layout->lineAt(i).height());
//                        qWarning() << "draw line" << i;
                        ++i;
                    }
                    drawnedSize.setHeight(heightSave);
                    // END TODO
                }
                pageNumber = complexDrawNewPage(painter, headerSize, footerSize, pageSize,
                                                 correctedY, drawnedSize, pageNumber);
            }

            block.layout()->draw(&painter, QPointF(0,0));

            drawnedSize.setHeight(drawnedSize.height() + blockRect.size().height() +
                                   (blockRect.top() - lastDrawnedRect.bottom()));
            lastDrawnedRect = blockRect;
        }
    }
    painter.restore();
    painter.end();
    return true;
}

int PrinterPrivate::complexDrawNewPage(QPainter &p, QSizeF & headerSize, QSizeF & footerSize,
                                          QSizeF & pageSize, int & correctedY, QSizeF & drawnedSize,
                                          int currentPageNumber)
{
//    bool headerDrawned = false;
//    bool footerDrawned = false;

    // correctedY --> translate painter to  (0, correctedY)  in order to paint with paperRect coordonnates

    // do we have to create a newpage into printer ?
    if (currentPageNumber != 0) {
        m_Printer->newPage();
        p.restore();
        int previousHeaderHeight = 0;
        foreach(QTextDocument *doc, headers(currentPageNumber)) {
            previousHeaderHeight += doc->size().height();
        }
        p.translate(0, -drawnedSize.height() - previousHeaderHeight);
        correctedY += drawnedSize.height();
        p.save();
        // painter points at the beginning of the page
    }

    // do we have to include a watermark ?
    if (presenceIsRequiredAtPage(currentPageNumber+1 , m_WatermarkPresence)) {
        p.save();
        p.translate(0, correctedY);
        p.drawPixmap(m_Printer->paperRect(), m_Watermark);
        p.restore();
    }

    // do we have to include the header ?
    int specialY = correctedY;
    int headerHeight = 0;
    foreach(QTextDocument *doc, headers(currentPageNumber + 1)) {
        headerSize = doc->size();
        // draw all headers
        p.save();
        p.translate(0, specialY);
        specialY = 0;
        headerHeight += doc->size().height();
        QRectF headRect = QRectF(QPoint(0,0), headerSize);
        doc->drawContents(&p, headRect);
        p.restore();
        // translate painter under the header
        p.restore();
        p.translate(0, doc->size().height());
        p.save();
//        headerDrawned = true;
    }
    headerSize.setHeight(headerHeight);


    // do we have to include the footer ?
    int footHeight = 0;
    foreach(QTextDocument *doc, footers(currentPageNumber + 1)) {
        footerSize = QSizeF(doc->size().width(),0);
        footHeight += doc->size().height();
        p.save();
        p.translate(0, m_Printer->paperRect().bottom() + correctedY - footHeight - headerSize.height());
        QRectF footRect = QRectF(QPoint(0,0), QSizeF(doc->size().width(), footHeight));
        doc->drawContents(&p, footRect);
        p.restore();
//        footerDrawned = true;
    }
    footerSize.setHeight(footHeight);

    // recalculate the content size of the content page
    pageSize = QSizeF(pageWidth(),
                       m_Printer->paperRect().height() - headerSize.height() - footerSize.height());

    // reset drawnedSize (nothing is drawned into the new page)
    drawnedSize = QSizeF(0,0);

    return currentPageNumber + 1;
}

/**
    \brief This drawing funtion assumes that there is the same number of headers
    and footers on each pages.
    Watermark are printed first, text lies over the watermark. Then headers and
    footers are added, and finally the content of the document to print is added
    page by page.
*/
bool PrinterPrivate::simpleDraw()
{
    if (!m_Content) {
        LOG_ERROR_FOR("Printer", QCoreApplication::translate("tkPrinter", "No content to preview (simpleDraw)."));
        return false;
    }
    m_PrintingDuplicata = false;
    // All pages measures
    // pageWidth, margins of document, margins of printer
    int _pageWidth = pageWidth();                     //TODO add margins
    this->setTextWidth(_pageWidth);

    m_Content->setPageSize(getSimpleDrawContentPageSize());
//    qWarning() << getSimpleDrawContentPageSize();
    m_Content->setUseDesignMetrics(true);

    // prepare drawing areas
    QRect contentRect = QRect(QPoint(0,0), m_Content->size().toSize());     // whole document drawing rectangle

    if (!simpleDrawPreparePages(contentRect))
        return false;

    return true;
}

/**
  \brief Draws the pages into the m_Pages (QPicture) with the headers/footers/watermarks and the content using the simpleDraw method.
  \sa isSimple(), simpleDraw(), Printer::previewToPixmap()
*/
bool PrinterPrivate::simpleDrawPreparePages(QRect &contentRect)
{
    qDeleteAll(m_Pages);
    m_Pages.clear();
    int _pageWidth = contentRect.size().width();
    QSizeF headerSize(_pageWidth, 0);
    QSizeF footerSize(_pageWidth, 0);
    int pageNumber = 1;
    int drawnHeight = 0;
//    QRectF headRect = QRectF(QPoint(0,0), headerSize);
    QRect currentRect = contentRect;
    int fromPage = m_Printer->fromPage();
    int toPage = m_Printer->toPage();
    bool fromToPage = ((fromPage>0) || (toPage>0));
    while (currentRect.intersects(contentRect)) {
        QPicture *pic = new QPicture;
        QPainter painter;
        painter.begin(pic);
        currentRect = getSimpleDrawCurrentRectangle(pageNumber);
        if (fromToPage) {
            if (pageNumber>toPage)
                break;
            if (pageNumber<fromPage) {
                drawnHeight += currentRect.height();
                pageNumber++;
                continue;
            }
        }
        // at the beginning of the while, painter is translated to the 0,0 position of the new page
        simpleDrawWatermark(painter, pageNumber);
        simpleDrawHeaderFooter(painter, headerSize, footerSize, pageNumber);

        // draw content for this page
        simpleDrawContent(painter, headerSize, currentRect, drawnHeight);

        // calculate new page
        // go to content next page
        if (m_WithDuplicata) {
            if (m_PrintingDuplicata) {
                drawnHeight += currentRect.height();
                // duplicate is drawn --> translate the currentRect to the beginning of the next page
                currentRect.translate(0, currentRect.height());
                pageNumber++;
            }
            m_PrintingDuplicata = !m_PrintingDuplicata;
        } else {
            drawnHeight += currentRect.height();
            // translate the currentRect to the beginning of the next page
            currentRect.translate(0, currentRect.height());
            pageNumber++;
        }

        // if there is still something to print --> create a newpage to the printer
        if (currentRect.intersects(contentRect)) {
            if (fromToPage) {
                if ((pageNumber>=fromPage) && (pageNumber<=toPage))
                    m_Printer->newPage();
            } else {
                    m_Printer->newPage();
            }
        }
        m_Pages.append(pic);
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  PUBLIC PART  ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
Printer::Printer(QObject * parent)
    : QObject(parent),
    d(0)
{
    setObjectName("Printer");
    d = new PrinterPrivate(this);
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

/** \brief Get the printer as set in application's settings */
bool Printer::getUserPrinter()
{
    delete d->m_Printer;
    d->m_Printer = 0;
    d->m_TwoNUp = settings()->value(Print::Constants::S_TWONUP).toBool();
    const QString &name = settings()->value(Constants::S_DEFAULT_PRINTER).toString();
    if (name.compare("system",Qt::CaseInsensitive)==0 || name.compare("user", Qt::CaseInsensitive)==0) {
        if (QPrinterInfo::defaultPrinter().isNull()) {
            d->m_Printer = new QPrinter;
            d->m_Printer->setResolution(QPrinter::ScreenResolution);
        } else {
            d->m_Printer = new QPrinter(QPrinterInfo::defaultPrinter(), QPrinter::ScreenResolution);
        }
    } else {
        foreach(const QPrinterInfo &info, QPrinterInfo::availablePrinters()) {
            if (info.printerName() == name) {
                d->m_Printer = new QPrinter(info, QPrinter::ScreenResolution);
                break;
            }
        }
    }
    if (d->m_Printer) {
        d->m_Printer->setColorMode(QPrinter::ColorMode(settings()->value(Constants::S_COLOR_PRINT).toInt()));
        d->m_Printer->setPaperSize(QPrinter::A4);
//        d->m_Printer->setPageMargins(50,50,50,50, QPrinter::DevicePixel);
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
    dialog.setWindowTitle(tr("Print document"));
    if (dialog.exec() == QDialog::Accepted)
        return true;
    return false;
}

/** \brief Defines the printer to use. */
void Printer::setPrinter(QPrinter *printer)
{
    if (printer) {
        if (d->m_Printer) {
            delete d->m_Printer;
            d->m_Printer = 0;
        }
        d->m_Printer = printer;
    } else {
        d->renewPrinter();
    }
}

/** \brief Returns the printer in use. */
QPrinter *Printer::printer()
{
    return d->m_Printer;
}

/**
  \brief Set a header content, presence and priority.
*/
void Printer::setHeader(const QString & html, Presence presence, Printer::Priority prior)
{
    d->m_Headers.append(new TextDocumentExtra(html, presence, prior));
}

/**
  \brief Set a footer content, presence and priority.
*/
void Printer::setFooter(const QString & html, Presence presence, Printer::Priority prior)
{
    d->m_Footers.append(new TextDocumentExtra(html, presence, prior));
}

/** \brief Set the main text to print/ */
void Printer::setContent(const QString & html)
{
    d->renewContent();
    d->content()->setHtml(html);
}

/** \brief Set the main text to print/ */
void Printer::setContent(const QTextDocument & docToPrint)
{
    d->renewContent();
    d->m_Content = docToPrint.clone();
}

/** Return the html content of the printer. This does not includes header, footer and watermark. */
QString Printer::htmlContent() const
{
    return d->m_Content->toHtml();
}

QString Printer::toHtml() const
{
    QString html, content, css, header, body;
    QTextDocument *doc = 0;
    // Add Each page header
    doc = d->header(EachPages);
    if (doc) {
        content = doc->toHtml("UTF-8");
        css = Utils::htmlTakeAllCssContent(content);
        content = Utils::htmlBodyContent(content);
        content.prepend(css);
        html += content;
    }
    // Add first page header
    QTextDocument *doc2 = 0;
    doc2 = d->header(FirstPageOnly);
    if (doc2 && doc2 != doc) {
        content = doc->toHtml("UTF-8");
        css = Utils::htmlTakeAllCssContent(content);
        content = Utils::htmlBodyContent(content);
        content.prepend(css);
        html += content;
    }
    // Add content
    content = d->m_Content->toHtml("UTF-8");
    css = Utils::htmlTakeAllCssContent(content);
    content = Utils::htmlBodyContent(content);
    content.prepend(css);
    html += content;
    // Add Footer
    doc = d->footer(EachPages);
    if (doc) {
        content = doc->toHtml("UTF-8");
        css = Utils::htmlTakeAllCssContent(content);
        content = Utils::htmlBodyContent(content);
        content.prepend(css);
        html += content;
    }
    // Add HTML5 HEADER
    css = Utils::htmlTakeAllCssContent(html);
    content = Utils::htmlBodyContent(html);
    body = QString("<body>%1</body>").arg(content);
    header = QString("<head><meta charset=\"UTF-8\"><title></title>%1</head>").arg(css);
    html = body.prepend(header);
    html = QString("<!DOCTYPE html><html>%1</html>").arg(html);
    return html;
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
    d->m_Printer = new QPrinter(def, QPrinter::ScreenResolution);
    return true;
}

void Printer::setOrientation(QPrinter::Orientation orientation)
{
    if (!d->m_Printer)
        d->renewPrinter();
    d->m_Printer->setOrientation(orientation);
    // TODO Manage Margins
    d->setTextWidth(d->pageWidth());
}

void Printer::setPaperSize(QPrinter::PaperSize size)
{
    if (!d->m_Printer)
        d->renewPrinter();
    d->m_Printer->setPaperSize(size);
    // TODO Manage Margins
    d->setTextWidth(d->pageWidth());
}

void Printer::setTwoNUp(bool state)
{
    d->m_TwoNUp = state;
}

bool Printer::isTwoNUp() const
{
    return d->m_TwoNUp;
}

bool Printer::preparePages()
{
    if (!d->m_Printer)
        return false;

    if (d->isSimple()) {
        LOG("Printing using simpleDraw method.");
        return d->simpleDraw();
    }
    else {
        LOG("WARNING: Printing using complexDraw method (should be buggy).");
        return d->complexDraw();
    }
}

QList<QPicture *> Printer::pages() const
{
    return d->m_Pages;
}


/** \brief Shows the print preview dialog. \e test param should only be used for debugging. */
bool Printer::previewDialog(QWidget *parent, bool test)
{
    if (!d->m_Printer)
        d->m_Printer = new QPrinter(QPrinter::ScreenResolution);
    Q_UNUSED(test);
    preparePages();
    QPrintPreviewDialog dialog(d->m_Printer, parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    connect(&dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(print(QPrinter *)));
    dialog.exec();
    return true;
}

/**
  \brief Print the documents as the content (headers, footers and watermark must be defined before).
*/
bool Printer::print(const QTextDocument &docToPrint)
{
    d->renewContent();
    d->m_Content = docToPrint.clone();
    print(d->m_Printer);
    return true;
}

/**
  \brief Print the documents as the content (headers, footers and watermark must be defined before).
*/
bool Printer::print(const QString &htmlToPrint)
{
    QTextDocument t;
    t.setHtml(htmlToPrint);
    return print(t);
}

/** \brief Re-Print the same documents as the previous call of print(). */
bool Printer::reprint(QPrinter *printer)
{
    if (!printer)
        return false;
    if (!printer->isValid())
        return false;

    if (d->m_TwoNUp)
        printer->setOrientation(QPrinter::Landscape);
    else
        printer->setOrientation(QPrinter::Portrait);

    QPainter print;
    print.begin(printer);
    int from;
    int to;
    if (printer->printRange()==QPrinter::PageRange) {
        from = printer->fromPage();
        to = printer->toPage();
    } else {
        from = 1;
        to = d->m_Pages.count();
    }

//    qWarning() << from << to << d->m_TwoNUp;

    while (from <= to) {
        pageToPainter(&print, from, d->m_TwoNUp, false);
        d->m_TwoNUp ? from += 2 : ++from;
//        qWarning() << from;
        if (from <= to)
            if (!printer->newPage())
                return false;
    }
    print.end();
    return true;
}

/**
  \brief Inform the printing engine that it must print duplicatas too.
  If set to true, the duplicatas are printed just after the page, not at the end of the whole printing.\n
  Setting your printer driver to 2n-up should be fine to print one the left the original and on the right the duplicata.
*/
void Printer::setPrintWithDuplicata(bool state)
{
    d->m_WithDuplicata=state;
}

bool Printer::printWithDuplicatas() const
{
    return d->m_WithDuplicata;
}

/**
  \brief Print all documents (headers, footers, watermark and content) to the printer \e printer
  This member is used by QPrintDialogPreview in order to refresh the preview.\n
  You should not use this function directly.
  \sa print(const QTextDocument &), print(const QString &)
*/
bool Printer::print(QPrinter *printer)
{
    if (!printer)
        printer = d->m_Printer;

    if (!printer)
        return false;

    reprint(d->m_Printer);
    return true;
}

/** \brief Defines the QPixmap to use for the Watermark */
void Printer::addPixmapWatermark(const QPixmap & pix, const Presence p , const Qt::AlignmentFlag watermarkAlignment)
{
    if (! d->m_Printer)
        return;
    d->m_WatermarkPresence = p;
    QRectF paperRect = d->m_Printer->paperRect();

    // prepare watermark pixmap
    if (d->m_Watermark.isNull()) {
        d->m_Watermark = QPixmap(paperRect.width(), paperRect.height());
        d->m_Watermark.fill();
    }

    // TODO page margins
    // TODO manageDPI of pixmap
    QRectF pixRect = pix.rect();
    int rotationAngle = d->calculateWatermarkRotation(paperRect, watermarkAlignment);

    // Prepare painter
    QPainter painter;
    painter.begin(&d->m_Watermark);
    painter.translate(-paperRect.topLeft());  // TODO : this is wrong because we loose the margins
    painter.save();
    // rotate the painter from its middle
    if (rotationAngle != 0) {
        painter.translate(pixRect.center());
        painter.rotate(rotationAngle);
        // scale textRect to feet inside the paperRect - margins
        QRectF boundingRect = d->rotatedBoundingRect(pixRect, rotationAngle);
        double scale = qMin(paperRect.width() / boundingRect.width(), paperRect.height() / boundingRect.height());
        painter.scale(scale, scale);
        painter.translate(-pixRect.center());
    }
    painter.drawRect(pixRect);
    painter.drawPixmap(pixRect, pix, QRectF());
    painter.restore();
    painter.end();
}

/** \brief Defines the html text to use for the Watermark */
void Printer::addHtmlWatermark(const QString & html,
                                  const Presence p,
                                  const Qt::Alignment watermarkAlignment,
                                  const int orientation)
{
    if (! d->m_Printer)
        return;
    d->m_WatermarkPresence = p;

    // TODO Manage page margins +++

    // get some values about the printing page and prepare the pixmap
    QRectF paperRect = d->m_Printer->paperRect();

    d->m_Watermark = QPixmap(paperRect.width(), paperRect.height());
    d->m_Watermark.fill();
    previewHtmlWatermark(d->m_Watermark, html, p, watermarkAlignment, orientation);
}

/**
  \brief Draws to the QPixmap \e drawTo the watermark defined by the QTextDocument \e doc.
  \todo When asking multi watermarks, is watermarks does not have the same presence --> Bug. Need to store d->watermarks into a hash<presence,qpixmap>
*/
void Printer::previewDocumentWatermark(QPixmap &drawTo,
                                          QTextDocument *doc,
                                          const Presence p,
                                          const Qt::Alignment watermarkAlignment,
                                          const int orientation)
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
    QPointF pageCenter(drawTo.rect().center());
    QRect paperRect = drawTo.rect();
    // Calculates the painting area for the text
    QRectF textRect = QRectF(QPointF(0,0), doc->size());
    int rotationAngle = PrinterPrivate::calculateWatermarkRotation(paperRect, watermarkAlignment);

    // Prepare painter
    QPainter painter;
    painter.begin(&drawTo);
    painter.translate(-paperRect.topLeft());  // TODO : this is wrong because we loose the margins
    painter.save();
    // rotate the painter from its middle
//    if (rotationAngle != 0) {
    {
        QRectF boundingRect = PrinterPrivate::rotatedBoundingRect(textRect, rotationAngle);
        double scale = qMin(paperRect.width() / boundingRect.width(), paperRect.height() / boundingRect.height());
        PrinterPrivate::moveTextRect(textRect, paperRect, watermarkAlignment, scale);
        painter.translate(textRect.center());
        painter.rotate(rotationAngle);
        painter.scale(scale, scale);
        // scale textRect to feet inside the paperRect - margins
        painter.translate(-textRect.center());
    }
//    else {
//        QSizeF docSize = wm.size();
////    painter.translate(textRect.topLeft());
//        double scale = qMin(paperRect.width() / docSize.width(), paperRect.height() / docSize.height());
//        tkPrinterPrivate::moveTextRect(textRect, paperRect, watermarkAlignment, scale);
//        painter.scale(scale, scale);
////    painter.translate(-textRect.topLeft());
//    }

    painter.translate(textRect.topLeft());
    doc->drawContents(&painter);//, textRect);
    painter.translate(-textRect.topLeft());

    painter.restore();
    painter.end();
    doc->setDefaultTextOption(docOptionSave);
    doc->setPageSize(docSizeSave);
}

/** \brief Draws the \e html text watermark to QPixmap \e drawTo */
void Printer::previewHtmlWatermark(QPixmap &drawTo,
                                      const QString & html,
                                      const Presence p,
                                      const Qt::Alignment watermarkAlignment,
                                      const int orientation)
{
    QTextDocument wm;
    wm.setHtml(html);
    previewDocumentWatermark(drawTo, &wm, p, watermarkAlignment, orientation);
}

/** \brief Draws the plain text \e plainText watermark to QPixmap \e drawTo */
void Printer::previewTextWatermark(QPixmap &drawTo,
                                      const QString & plainText,
                                      const Presence p,
                                      const Qt::Alignment watermarkAlignment,
                                      const int orientation)
{
    QTextDocument wm;
    wm.setPlainText(plainText);
    previewDocumentWatermark(drawTo, &wm, p, watermarkAlignment, orientation);
}

/** \brief Defines the plain text to use for the Watermark */
void Printer::addTextWatermark(const QString & plainText,
                                  const Presence p,
                                  const Qt::Alignment watermarkAlignment,
                                  const Qt::Alignment textAlignment,
                                  const QFont & font,
                                  const QColor & color,
                                  const int orientation)
{
    if (! d->m_Printer)
        return;
    d->m_WatermarkPresence = p;

    // TODO Manage page margins +++

    // get some values about the printing page and prepare the pixmap
    QRectF paperRect = d->m_Printer->paperRect();

    if (d->m_Watermark.isNull()) {
        d->m_Watermark = QPixmap(paperRect.width(), paperRect.height());
        d->m_Watermark.fill();
    }

    QString html = QString("<html><body><p %1 style=\"%2\">%3</p></body></html>")
                   .arg(Utils::textAlignmentToHtml(textAlignment))
                   .arg(Utils::fontToHtml(font, color))
                   .arg(plainText);
    html.replace("\n", "<br/>");
    previewHtmlWatermark(d->m_Watermark, html, p, watermarkAlignment, orientation);
}

/**
  \brief Returns the Previewer widget as a pointer
  This widget is used inside the DefaultUserPapersWidget of the User Manager.
  \sa tkPrinterPreviewer
  \sa DefaultUserPapersWidget
*/
PrinterPreviewer *Printer::previewer(QWidget *parent)
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
void Printer::previewToPixmap(QPixmap &drawTo, QPrinter *printer)
{
    Q_ASSERT(printer);
//    QSize savePixSize = drawTo.size();
    if (!d->m_Content) {
        d->m_Content = new QTextDocument(this);
        d->m_Content->setHtml("<p>This is the previewing mode</p><p&nbsp;</p><p>This is the previewing mode</p><p&nbsp;</p><p>This is the previewing mode</p><p&nbsp;</p>");
    }
    d->m_PrintingDuplicata = false;

    int _pageWidth = printer->paperRect().width();//d->pageWidth();
    d->setTextWidth(_pageWidth);

    d->m_Content->setPageSize(printer->paperRect().size());//d->getSimpleDrawContentPageSize());
    d->m_Content->setUseDesignMetrics(true);

    // prepare drawing areas
//    QRect contentRect = QRect(QPoint(0,0), d->m_Content->size().toSize());     // whole document drawing rectangle

    // prepare painter then draw
    drawTo = QPixmap(_pageWidth, printer->paperRect().height() + 30);
    drawTo.fill();
    QPainter painter;
    painter.begin(&drawTo);

    // TODO: code here
//    d->simpleDrawToPainter(painter, contentRect);
    drawTo = drawTo.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

/** \brief Draw the header and footer passed as params into \e drawTo. \e drawTo must be sized before. */
void Printer::previewHeaderFooter(QPixmap &drawTo,
                                     const QString &headerHtml,
                                     const QString &footerHtml)
{
//    QSize savePixSize = drawTo.size();
    // prepare pseudo printer
    QPrinter *printer = new QPrinter;
    printer->setPaperSize(QPrinter::A4);
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
        headerDoc->setTextWidth(_pageWidth);
        headerSize.setHeight(headerDoc->size().height());
    }
    QTextDocument *footerDoc = d->footer(Printer::EachPages);
    if (footerDoc) {
        footerDoc->setTextWidth(_pageWidth);
        footerSize.setHeight(footerDoc->size().height());
    }

    // prepare painter
    drawTo = QPixmap(_pageWidth, printer->paperRect().height());
    drawTo.fill();
    QPainter painter;
    painter.begin(&drawTo);
//    painter.translate(11, 11);
    d->simpleDrawHeaderFooter(painter, headerSize, footerSize, 1);
    // scale painter
    // close painter
    painter.end();
    drawTo = drawTo.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

/**
  \brief Draws the selected page into the painter. The painter is not ended.
If \e twoNUp is specified two consecutive pages are drawned.
The \e pageNumber should be ranged into [1, ...]. Page number must exist.
  \code
    Print::Printer *printer = new Print::Printer;
    // Draw page 1-2 in 2-N-Up to a pixmap
    QPixMap pix(size);
    QPainter paint;
    paint.begin(&pix);
    printer->pageToPainter(&paint, 1, true, true);
    paint.end();
    pix.scale(...);

    // Print page 1-2 in 2-N-Up to a printer
    QPrinter print;
    print.setResolution(QPrinter::ScreenResolution);
    QPainter paint;
    paint.begin(&print);
    printer->pageToPainter(&paint, 1, true, false);
  \endcode
*/
bool Printer::pageToPainter(QPainter *paint, const int pageNumber, bool twoNUp, bool pixmapPreview)
{
    if (!d->m_Printer)
        return false;

    if (pageNumber>d->m_Pages.count())
        return false;

    if (pageNumber<=0)
        return false;

    const QSizeF &paperSize = d->m_Printer->paperRect(QPrinter::DevicePixel).size();
    const QSizeF &pageSize = d->m_Printer->pageRect(QPrinter::DevicePixel).size();

    if (paint)
        paint->save();

    if (!twoNUp) {
        QPicture *page = d->m_Pages.at(pageNumber-1);
        if (!page) {
            paint->restore();
            return false;
        }
        paint->scale(0.95, 0.95); // here is the pseudo margins management
        if (pixmapPreview)
            paint->translate((paperSize.width() - pageSize.width())/2.0, (paperSize.height()-pageSize.height())/2.0);
        page->play(paint);
    } else {
        QPicture *page1 = d->m_Pages.at(pageNumber-1);

        // landscape printing
        QSizeF pageS = pageSize;
        QSizeF paperS = paperSize;
        // correctly manages portrait/landscape printing
        if (pageSize.width() > pageSize.height()) {
            pageS = QSizeF(pageSize.height(), pageSize.width());
        }
        if (paperSize.width() > paperSize.height()) {
            paperS = QSizeF(paperSize.height(), paperSize.width());
        }

        if (!page1) {
            paint->restore();
            return false;
        }
        QPicture *page2 = 0;
        if (pageNumber < d->m_Pages.count())
            page2 = d->m_Pages.at(pageNumber);

        // First page
        paint->scale(0.7, 0.68);
        if (pixmapPreview)
            paint->translate((paperS.height() - pageS.height())/2.0, (paperS.width()-pageS.width())/2.0);  // here is the pseudo margins management
//        paint->translate((paperSize.height() - pageSize.height()), (paperSize.width()-pageSize.width()));  // here is the pseudo margins management
        page1->play(paint);

        // Second page
        if (page2) {
            if (pixmapPreview)
                paint->translate(pageSize.width()+(paperSize.width() - pageSize.width())/2.0, 0);  // here is the pseudo margins management
            else
                paint->translate(pageS.width() + ((paperS.height() - pageS.height())/2.0), 0);
//            paint->translate(pageSize.height(), 0);  // landscape printing
            page2->play(paint);
        }
    }
    paint->restore();
    return true;
}

/**
  \brief Prints the recorded headers/footers/watermarks and content to a PDF file named \e fileName.
  \e fileName must be an absolute path file name and will be replaced without warning if it already exists.
  The QPrinter MUST BE specified FIRST !
*/
bool Printer::toPdf(const QString &fileName, const QString &docName)
{
    Q_ASSERT(!fileName.isEmpty());
    if (fileName.isEmpty())
        return false;
    QString tmp = fileName;
    if (QFileInfo(tmp).suffix().isEmpty())
        tmp.append(".pdf");

    // set resolution to high
    d->m_Printer->setResolution(QPrinter::HighResolution);

    QPrinter::OutputFormat format = d->m_Printer->outputFormat();
    // try with PdfFormat again
    #ifdef Q_OS_MAC                                                             
    d->m_Printer->setOutputFormat(QPrinter::PdfFormat);                           
    // On other OS, use PdfFormat                                               
    #else
    d->m_Printer->setOutputFormat(QPrinter::PdfFormat);
    #endif

    d->m_Printer->setCreator(qApp->applicationName() + " " + qApp->applicationVersion());
    d->m_Printer->setOutputFileName(tmp);
    d->m_Printer->setDocName(docName);
    bool ok = reprint(d->m_Printer);
    d->m_Printer->setOutputFormat(format);
    return ok;
}

/**
  \brief Prints the \e docToPrint document with the recorded headers/footers/watermarks to a PDF file named \e fileName.
  \e fileName must be an absolute path file name and will be replaced without warning if it already exists.
  This function replaces the actual content of this class.
  The QPrinter MUST BE specified FIRST !
*/
bool Printer::toPdf(const QString &fileName, const QTextDocument &docToPrint)
{
    d->m_Content->setHtml(docToPrint.toHtml("UTF-8"));
    return toPdf(fileName, "");
}
