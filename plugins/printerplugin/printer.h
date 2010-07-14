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
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Filipe Azevedo <>                                                 *
 *       Jourdan Romain <>                                                 *
 ***************************************************************************/
#ifndef PRINTER_H
#define PRINTER_H

#include <printerplugin/printer_exporter.h>

#include <QWidget>
#include <QFlags>
#include <QPixmap>
#include <QTextDocument>
#include <QPrinter>
#include <QFont>
#include <QColor>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QComboBox;
QT_END_NAMESPACE

/**
 * \file printer.h
 * \author Eric Maeker and the QPrinterEasy Team
 * \version 0.4.2
 * \date 14 July 2010
*/


namespace Print {
namespace Internal {
class PrinterPrivate;
}
class TextDocumentHeader;
class PrinterPreviewer;
class TextDocumentExtra;

class PRINTER_EXPORT Printer : public QObject
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
    Printer( QObject * parent = 0 );
    ~Printer();

    bool getUserPrinter();
    bool askForPrinter( QWidget *parent = 0 );
    bool useDefaultPrinter();
    void setPrinter(QPrinter *printer);
    QPrinter *printer();

    bool previewDialog(QWidget *parent = 0, bool test = false);

    void setHeader(const QString &html, Presence p = EachPages, Printer::Priority prior = First);
    void clearHeaders();

    void setFooter(const QString &html, Presence p = EachPages, Printer::Priority prior = First);
    void clearFooters();

    void setContent(const QString &html);
    void setContent(const QTextDocument & docToPrint);

    void setOrientation(QPrinter::Orientation orientation);
    void setPaperSize(QPrinter::PaperSize size);
    void setTwoNUp(bool state = true);
    bool isTwoNUp() const;

    bool preparePages();
    QList<QPicture *> pages() const;

public Q_SLOTS:
    // Watermark management
    void addPixmapWatermark( const QPixmap & pix,
                             const Presence p = EachPages,
                             const Qt::AlignmentFlag alignement = Qt::AlignCenter);

    void addTextWatermark( const QString & plainText,
                           const Presence p = EachPages,
                           const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                           const Qt::Alignment textAlignment = Qt::AlignCenter,
                           const QFont & font = QFont( "Hevetica", 36 ),
                           const QColor & color = QColor("lightgrey"),
                           const int orientation = -1 );

    void addHtmlWatermark( const QString & html,
                           const Presence p = EachPages,
                           const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                           const int orientation = -1 );

    static PrinterPreviewer *previewer(QWidget *parent);

    void previewToPixmap(QPixmap &drawTo, QPrinter *printer);

    void previewHeaderFooter( QPixmap &drawTo,
                              const QString &headerHtml,
                              const QString &footerHtml );

    static void previewDocumentWatermark( QPixmap &drawTo,
                                       QTextDocument *doc,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );
    static void previewHtmlWatermark( QPixmap &drawTo,
                                      const QString & html,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );
    static void previewTextWatermark( QPixmap &drawTo,
                                      const QString & plainText,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );

    void clearWatermark();

    void setPrintWithDuplicata(bool state = true);
    bool printWithDuplicatas() const;

    bool print(const QTextDocument & docToPrint);
    bool print(const QString &htmlToPrint);
    bool reprint(QPrinter *printer);

    bool pageToPainter(QPainter *paint, const int pageNumber, bool twoNUp = false, bool pixmapPreview = false);

    bool toPdf(const QString &fileName, const QString &docName);
    bool toPdf(const QString &fileName, const QTextDocument & docToPrint);

protected Q_SLOTS:
    bool print(QPrinter *printer = 0);  // used by tkPrinter PreviewDialog

private:
    Internal::PrinterPrivate *d;
};

class PrinterPreviewer : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PrinterPreviewer)

    Q_PROPERTY(QString htmlHeader     READ headerToHtml    WRITE setHeaderHtml    USER true)
    Q_PROPERTY(QString htmlFooter     READ footerToHtml    WRITE setFooterHtml    USER true)
    Q_PROPERTY(QString htmlWatermark  READ watermarkToHtml WRITE setWatermarkHtml USER true)

    Q_PROPERTY(int headerPresence     READ headerPresence    WRITE setHeaderPresence    USER true)
    Q_PROPERTY(int footerPresence     READ footerPresence    WRITE setFooterPresence    USER true)
    Q_PROPERTY(int watermarkPresence  READ watermarkPresence WRITE setWatermarkPresence NOTIFY watermarkPresenceChanged USER true)


public:
    explicit PrinterPreviewer(QWidget *parent = 0) : QWidget(parent) {}
    virtual ~PrinterPreviewer() {}

    virtual void initialize() = 0;

    // Properties used in QDataWidgetMapper
    virtual void setHeaderHtml(const QString &html) = 0;
    virtual void setFooterHtml(const QString &html) = 0;
    virtual void setWatermarkHtml(const QString &html) = 0;

    virtual void setHeaderPresence(const int presence) = 0;
    virtual void setFooterPresence(const int presence) = 0;
    virtual void setWatermarkPresence(const int presence) = 0;

    virtual QString headerToHtml() const = 0;
    virtual QString footerToHtml() const = 0;
    virtual QString watermarkToHtml() const = 0;

    virtual int headerPresence() const = 0;
    virtual int footerPresence() const = 0;
    virtual int watermarkPresence() const = 0;

    // manual setter / getters
    virtual void setHeader(const TextDocumentExtra *extra) = 0;
    virtual void setFooter(const TextDocumentExtra *extra) = 0;
    virtual void setWatermark(const TextDocumentExtra *extra) = 0;

    virtual void headerToPointer(TextDocumentExtra *extra) = 0;
    virtual void footerToPointer(TextDocumentExtra *extra) = 0;
    virtual void watermarkToPointer(TextDocumentExtra *extra) = 0;

    // Editor's access
    virtual QTextEdit *headerEditor() const = 0;
    virtual QTextEdit *footerEditor() const = 0;
    virtual QTextEdit *watermarkEditor() const = 0;

    // Presence combos
    virtual QComboBox *headerPresenceCombo() const = 0;
    virtual QComboBox *footerPresenceCombo() const = 0;
    virtual QComboBox *watermarkPresenceCombo() const = 0;

Q_SIGNALS:
    void watermarkPresenceChanged(const int presence);
};

} // End Print

#endif // TKPRINTER_H
