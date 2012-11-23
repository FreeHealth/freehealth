/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 ***************************************************************************/
#ifndef TEXTDOCUMENTEXTRA_H
#define TEXTDOCUMENTEXTRA_H

#include <printerplugin/printer_exporter.h>
#include <printerplugin/printer.h>

#include <QTextDocument>
#include <QObject>

/**
  \brief This class is used by Printer for header/footer management.
  \sa Printer
 */

namespace Print {

class PRINTER_EXPORT TextDocumentExtra : public QTextDocument
{
    Q_OBJECT
public:
    TextDocumentExtra( QObject *parent = 0 ): QTextDocument( parent ) {}
    TextDocumentExtra( const QString & text, QObject * parent = 0 ): QTextDocument( text, parent ) {}
    ~TextDocumentExtra() {}

    void setPriority( Printer::Priority p )  { m_Priority = p; }
    void setPresence( Printer::Presence p )  { m_Presence = p; }

    Printer::Priority priority() { return m_Priority; }
    Printer::Presence presence() { return m_Presence; }

    bool lessThan( const TextDocumentExtra *h1, const TextDocumentExtra *h2 )
    {
        /** \todo  ? */
        return true;
    }

private:
    Printer::Presence  m_Presence;
    Printer::Priority  m_Priority;
};

}  // End Print

#endif // TEXTDOCUMENTEXTRA_H
