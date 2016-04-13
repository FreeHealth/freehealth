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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DOCUMENTPRINTER_H
#define DOCUMENTPRINTER_H

#include <coreplugin/idocumentprinter.h>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

/**
 * \file documentprinter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 22 Apr 2013
*/


namespace Print {
class Printer;
namespace Internal {

class DocumentPrinter : public Core::IDocumentPrinter
{
    Q_OBJECT

public:
    DocumentPrinter(QObject *parent = 0);
    ~DocumentPrinter();

    void clearTokens();
    void addTokens(const int tokenWhere, const QHash<QString, QVariant> &tokensAndValues);

    bool print(const QTextDocument &text, const int papers = Core::IDocumentPrinter::Papers_Generic_User, bool printDuplicata = false) const;
    bool print(QTextDocument *text, const int papers = Core::IDocumentPrinter::Papers_Generic_User, bool printDuplicata = false) const;
    bool print(const QString &html, const int papers = Core::IDocumentPrinter::Papers_Generic_User, bool printDuplicata = false) const;

    bool toPdf(const QString &html, const QString &absOutputFilePath, const int papers = Papers_Generic_User, bool printDuplicata = false) const;

    bool printPreview(const QString &html, const int papers = Papers_Generic_User, bool printDuplicata = false) const;

private:
    void prepareHeader(Print::Printer *p, const int papers) const;
    void prepareFooter(Print::Printer *p, const int papers) const;
    void prepareWatermark(Print::Printer *p, const int papers) const;
    void setDocumentName(Print::Printer *p) const;

private:
    QHash<QString, QVariant> headerTokens, footerTokens, watermarkTokens, globalTokens;

};

}  // End namespace Internal
}  // End namespace Print

#endif // DOCUMENTPRINTER_H
