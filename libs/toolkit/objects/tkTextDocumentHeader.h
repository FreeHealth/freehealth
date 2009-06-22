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
#ifndef TKTEXTDOCUMENTEXTRA_H
#define TKTEXTDOCUMENTEXTRA_H

#include <tkExporter.h>
#include <tkPrinter.h>

#include <QTextDocument>
#include <QObject>

/**
  \brief This class is used by tkPrinter for header/footer management.
  \sa tkPrinter
 */
class Q_TK_EXPORT tkTextDocumentExtra : public QTextDocument
{
    Q_OBJECT
public:
    tkTextDocumentExtra( QObject *parent = 0 ): QTextDocument( parent ) {}
    tkTextDocumentExtra( const QString & text, QObject * parent = 0 ): QTextDocument( text, parent ) {}
    ~tkTextDocumentExtra() {}

    void setPriority( tkPrinter::Priority p )  { m_Priority = p; }
    void setPresence( tkPrinter::Presence p )  { m_Presence = p; }

    tkPrinter::Priority priority() { return m_Priority; }
    tkPrinter::Presence presence() { return m_Presence; }

    bool lessThan( const tkTextDocumentExtra *h1, const tkTextDocumentExtra *h2 )
    {
        /** \todo  ? */
        return true;
    }

private:
    tkPrinter::Presence  m_Presence;
    tkPrinter::Priority  m_Priority;
};


#endif // TKTEXTDOCUMENTEXTRA_H
