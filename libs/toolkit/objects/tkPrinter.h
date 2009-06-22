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
#ifndef TKPRINTER_H
#define TKPRINTER_H

#include <tkExporter.h>
class tkPrinterPrivate;
class tkTextDocumentHeader;
class tkPrinterPreviewer;
class tkTextDocumentExtra;

#include <QWidget>
#include <QFlags>
#include <QPixmap>
#include <QTextDocument>
#include <QPrinter>
#include <QFont>
#include <QColor>
#include <QVariant>
class QTextEdit;

/**
 * \file tkPrinter.h
 * \author Eric Maeker and the QPrinterEasy Team
 * \version 0.0.9
 * \date 03 May 2009
*/

/**
  \brief Manages document printing task with headers, footers and watermark.
  \ingroup toolkit
  \ingroup object_toolkit
*/
class Q_TK_EXPORT tkPrinter : public QObject
{
    Q_OBJECT
public:
    /** \brief This enum is used to define the presence of headers, footers, watermarks */
    enum Presence {
        EachPages = 0,
        FirstPageOnly,
        SecondPageOnly,
        LastPageOnly,
        ButFirstPage,
        OddPages,  // pages impaires
        EvenPages,  // pages paires
        DuplicataOnly
    };
    static QStringList presencesAvailable()
    {
        return QStringList()
                << tr("Each Pages")
                << tr("First page only")
                << tr("Second page only")
                << tr("Last page only")
                << tr("All pages but first")
                << tr("Odd pages")
                << tr("Even pages")
                << tr("Duplicatas only");
    }

    enum Priority {
        First = 0,
        Second,
        Third,
        Quater
    };

    /** \brief Class instanciation.
      *
      */
    tkPrinter( QObject * parent = 0 );
    ~tkPrinter();

    /** \brief Shows the print dialog. */
    bool askForPrinter( QWidget *parent = 0 );
    /** \brief Try to find the default system printer and use it without annoying user. */
    bool useDefaultPrinter();
    /** \brief Defines the printer to use. */
    void setPrinter( QPrinter * printer );
    /** \brief Returns the printer in use. */
    QPrinter *printer();

    /** \brief Shows the print preview dialog. test param should only be used for debugging. */
    bool previewDialog( QWidget *parent = 0, bool test = false );

    /** \brief Set a header. */
    void setHeader( const QString & html, Presence p = EachPages, tkPrinter::Priority prior = First );
    /** \brief Deletes all headers. */
    void clearHeaders();

    /** \brief Set a footer. */
    void setFooter( const QString & html, Presence p = EachPages, tkPrinter::Priority prior = First );
    /** \brief Deletes all footers. */
    void clearFooters();

    /** \brief Set the main text to print/ */
    void setContent( const QString & html );

    /** */
    void setOrientation(QPrinter::Orientation orientation);
    /** */
    void setPaperSize(QPrinter::PaperSize size);

public Q_SLOTS:
    // Watermark management
    /** \brief Add a pixmap watermark to pages. */
    void addPixmapWatermark( const QPixmap & pix,
                             const Presence p = EachPages,
                             const Qt::AlignmentFlag alignement = Qt::AlignCenter);

    /** \brief Add a plain text watermark to pages. */
    void addTextWatermark( const QString & plainText,
                           const Presence p = EachPages,
                           const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                           const Qt::Alignment textAlignment = Qt::AlignCenter,
                           const QFont & font = QFont( "Hevetica", 36 ),
                           const QColor & color = QColor("lightgrey"),
                           const int orientation = -1 );

    /** \brief Add a Html watermark to pages. */
    void addHtmlWatermark( const QString & html,
                           const Presence p = EachPages,
                           const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                           const int orientation = -1 );

    static tkPrinterPreviewer *previewer( QWidget *parent );

    void previewToPixmap( QPixmap &drawTo, QPrinter *printer );

    /** */
    void previewHeaderFooter( QPixmap &drawTo,
                              const QString &headerHtml,
                              const QString &footerHtml );

    /** */
    static void previewDocumentWatermark( QPixmap &drawTo,
                                       QTextDocument *doc,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );
    /** */
    static void previewHtmlWatermark( QPixmap &drawTo,
                                      const QString & html,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );
    /** */
    static void previewTextWatermark( QPixmap &drawTo,
                                      const QString & plainText,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );

    /** \brief Clear all watermark already passed. */
    void clearWatermark();

    /** */
    bool print( const QTextDocument & docToPrint );
    /** \brief By default no duplicatas are printed, if you want a duplicata set it to true. */
    bool printWithDuplicata( bool state = true );
    /** */
    bool print( const QString &htmlToPrint );

protected Q_SLOTS:
    /** \brief Slot used by the preview dialog. */
    bool print( QPrinter *printer = 0 );  // used by tkPrinter PreviewDialog

private:
    tkPrinterPrivate *d;
};

/**
  \brief QWidget for printing previewing. Must be instanciate by tkPrinter::previewer().
  \ingroup toolkit widget_toolkit
 */
class tkPrinterPreviewer : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(tkPrinterPreviewer)
    Q_PROPERTY(QString htmlHeader  READ headerToHtml WRITE setHeader USER true)
    Q_PROPERTY(QString htmlFooter  READ footerToHtml WRITE setFooter USER true)

//    Q_PROPERTY(QVariant extraDoc  READ extraDocument  WRITE setExtraDocument   USER true)

public:
    explicit tkPrinterPreviewer(QWidget *parent = 0) : QWidget(parent) {}
    virtual void initialize() = 0;

    virtual void setHeader(const QString &html, tkPrinter::Presence p = tkPrinter::EachPages) = 0;
    virtual void setFooter(const QString &html, tkPrinter::Presence p = tkPrinter::EachPages) = 0;
    virtual void setWatermark(const QString &html, tkPrinter::Presence p = tkPrinter::EachPages) = 0;

    virtual void setHeader(const tkTextDocumentExtra *extra) = 0;
    virtual void setFooter(const tkTextDocumentExtra *extra) = 0;
    virtual void setWatermark(const tkTextDocumentExtra *extra) = 0;

    virtual QTextEdit *headerEditor() = 0;
    virtual QTextEdit *footerEditor() = 0;
    virtual QTextEdit *watermarkEditor() = 0;

    virtual QString headerToHtml() = 0;
    virtual QString footerToHtml() = 0;
    virtual QString watermarkToHtml() = 0;
    virtual int headerPresence() = 0;
    virtual int footerPresence() = 0;
    virtual int watermarkPresence() = 0;

    virtual void headerToPointer(tkTextDocumentExtra *extra) = 0;
    virtual void footerToPointer(tkTextDocumentExtra *extra) = 0;
    virtual void watermarkToPointer(tkTextDocumentExtra *extra) = 0;
};

#endif // TKPRINTER_H
