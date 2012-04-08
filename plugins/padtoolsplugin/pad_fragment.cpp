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
#include "pad_fragment.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QClipboard>
#include <QApplication>

#include <QDebug>

using namespace PadTools;

PadFragment::PadFragment(PadFragment *parent) :
    _start(-1),
    _end(-1),
    _id(-1),
    _outputStart(-1),
    _outputEnd(-1),
    _parent(parent)
{
}

/** When deleting an object, all its children will be deleted */
PadFragment::~PadFragment()
{
    qDeleteAll(_fragments);
    _fragments.clear();
    _parent = 0;
}

/** Add a PadTools::PadFragment as a direct child of this object. Children are stored in an ordered list. */
void PadFragment::addChild(PadFragment *fragment)
{
    fragment->setParent(this);
    _fragments << fragment;
}

/** Removes a PadTools::PadFragment from the object children. Children are stored in an ordered list. */
void PadFragment::removeAndDeleteFragment(PadFragment *fragment)
{
    if (_fragments.contains(fragment)) {
        _fragments.removeAll(fragment);
        delete fragment;
        fragment = 0;
    }
}

/** Return the smallest PadTools::PadFragment that include the position. All children are checked. Return 0 if the pos is not included in the fragment. */
PadFragment *PadFragment::padFragmentForSourcePosition(int pos) const
{
    if (_start > pos || _end < pos)
        return 0;
    if (_fragments.isEmpty())
        return (PadFragment*)(this);
    // check all children
    PadFragment *child = 0;
    foreach(PadFragment *frag, _fragments) {
        PadFragment *test = frag->padFragmentForSourcePosition(pos);
        if (test)
            child = test;
    }
    return child;
}

/** Return the smallest PadTools::PadFragment that include the position. All children are checked. Return 0 if the pos is not included in the fragment. */
PadFragment *PadFragment::padFragmentForOutputPosition(int pos) const
{
    if (_outputStart > pos || _outputEnd < pos)
        return 0;
    if (_fragments.isEmpty())
        return (PadFragment*)(this);
    // check all children
    PadFragment *child = 0;
    foreach(PadFragment *frag, _fragments) {
        PadFragment *test = frag->padFragmentForOutputPosition(pos);
        if (test)
            child = test;
    }
    return child;
}

/** Insert the content of the PadFragment rawSource to the output */
void PadFragment::insertFragment(QTextDocument *source, QTextDocument *out) const
{
    if (_start>=0) {
        QTextCursor cursor(source);
        cursor.setPosition(_start, QTextCursor::MoveAnchor);
        cursor.setPosition(_end, QTextCursor::KeepAnchor);

        QTextCursor toCursor(out);
        toCursor.movePosition(QTextCursor::End);
        _outputStart = toCursor.position();

        toCursor.insertHtml(cursor.selection().toHtml());

        toCursor.movePosition(QTextCursor::End);
        _outputEnd = toCursor.position();
    }
}

/** Insert html at the end of the output \e out QTextDocument and compute fragment output range */
void PadFragment::insertText(QTextDocument *out, const QString &text) const
{
    if (_start>=0) {
        QTextCursor toCursor(out);
        toCursor.movePosition(QTextCursor::End);
        _outputStart = toCursor.position();
        toCursor.insertHtml(text);
        toCursor.movePosition(QTextCursor::End);
        _outputEnd = toCursor.position();
    }
}

/** Moves the PadTools::PadFragment from \e nbChars. \e nbChars can be a positive (moving forward) or a negative int (moving backward). Manages children too.*/
void PadFragment::move(int nbChars)
{
    _start+=nbChars;
    _outputStart+=nbChars;
    _end+=nbChars;
    _outputEnd+=nbChars;
    foreach(PadFragment *f, _fragments)
        f->move(nbChars);
}

/** Move the end tag of the fragment in rawsource and output. Manages parent end too. */
void PadFragment::moveEnd(int nbOfChars)
{
    _end += nbOfChars;
    _outputEnd += nbOfChars;
    if (_parent && _parent->id()>=0)
        _parent->moveEnd(nbOfChars);
}
