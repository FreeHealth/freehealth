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
#ifndef PAD_ITEM_H
#define PAD_ITEM_H

#include <padtoolsplugin/pad_fragment.h>

#include <QList>
#include <QMap>
#include <QVariant>

namespace PadTools {
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

    void addDelimiter(const int posInRaw, const int size);

    void run(QMap<QString,QVariant> &tokens);
    void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out);
    void run(QMap<QString,QVariant> &tokens, PadDocument *document);

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

    const QString &name() const { return _name; }
    void setName(const QString &name) { _name = name; }

    void debug(int indent = 0) const;

//    void run(QMap<QString,QVariant> &tokens);
//    void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *output);
    void run(QMap<QString,QVariant> &tokens, PadDocument *document);


private:
    QString _name;
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

//    PadFragment *fragment(const int type) const;

    void addDelimiter(const int posInRaw, const int size);

    void debug(int indent = 0) const;

//    void run(QMap<QString,QVariant> &tokens);
//    void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out);
    void run(QMap<QString,QVariant> &tokens, PadDocument *document);

    QList<PadFragment*> children() const;
	PadCore *getCore() const;

private:
    QList<PadDelimiter> _delimiters;
};

}  // PadTools

#endif
