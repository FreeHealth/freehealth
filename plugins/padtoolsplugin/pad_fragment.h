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
#ifndef PAD_FRAGMENT_H
#define PAD_FRAGMENT_H

#include <QString>
#include <QHash>
#include <QVariant>
#include <QTextDocument>

namespace PadTools {

/**
 * Represent a string or a token
 */
class PadFragment
{
public:
    PadFragment();

	virtual void print(int indent = 0) const = 0;

    /** Returns the start position in the raw source string/document */
    int id() const { return _id; }
    /** Defines the start position in the raw source string/document */
    void setId(int id) { _id = id; }

    /** Returns the start position in the raw source string/document */
	int start() const { return _start; }
    /** Defines the start position in the raw source string/document */
    void setStart(int start) { _start = start; }

    /** Returns the end position in the raw source string/document */
    int end() const { return _end; }
    /** Defines the end position in the raw source string/document */
    void setEnd(int end) { _end = end; }

    /** Returns the start position in the output QTextDocument. This is defined only after the calling run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const. */
    int outputStart() const {return _outputStart;}
    /** Returns the end position in the output QTextDocument. This is defined only after the calling run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const. */
    int outputEnd() const {return _outputEnd;}

    void setToolTip(const QString &tooltip) {_toolTip = tooltip;}
    const QString &toolTip() const {return _toolTip;}

    void setUserData(const int ref, const QVariant &value) {_userData.insert(ref, value);}
    QVariant userData(const int ref) const {return _userData.value(ref);}

    /**  Run this fragment over some tokens and returns the result text */
	virtual QString run(QMap<QString,QVariant> &tokens) const = 0;

    /**  Run this fragment over some tokens inside QTextDocuments */
    virtual void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const = 0;

    void insertFragment(QTextDocument *source, QTextDocument *out) const;
    void insertText(QTextDocument *out, const QString &text) const;

private:
	int _start; // index of the first char in the text
	int _end; // index of the last char in the text
    long long _id; // unique identifier
    QString _toolTip;
    QHash<int, QVariant> _userData;

protected:
    mutable int _outputStart, _outputEnd;
};

}  // PadTools

#endif
