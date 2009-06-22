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
 ***************************************************************************/
#ifndef TKTEXTDOCUMENTHEADER_H
#define TKTEXTDOCUMENTHEADER_H

#include <tkExporter.h>
#include <tkPrinter.h>
#include <tkGlobal.h>

#include <QTextDocument>
#include <QObject>
#include <QVariant>
#include <QDebug>

Q_TK_BEGIN_CONSTANTS
const char* const TKDOCUMENT_GENERAL_XML_TAG   = "tkDocumentExtra";  /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_VERSION_XML_TAG   = "Version";          /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_PRESENCE_XML_TAG  = "Presence";         /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_PRIORITY_XML_TAG  = "Priority";         /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_EXTRA_XML_TAG     = "Extras";           /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */
const char* const TKDOCUMENT_HTML_XML_TAG      = "tkDocumentHtml";   /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */

const char* const TKDOCUMENT_XML_ACTUALVERSION = "1.0";              /*!< \brief tkDocumentExtra Xml Tag. \sa tkDocumentExtra::toXml(), tkDocumentExtra::fromXml() */

Q_TK_END_CONSTANTS

Q_TK_USING_CONSTANTS

/**
  \brief This class is used by tkPrinter for header/footer/watermark management.
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
  \li set the Presence of the document according to tkPrinter::Presence\n
  \li set the Priority of the document according to tkPrinter::Priority\n
  \li get/set to/from Xml using toXml() , fromXml()\n

  \sa tkPrinter
  \ingroup object_toolkit
*/
class Q_TK_EXPORT tkTextDocumentExtra
{
public:
    tkTextDocumentExtra() : m_Doc(0)
    {
        xmlVersion = TKDOCUMENT_XML_ACTUALVERSION;
        m_Html = "";
        m_Priority = tkPrinter::First;
        m_Presence = tkPrinter::EachPages;
        m_DocCreated = false;
    }

    tkTextDocumentExtra( const QString &html, const int presence = tkPrinter::EachPages, const int priority = tkPrinter::First, const QString &version = QString::null ) : m_Doc(0)
    {
        if (version.isEmpty())
            xmlVersion = TKDOCUMENT_XML_ACTUALVERSION;
        else
            xmlVersion = version;
        m_Priority = tkPrinter::Priority(priority);
        m_Presence = tkPrinter::Presence(presence);
        m_Html = html;
        m_DocCreated = false;
    }

    ~tkTextDocumentExtra();

    void setPriority( tkPrinter::Priority p )  { m_Priority = p; }
    void setPresence( tkPrinter::Presence p )  { m_Presence = p; }
    void setHtml( const QString &html)         { m_Html = html; if (m_DocCreated) m_Doc->setHtml(html); }

    tkPrinter::Priority priority() const { return m_Priority; }
    tkPrinter::Presence presence() const { return m_Presence; }

    QTextDocument *document() const;

    void setTextWidth( qreal width );
    bool lessThan( const tkTextDocumentExtra *h1, const tkTextDocumentExtra *h2 )
    {
        /** \todo  ? */
        return true;
    }

    QString toXml() const
    {
        QHash<QString,QString> t;
        t.insert(TKDOCUMENT_VERSION_XML_TAG, xmlVersion);
        t.insert(TKDOCUMENT_PRESENCE_XML_TAG,QString::number(presence()));
        t.insert(TKDOCUMENT_PRIORITY_XML_TAG, QString::number(priority()));
        t.insert(TKDOCUMENT_EXTRA_XML_TAG, "");
        t.insert(TKDOCUMENT_HTML_XML_TAG, tkGlobal::toHtmlAccent(toHtml()));
        return tkGlobal::createXml(TKDOCUMENT_GENERAL_XML_TAG,t, 4, false);
    }

    static tkTextDocumentExtra *fromXml(const QString &xml)
    {
        if (!xml.contains(QString("<%1>").arg(TKDOCUMENT_GENERAL_XML_TAG)))
            return new tkTextDocumentExtra();
        QHash<QString, QString> t;
        if (!tkGlobal::readXml(xml, TKDOCUMENT_GENERAL_XML_TAG, t, false))
            return new tkTextDocumentExtra();
        return new tkTextDocumentExtra(t.value(TKDOCUMENT_HTML_XML_TAG),
                                       tkPrinter::Presence(t.value(TKDOCUMENT_PRESENCE_XML_TAG).toInt()),
                                       tkPrinter::Priority(t.value(TKDOCUMENT_PRIORITY_XML_TAG).toInt()),
                                       t.value(TKDOCUMENT_VERSION_XML_TAG));
    }

    QString toHtml() const;

private:
    tkPrinter::Presence  m_Presence;
    tkPrinter::Priority  m_Priority;
    QString xmlVersion;
    QString m_Html;
    mutable bool m_DocCreated;
    mutable QTextDocument *m_Doc;
};
Q_DECLARE_METATYPE(tkTextDocumentExtra)

#endif // TKTEXTDOCUMENTHEADER_H
