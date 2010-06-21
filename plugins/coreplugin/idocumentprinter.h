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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef IDOCUMENTPRINTER_H
#define IDOCUMENTPRINTER_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QString>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QVariant;
QT_END_NAMESPACE

/**
 * \file idocumentprinter.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 21 June 2010
*/


namespace Core {

/**
  \brief This class is a Document Printer's manager. The printerplugin will create one instance of it and leave it in the pluginmanager pool.
  Get it, and use it for your printing. But remember to never delete it.

  \todo create a queue that manages printing jobs
*/

class CORE_EXPORT IDocumentPrinter : public QObject
{
    Q_OBJECT

public:
    enum PapersToUse {
        Papers_Generic_User = 0,
        Papers_Administrative_User,
        Papers_Prescription_User
    };

    enum TokensWhere {
        Tokens_Header = 0,
        Tokens_Footer,
        Tokens_Watermark,
        Tokens_Global
    };

    IDocumentPrinter(QObject *parent) : QObject(parent) {}
    virtual ~IDocumentPrinter() {}

    virtual void clearTokens() = 0;
    virtual void addTokens(const int tokenWhere, const QHash<QString, QVariant> &tokensAndValues) = 0;

    virtual bool print(const QTextDocument &text, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;
    virtual bool print(QTextDocument *text, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;
    virtual bool print(const QString &html, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;

};


}  // End namespace Core


#endif // IDOCUMENTPRINTER_H
