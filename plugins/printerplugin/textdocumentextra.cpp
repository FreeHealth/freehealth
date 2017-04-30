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
 *       NAME <MAIL>                                                       *
 ***************************************************************************/
#include "textdocumentextra.h"

#include <utils/global.h>

/**
  \class TextDocumentExtra
  \brief This class is used by Printer for header/footer/watermark management.
  Xml structure looks like:\n
  \verbatim
  <tkConstants::TKDOCUMENT_GENERAL_XML_TAG> </tkConstants::TKDOCUMENT_GENERAL_XML_TAG>\n
    <tkConstants::TKDOCUMENT_VERSION_XML_TAG> </tkConstants::TKDOCUMENT_VERSION_XML_TAG>\n
    <tkConstants::TKDOCUMENT_PRESENCE_XML_TAG> </tkConstants::TKDOCUMENT_PRESENCE_XML_TAG>\n
    <tkConstants::TKDOCUMENT_PRIORITY_XML_TAG> </tkConstants::TKDOCUMENT_PRIORITY_XML_TAG>\n
    <tkConstants::TKDOCUMENT_EXTRA_XML_TAG> </tkConstants::TKDOCUMENT_EXTRA_XML_TAG>\n
    <tkConstants::TKDOCUMENT_HTML_XML_TAG> </tkConstants::TKDOCUMENT_HTML_XML_TAG>\n
  </tkConstants::TKDOCUMENT_GENERAL_XML_TAG>\n
  \endverbatim
  You can:
  \li get a pointer to the referenced QTextDocument using document().\n
  \li set the Presence of the document according to Printer::Presence\n
  \li set the Priority of the document according to Printer::Priority\n
  \li get/set to/from Xml using toXml() , fromXml()\n

  \sa Printer
  \ingroup object_toolkit
*/

using namespace Print;
using namespace Print::Internal;

namespace Print {
namespace Internal {
class TextDocumentExtraPrivate {
public:
    TextDocumentExtraPrivate() :
        m_Presence(Printer::EachPages),
        m_Priority(Printer::First),
        m_DocCreated(false),
        m_Doc(0)
    {}

    Printer::Presence  m_Presence;
    Printer::Priority  m_Priority;
    QString xmlVersion;
    QString m_Html;
    mutable bool m_DocCreated;
    mutable QTextDocument *m_Doc;
};
}
}

TextDocumentExtra::TextDocumentExtra() : d(0)
{
    d = new TextDocumentExtraPrivate();
    d->xmlVersion = Constants::TKDOCUMENT_XML_CURRENTVERSION;
    d->m_Html = "";
    d->m_Priority = Printer::First;
    d->m_Presence = Printer::EachPages;
    d->m_DocCreated = false;
}

TextDocumentExtra::TextDocumentExtra( const QString &html, const int presence, const int priority, const QString &version) : d(0)
{
    d = new TextDocumentExtraPrivate();
    if (version.isEmpty())
        d->xmlVersion = Constants::TKDOCUMENT_XML_CURRENTVERSION;
    else
        d->xmlVersion = version;
    d->m_Priority = Printer::Priority(priority);
    d->m_Presence = Printer::Presence(presence);
    d->m_Html = html;
    d->m_DocCreated = false;
}

TextDocumentExtra::~TextDocumentExtra()
{
    if (d) delete d;
    d = 0;
}

void TextDocumentExtra::setPriority( Printer::Priority p )
{ d->m_Priority = p; }

void TextDocumentExtra::setPresence( Printer::Presence p )
{ d->m_Presence = p; }

void TextDocumentExtra::setHtml( const QString &html)
{
    d->m_Html = html;
    if (d->m_DocCreated)
        d->m_Doc->setHtml(html);
}

Printer::Priority TextDocumentExtra::priority() const
{ return d->m_Priority; }

Printer::Presence TextDocumentExtra::presence() const
{ return d->m_Presence; }

bool TextDocumentExtra::lessThanPresence(const TextDocumentExtra *h1, const TextDocumentExtra *h2)
{
    return h1->presence() < h2->presence();
}

QString TextDocumentExtra::toXml() const
{
    QHash<QString,QString> t;
    t.insert(Constants::TKDOCUMENT_VERSION_XML_TAG, d->xmlVersion);
    t.insert(Constants::TKDOCUMENT_PRESENCE_XML_TAG,QString::number(presence()));
    t.insert(Constants::TKDOCUMENT_PRIORITY_XML_TAG, QString::number(priority()));
    t.insert(Constants::TKDOCUMENT_EXTRA_XML_TAG, "");
    t.insert(Constants::TKDOCUMENT_HTML_XML_TAG, Utils::htmlReplaceAccents(toHtml()));
    return Utils::createXml(Constants::TKDOCUMENT_GENERAL_XML_TAG,t, 4, false);
}

TextDocumentExtra *TextDocumentExtra::fromXml(const QString &xml)
{
    if (!xml.contains(QString("<%1>").arg(Constants::TKDOCUMENT_GENERAL_XML_TAG)))
        return new TextDocumentExtra();
    QHash<QString, QString> t;
    if (!Utils::readXml(xml, Constants::TKDOCUMENT_GENERAL_XML_TAG, t, false))
        return new TextDocumentExtra();
    return new TextDocumentExtra(t.value(Constants::TKDOCUMENT_HTML_XML_TAG),
                                   Printer::Presence(t.value(Constants::TKDOCUMENT_PRESENCE_XML_TAG).toInt()),
                                   Printer::Priority(t.value(Constants::TKDOCUMENT_PRIORITY_XML_TAG).toInt()),
                                   t.value(Constants::TKDOCUMENT_VERSION_XML_TAG));
}


QString TextDocumentExtra::toHtml() const
{
    if (d->m_Doc) {
        return document()->toHtml("UTF-8");
    }
    return d->m_Html;
}

QTextDocument *TextDocumentExtra::document() const
{
    if (!d->m_Doc) {
        d->m_Doc = new QTextDocument();
        d->m_Doc->setHtml(d->m_Html);
        d->m_DocCreated = true;
    }
    return d->m_Doc;
}

void TextDocumentExtra::setTextWidth( qreal width )
{
    document()->setTextWidth(width);
}
