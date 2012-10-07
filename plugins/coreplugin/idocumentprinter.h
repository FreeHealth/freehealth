/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDOCUMENTPRINTER_H
#define IDOCUMENTPRINTER_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QVariant;
QT_END_NAMESPACE

/**
 * \file idocumentprinter.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.2
 * \date 13 July 2010
*/


namespace Core {
class IDocumentPrinter;

class CORE_EXPORT PrintedDocumentTracer
{
    friend class IDocumentPrinter;

public:
    PrintedDocumentTracer() {}
    ~PrintedDocumentTracer() {}

    QString documentName() const {return m_docName;}
    QString fileName() const {return m_fileName;}
    QString userUid() const {return m_userUid;}
    QDateTime dateTime() const {return m_dateTime;}

protected:
    QString m_docName, m_fileName, m_userUid;
    QDateTime m_dateTime;
};

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

    virtual bool printPreview(const QString &html, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;

    static void addPrintedDoc(const QString &fileName, const QString &docName, const QDateTime &dt, const QString &userUid);
    static QList<PrintedDocumentTracer> printedDocs() {return m_Docs;}

private:
    static QList<PrintedDocumentTracer> m_Docs;
};


}  // End namespace Core


#endif // IDOCUMENTPRINTER_H
