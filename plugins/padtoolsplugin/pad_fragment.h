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
    PadFragment(PadFragment *parent = 0);
    virtual ~PadFragment();

    virtual void setParent(PadFragment *parent) {_parent = parent;}
    virtual PadFragment *parent() const {return _parent;}

	virtual void print(int indent = 0) const = 0;

    /** Returns the start position in the raw source string/document */
    virtual int id() const { return _id; }
    /** Defines the start position in the raw source string/document */
    virtual void setId(int id) { _id = id; }

    /** Returns the start position in the raw source string/document */
	int start() const { return _start; }
    /** Defines the start position in the raw source string/document */
    void setStart(int start) { _start = start; }

    /** Returns the end position in the raw source string/document */
    int end() const { return _end; }
    /** Defines the end position in the raw source string/document */
    void setEnd(int end) { _end = end; }

    /** Return the length of the fragment in the raw source string/document */
    int rawLength() const {return _end - _start;}
    /** Return the length of the fragment in the output string/document */
    int outputLength() const {return _outputEnd - _outputStart;}

    void move(int nbChars);
    void moveEnd(int nbOfChars);

    /** Returns the start position in the output QTextDocument. This is defined only after the calling run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const. */
    int outputStart() const {return _outputStart;}
    /** Returns the end position in the output QTextDocument. This is defined only after the calling run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const. */
    int outputEnd() const {return _outputEnd;}

    /** Contains raw source \e position */
    bool containsRawPosition(const int pos) const {return (_start <= pos && _end >= pos);}
    /** Contains output \e position */
    bool containsOutputPosition(const int pos) const {return (_outputStart <= pos && _outputEnd >= pos);}

    void setToolTip(const QString &tooltip) {_toolTip = tooltip;}
    const QString &toolTip() const {return _toolTip;}

    void setUserData(const QString &key, const QVariant &value) {_userData.insert(key, value);}
    QVariant userData(const QString &key) const {return _userData.value(key);}

    /**  Run this fragment over some tokens and returns the result text */
	virtual QString run(QMap<QString,QVariant> &tokens) const = 0;

    /**  Run this fragment over some tokens inside QTextDocuments */
    virtual void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const = 0;

    virtual void addChild(PadFragment *fragment);
    virtual void removeAndDeleteFragment(PadFragment *fragment);
    /** Returns the list of PadTools::PadFragment children */
    virtual QList<PadFragment*> children() const {return _fragments;}

    virtual PadFragment *padFragmentForSourcePosition(int pos) const;
    virtual PadFragment *padFragmentForOutputPosition(int pos) const;

    void insertFragment(QTextDocument *source, QTextDocument *out) const;
    void insertText(QTextDocument *out, const QString &text) const;

protected:
    QList<PadFragment *> _fragments;

private:
    PadFragment *_parent;
    int _start; // index of the first char in the text
	int _end; // index of the last char in the text
    long long _id; // unique identifier
    QString _toolTip;
    QHash<QString, QVariant> _userData;

protected:
    mutable int _outputStart, _outputEnd;
};

}  // PadTools

#endif
