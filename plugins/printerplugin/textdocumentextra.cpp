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
 *       NAME <MAIL>                                                       *
 ***************************************************************************/
#include "textdocumentextra.h"

/**
  \class TextDocumentExtra
  \brief This class is used by Printer for header/footer/watermark management.
  Xml structure looks like :\n
  <tkConstants::TKDOCUMENT_GENERAL_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_VERSION_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_PRESENCE_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_PRIORITY_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_EXTRA_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_HTML_XML_TAG> </idem>\n
  </tkConstants::TKDOCUMENT_GENERAL_XML_TAG>\n
  You can :
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
    TextDocumentExtraPrivate() : m_Doc(0) {}

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
    d->xmlVersion = Constants::TKDOCUMENT_XML_ACTUALVERSION;
    d->m_Html = "";
    d->m_Priority = Printer::First;
    d->m_Presence = Printer::EachPages;
    d->m_DocCreated = false;
}

TextDocumentExtra::TextDocumentExtra( const QString &html, const int presence, const int priority, const QString &version) : d(0)
{
    d = new TextDocumentExtraPrivate();
    if (version.isEmpty())
        d->xmlVersion = Constants::TKDOCUMENT_XML_ACTUALVERSION;
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

bool TextDocumentExtra::lessThan( const TextDocumentExtra *h1, const TextDocumentExtra *h2 )
{
    /** \todo  ? */
    return true;
}

QString TextDocumentExtra::toXml() const
{
    QHash<QString,QString> t;
    t.insert(Constants::TKDOCUMENT_VERSION_XML_TAG, d->xmlVersion);
    t.insert(Constants::TKDOCUMENT_PRESENCE_XML_TAG,QString::number(presence()));
    t.insert(Constants::TKDOCUMENT_PRIORITY_XML_TAG, QString::number(priority()));
    t.insert(Constants::TKDOCUMENT_EXTRA_XML_TAG, "");
    t.insert(Constants::TKDOCUMENT_HTML_XML_TAG, Core::tkGlobal::toHtmlAccent(toHtml()));
    return Core::tkGlobal::createXml(Constants::TKDOCUMENT_GENERAL_XML_TAG,t, 4, false);
}

TextDocumentExtra *TextDocumentExtra::fromXml(const QString &xml)
{
    if (!xml.contains(QString("<%1>").arg(Constants::TKDOCUMENT_GENERAL_XML_TAG)))
        return new TextDocumentExtra();
    QHash<QString, QString> t;
    if (!Core::tkGlobal::readXml(xml, Constants::TKDOCUMENT_GENERAL_XML_TAG, t, false))
        return new TextDocumentExtra();
    return new TextDocumentExtra(t.value(Constants::TKDOCUMENT_HTML_XML_TAG),
                                   Printer::Presence(t.value(Constants::TKDOCUMENT_PRESENCE_XML_TAG).toInt()),
                                   Printer::Priority(t.value(Constants::TKDOCUMENT_PRIORITY_XML_TAG).toInt()),
                                   t.value(Constants::TKDOCUMENT_VERSION_XML_TAG));
}


QString TextDocumentExtra::toHtml() const
{
    if (d->m_Doc) {
        return document()->toHtml();
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
