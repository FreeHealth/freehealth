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
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 ***************************************************************************/

#ifndef IPADTOOLS_H
#define IPADTOOLS_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QSyntaxHighlighter>

#include <coreplugin/core_exporter.h>

class QTextEdit;

/**
 * \file ipadtools.h
 * \author Eric Maeker <eric.maeker@gmail.com>, Guillaume DENRY <guillaume.denry@gmail.com>
 * \version 0.8.0
 * \date 05 May 2012
 */

namespace Core {
class ICore;

class CORE_EXPORT PadAnalyzerError {
public:
    enum Type {
        Error_NoError,
        Error_UnexpectedChar,
        Error_CoreDelimiterExpected
    };

    PadAnalyzerError(Type errorType, int pos, const QMap<QString,QVariant> &errorTokens) :
        _errorType(errorType),
        _pos(pos),
        _errorTokens(errorTokens) {}

    int errorType() const { return _errorType; }
    int pos() const { return _pos; }
    const QMap<QString,QVariant> &errorTokens() const { return _errorTokens; }

private:
    Type _errorType;
    int _pos;
    QMap<QString,QVariant> _errorTokens;
};


class CORE_EXPORT TokenDescription
{
public:
    TokenDescription(const QString &name);
    virtual ~TokenDescription() {}

    virtual QString fullName() const {return _name;}
    virtual QString humanReadableName() const;
    virtual QString tooltip() const;
    virtual QString helpText() const;
    virtual QString shortHtmlDescription() const;

    virtual void setTrContext(const QString &context) {_trContext=context;}
    virtual void setUntranslatedTooltip(const QString &tooltip) {_tooltip=tooltip;}
    virtual void setUntranslatedHelpText(const QString &help) {_help=help;}
    virtual void setUntranslatedHumanReadableName(const QString &name) {_human=name;}
    virtual void setUntranslatedHtmlDescription(const QString &descr) {_descr=descr;}

    bool operator<(const TokenDescription &descr) const;

private:
    QString _name, _human, _tooltip, _descr, _help, _trContext;
};

class CORE_EXPORT TokenNamespace : public TokenDescription
{
public:
    TokenNamespace(const QString &name = QString::null) : TokenDescription(name) {}
    ~TokenNamespace() {}

    bool isValid() const {return !fullName().isEmpty();}
};


class CORE_EXPORT IToken : public TokenDescription
{
public:
    IToken(const QString &fullName) : TokenDescription(fullName) {}
    virtual ~IToken() {}

    virtual QVariant testValue() const = 0;
    virtual QVariant value() const = 0;
};


class CORE_EXPORT ITokenPool : public QObject
{
    Q_OBJECT
    friend class Core::ICore;

protected:
    ITokenPool(QObject *parent = 0) : QObject(parent) {}

public:
    virtual ~ITokenPool() {}

//    virtual TokenNamespace &createNamespace(const QString &name) = 0;
//    virtual void registerNamespace(const TokenNamespace &ns) = 0;
//    virtual const TokenNamespace &getTokenNamespace(const QString &name) const = 0;

    virtual void addToken(Core::IToken *token) = 0;
    virtual void addTokens(QVector<Core::IToken *> &tokens) = 0;
    virtual Core::IToken *token(const QString &name) = 0;
    virtual void removeToken(Core::IToken *token) = 0;

    virtual QVector<Core::IToken *> tokens() const = 0;

    virtual QVariant tokenTestingValue(const QString &name) = 0;
    virtual QVariant tokenCurrentValue(const QString &name) = 0;

Q_SIGNALS:
    void tokenAdded(Core::IToken *token);
    void tokenRemoved(Core::IToken *token);
};


class CORE_EXPORT IPadTools : public QObject
{
    Q_OBJECT
public:
    IPadTools(QObject * parent = 0) : QObject(parent) {}
    virtual ~IPadTools() {}

    virtual Core::ITokenPool *tokenPool() const = 0;

    virtual QString processPlainText(const QString &plainText) {Q_UNUSED(plainText); return QString::null;}
    virtual QString processHtml(const QString &html) {Q_UNUSED(html); return QString::null;}

    virtual QString parse(const QString &templ, QMap<QString,QVariant> &tokens, QList<PadAnalyzerError> &errors) = 0;
    virtual QSyntaxHighlighter *createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens) { Q_UNUSED(textEdit); Q_UNUSED(tokens); return NULL ; }
};

}


#endif
