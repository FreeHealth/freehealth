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
 ***************************************************************************/
/**
  \class tkPrinter
  \brief Manages document printing task with headers, footers and watermark.
  \ingroup toolkit
  \ingroup object_toolkit
*/

#include "tkPrinter.h"

#include <tkLog.h>
#include <tkGlobal.h>

class tkPrinterPrivate {
};

tkPrinter::tkPrinter( QObject * parent )
    : QObject(parent),
    d(0)
{
    setObjectName("tkPrinter");
}

tkPrinter::~tkPrinter()
{
}

void tkPrinter::clearHeaders()
{
}

void tkPrinter::clearFooters()
{
}

bool tkPrinter::askForPrinter( QWidget *parent )
{
    Q_UNUSED(parent);
    return false;
}

void tkPrinter::setPrinter( QPrinter * printer )
{
    Q_UNUSED(printer);
}

QPrinter *tkPrinter::printer()
{
    return 0;
}

void tkPrinter::setHeader( const QString & html, Presence presence, tkPrinter::Priority prior )
{
    Q_UNUSED(html);
    Q_UNUSED(presence);
    Q_UNUSED(prior);
}

void tkPrinter::setFooter( const QString & html, Presence presence, tkPrinter::Priority prior )
{
    Q_UNUSED(html);
    Q_UNUSED(presence);
    Q_UNUSED(prior);
}

void tkPrinter::setContent( const QString & html )
{
    Q_UNUSED(html);
}

bool tkPrinter::useDefaultPrinter()
{
    return false;
}

void tkPrinter::setOrientation(QPrinter::Orientation orientation)
{
    Q_UNUSED(orientation);
}

void tkPrinter::setPaperSize(QPrinter::PaperSize size)
{
    Q_UNUSED(size);
}

bool tkPrinter::previewDialog( QWidget *parent, bool test )
{
    Q_UNUSED(parent);
    Q_UNUSED(test);
    tkGlobal::functionNotAvailableMessageBox( tr("Print Preview not available") );
    return false;
}

bool tkPrinter::print( const QTextDocument & docToPrint )
{
    Q_UNUSED(docToPrint);
    tkGlobal::functionNotAvailableMessageBox( tr("Print functions are not available.") );
    return false;
}

bool tkPrinter::print( const QString &htmlToPrint )
{
    Q_UNUSED(htmlToPrint);
    tkGlobal::functionNotAvailableMessageBox( tr("Print functions are not available.") );
    return false;
}

bool tkPrinter::printWithDuplicata( bool state )
{
    Q_UNUSED(state);
    return false;
}

bool tkPrinter::print( QPrinter *printer )
{
    Q_UNUSED(printer);
    tkGlobal::functionNotAvailableMessageBox( tr("Print functions are not available.") );
    return false;
}

void tkPrinter::addPixmapWatermark( const QPixmap & pix, const Presence p , const Qt::AlignmentFlag watermarkAlignment )
{
    Q_UNUSED(pix);
    Q_UNUSED(p);
    Q_UNUSED(watermarkAlignment);
}

void tkPrinter::addHtmlWatermark( const QString & html,
                                  const Presence p,
                                  const Qt::Alignment watermarkAlignment,
                                  const int orientation )
{
    Q_UNUSED(html);
    Q_UNUSED(p);
    Q_UNUSED(watermarkAlignment);
    Q_UNUSED(orientation);
}

void tkPrinter::previewDocumentWatermark( QPixmap &drawTo,
                                          QTextDocument *doc,
                                          const Presence p,
                                          const Qt::Alignment watermarkAlignment,
                                          const int orientation )
{
    Q_UNUSED(drawTo);
    Q_UNUSED(doc);
    Q_UNUSED(p);
    Q_UNUSED(watermarkAlignment);
    Q_UNUSED(orientation);
}

void tkPrinter::previewHtmlWatermark( QPixmap &drawTo,
                                      const QString & html,
                                      const Presence p,
                                      const Qt::Alignment watermarkAlignment,
                                      const int orientation )
{
    Q_UNUSED(drawTo);
    Q_UNUSED(html);
    Q_UNUSED(p);
    Q_UNUSED(watermarkAlignment);
    Q_UNUSED(orientation);
}

void tkPrinter::previewTextWatermark( QPixmap &drawTo,
                                      const QString & plainText,
                                      const Presence p,
                                      const Qt::Alignment watermarkAlignment,
                                      const int orientation )
{
    Q_UNUSED(drawTo);
    Q_UNUSED(plainText);
    Q_UNUSED(p);
    Q_UNUSED(watermarkAlignment);
    Q_UNUSED(orientation);
}

void tkPrinter::addTextWatermark( const QString & plainText,
                                  const Presence p,
                                  const Qt::Alignment watermarkAlignment,
                                  const Qt::Alignment textAlignment,
                                  const QFont & font,
                                  const QColor & color,
                                  const int orientation )
{
    Q_UNUSED(plainText);
    Q_UNUSED(p);
    Q_UNUSED(watermarkAlignment);
    Q_UNUSED(textAlignment);
    Q_UNUSED(font);
    Q_UNUSED(color);
    Q_UNUSED(orientation);
}

void tkPrinter::clearWatermark()
{
}

tkPrinterPreviewer *tkPrinter::previewer( QWidget *parent )
{
    Q_UNUSED(parent);
    return 0;
}

void tkPrinter::previewToPixmap( QPixmap &drawTo, QPrinter *printer )
{
    Q_UNUSED(drawTo);
    Q_UNUSED(printer);
}

void tkPrinter::previewHeaderFooter( QPixmap &drawTo,
                                     const QString &headerHtml,
                                     const QString &footerHtml )
{
    Q_UNUSED(drawTo);
    Q_UNUSED(headerHtml);
    Q_UNUSED(footerHtml);
}

