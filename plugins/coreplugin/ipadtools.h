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
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 ***************************************************************************/
#ifndef CORE_IPADTOOLS_H
#define CORE_IPADTOOLS_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QSyntaxHighlighter>

#include <coreplugin/core_exporter.h>

class QTextEdit;

/**
 * \file ipadtools.h
 * \author Eric Maeker, Guillaume DENRY
 * \version 0.8.0
 * \date 16 Aug 2012
 */

namespace Core {
class ICore;

class CORE_EXPORT PadAnalyzerError
{
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
    TokenDescription(const QString &uid);
    virtual ~TokenDescription() {}

    virtual QString uid() const {return _uid;}
    virtual QString humanReadableName() const;
    virtual QString tooltip() const;
    virtual QString helpText() const;
    virtual QString shortHtmlDescription() const;

    virtual void setUid(const QString &uid) {_uid=uid;}
    virtual void setTrContext(const QString &context) {_trContext=context;}
    virtual void setUntranslatedTooltip(const QString &tooltip) {_tooltip=tooltip;}
    virtual void setUntranslatedHelpText(const QString &help) {_help=help;}
    virtual void setUntranslatedHumanReadableName(const QString &name) {_human=name;}
    virtual void setUntranslatedHtmlDescription(const QString &descr) {_descr=descr;}

    bool operator<(const TokenDescription &descr) const;

private:
    QString _uid, _human, _tooltip, _descr, _help, _trContext;
};

class CORE_EXPORT TokenNamespace : public Core::TokenDescription
{
public:
    TokenNamespace(const QString &name = QString::null) : Core::TokenDescription(name) {}
    ~TokenNamespace() {}

    bool isValid() const {return !uid().isEmpty();}

    void addChild(const Core::TokenNamespace &child);
    QList<Core::TokenNamespace> children() const;
    void clearChildren();

private:
    QList<Core::TokenNamespace> _children;
};

class CORE_EXPORT IToken : public TokenDescription
{
public:
    IToken(const QString &fullName) : TokenDescription(fullName), _sort(0) {}
    virtual ~IToken() {}

    void setSortIndex(int index) {_sort=index;}
    virtual int sortIndex() const {return _sort;}

    virtual QString tooltip() const;

    virtual QVariant testValue() const = 0;
    virtual QVariant value() const = 0;

    static bool sortIndexLessThan(IToken *one, IToken *two) {return one->sortIndex() < two->sortIndex();}
private:
    int _sort;
};

class CORE_EXPORT ITokenPool : public QObject
{
    Q_OBJECT
    friend class Core::ICore;

protected:
    ITokenPool(QObject *parent = 0) : QObject(parent) {}

public:
    virtual ~ITokenPool() {}

    // Token namespaces
    virtual void registerNamespace(const Core::TokenNamespace &ns) = 0;
    virtual int rootNamespaceCount() const = 0;
    virtual const Core::TokenNamespace &rootNamespaceAt(int index) const = 0;
    virtual Core::TokenNamespace getTokenNamespace(const QString &name) const = 0;

    // Tokens
    virtual void addToken(Core::IToken *token) = 0;
    virtual void addTokens(const QVector<Core::IToken *> &tokens) = 0;
    virtual Core::IToken *token(const QString &name) = 0;
    virtual void removeToken(Core::IToken *token) = 0;

    virtual QList<Core::IToken *> tokens() const = 0;

    // Get token values
    virtual QVariant tokenTestingValue(const QString &name) = 0;
    virtual QVariant tokenCurrentValue(const QString &name) = 0;

Q_SIGNALS:
    void tokenAdded(Core::IToken *token);
    void tokenRemoved(Core::IToken *token);
};

class IPadTools;
class CORE_EXPORT IPadWriter : public QWidget
{
    Q_OBJECT
    friend class IPadTools;
protected:
    explicit IPadWriter(QWidget *parent = 0) : QWidget(parent) {}

public:
    virtual ~IPadWriter() {}

public Q_SLOTS:
    virtual void setPlainTextSource(const QString &plainText) = 0;
    virtual void setHtmlSource(const QString &html) = 0;
    virtual void setNamespaceFilter(const QString &tokenNamespace) = 0;
    virtual void setNamespacesFilter(const QStringList &tokenNamespaces) = 0;

public:
    virtual QString outputToPlainText() const = 0;
    virtual QString outputToHtml() const = 0;

    virtual QString rawSourceToPlainText() const = 0;
    virtual QString rawSourceToHtml() const = 0;
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

    virtual Core::IPadWriter *createWriter(QWidget *parent = 0) = 0;
};

}  // namespace Core


#endif  // CORE_IPADTOOLS_H
