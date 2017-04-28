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
const char* const TKDOCUMENT_XML_CURRENTVERSION = "1.0";              /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */

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
    bool lessThanPresence(const TextDocumentExtra *h1, const TextDocumentExtra *h2);

    QString toXml() const;
    static TextDocumentExtra *fromXml(const QString &xml);
    QString toHtml() const;

private:
    Internal::TextDocumentExtraPrivate *d;
};

}  // End Print

Q_DECLARE_METATYPE(Print::TextDocumentExtra)

#endif // TKTEXTDOCUMENTHEADER_H
