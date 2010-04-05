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
 ***************************************************************************/
#ifndef TKTEXTDOCUMENTHEADER_H
#define TKTEXTDOCUMENTHEADER_H

#include <printerplugin/printer_exporter.h>
#include <printerplugin/printer.h>

#include <utils/global.h>

#include <QTextDocument>
#include <QObject>
#include <QVariant>


namespace Print {
namespace Internal {
class TextDocumentExtraPrivate;
}  // End Internal

namespace Constants {
const char* const TKDOCUMENT_GENERAL_XML_TAG   = "tkDocumentExtra";  /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_VERSION_XML_TAG   = "Version";          /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_PRESENCE_XML_TAG  = "Presence";         /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_PRIORITY_XML_TAG  = "Priority";         /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_EXTRA_XML_TAG     = "Extras";           /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_HTML_XML_TAG      = "tkDocumentHtml";   /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_XML_ACTUALVERSION = "1.0";              /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */

}  // End Constants

class PRINTER_EXPORT TextDocumentExtra
{
public:
    TextDocumentExtra();
    TextDocumentExtra( const QString &html, const int presence = Printer::EachPages, const int priority = Printer::First, const QString &version = QString::null );
    ~TextDocumentExtra();

    void setPriority( Printer::Priority p );
    void setPresence( Printer::Presence p );
    void setHtml( const QString &html);

    Printer::Priority priority() const;
    Printer::Presence presence() const;

    QTextDocument *document() const;

    void setTextWidth( qreal width );
    bool lessThan( const TextDocumentExtra *h1, const TextDocumentExtra *h2 );

    QString toXml() const;
    static TextDocumentExtra *fromXml(const QString &xml);
    QString toHtml() const;

private:
    Internal::TextDocumentExtraPrivate *d;
};

}  // End Print

Q_DECLARE_METATYPE(Print::TextDocumentExtra)

#endif // TKTEXTDOCUMENTHEADER_H
