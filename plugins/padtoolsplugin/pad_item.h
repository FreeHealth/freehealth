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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_PAD_ITEM_H
#define PADTOOLS_PAD_ITEM_H

#include <padtoolsplugin/pad_fragment.h>

#include <QList>
#include <QMap>
#include <QVariant>

/**
 * \file pad_item.h
 * \author Eric Maeker, Guillaume Denry
 * \version 0.8.2
 * \date 25 Dec 2012
*/


namespace PadTools {
namespace Internal {
class PadDocument;

struct PadDelimiter {
    int rawPos, size;
};


class PadConditionnalSubItem : public PadFragment
{
public:
    enum TokenCoreCondition {
        Defined = 0,
        Undefined
    };
    enum Place {
        Prepend = 0,
        Append
    };

    PadConditionnalSubItem(TokenCoreCondition cond, Place place, PadFragment *parent = 0);
    virtual ~PadConditionnalSubItem() {}

    TokenCoreCondition tokenCoreCondition() const {return _coreCond;}
    Place place() const {return _place;}

    void addDelimiter(const int posInRaw, const int size);

    bool containsOutputPosition(const int pos) const;
    bool isBeforeOutputPosition(const int pos) const;
    bool isAfterOutputPosition(const int pos) const;

    void run(QMap<QString,QVariant> &tokens, PadDocument *document);
    void toOutput(Core::ITokenPool *pool, PadDocument *document, TokenReplacementMethod method);
    void toRaw(PadDocument *doc);

    void debug(int indent = 0) const;

private:
    TokenCoreCondition _coreCond;
    Place _place;
    QList<PadDelimiter> _delimiters;
};

class PadCore : public PadFragment
{
public:
    PadCore() : PadFragment() {}

    const QString &uid() const { return _uid; }
    void setUid(const QString &uid) { _uid = uid; }

    bool containsOutputPosition(const int pos) const;
    bool isBeforeOutputPosition(const int pos) const;
    bool isAfterOutputPosition(const int pos) const;

    void debug(int indent = 0) const;

    void run(QMap<QString,QVariant> &tokens, PadDocument *document);
    void toOutput(Core::ITokenPool *pool, PadDocument *document, TokenReplacementMethod method);
    void toRaw(PadDocument *doc);

    QString tokenValue(Core::ITokenPool *pool, TokenReplacementMethod method) const;

private:
    QString _uid;
};

class PadItem : public PadFragment
{
public:
    enum PadStringType {
        NoType = 0,
        Core,
        DefinedCore_PrependText,
        DefinedCore_AppendText,
        UndefinedCore_PrependText,
        UndefinedCore_AppendText
    };

    PadItem() : PadFragment() {}
	virtual ~PadItem();

    void addDelimiter(const int posInRaw, const int size);

    bool containsOutputPosition(const int pos) const;
    bool isBeforeOutputPosition(const int pos) const;
    bool isAfterOutputPosition(const int pos) const;

    void debug(int indent = 0) const;

    void run(QMap<QString,QVariant> &tokens, PadDocument *document);
    void toOutput(Core::ITokenPool *pool, PadDocument *document, TokenReplacementMethod method);
    void toRaw(PadDocument *doc);

    QList<PadFragment*> children() const;
	PadCore *getCore() const;
    PadConditionnalSubItem *subItem(const PadConditionnalSubItem::TokenCoreCondition cond, const PadConditionnalSubItem::Place place);

private:
    QList<PadDelimiter> _delimiters;
};

} // namespace Internal
} // namespace PadTools

#endif  // PADTOOLS_PAD_ITEM
