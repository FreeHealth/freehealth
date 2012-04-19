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
/** \class PadTools::PadFragment
  * Represents a fragment of the raw/output source
*/

/** \fn virtual void PadTools::PadFragment::setParent(PadFragment *parent)
  Define the parent of the fragment */

/** \fn virtual PadFragment *PadTools::PadFragment::parent() const
  Return the parent of the fragment */

/** \fn virtual void PadTools::PadFragment::debug(int indent = 0) const = 0;
  Debug to console */

/** \fn virtual int PadTools::PadFragment::id() const
  Returns the start position in the raw source string/document */

/** \fn virtual void PadTools::PadFragment::setId(int id)
  Defines the start position in the raw source string/document */

/** \fn int PadTools::PadFragment::start() const { return _start; }
  Returns the start position in the raw source string/document */

/** \fn void PadTools::PadFragment::setStart(int start) { _start = start; }
  Defines the start position in the raw source string/document */

/** \fn int PadTools::PadFragment::end() const { return _end; }
 Returns the end position in the raw source string/document */

/** \fn void PadTools::PadFragment::setEnd(int end) { _end = end; }
  Defines the end position in the raw source string/document */

/** \fn int PadTools::PadFragment::rawLength() const {return _end - _start;}
  Return the length of the fragment in the raw source string/document */

/** \fn int PadTools::PadFragment::outputLength() const {return _outputEnd - _outputStart;}
  Return the length of the fragment in the output string/document */

/** \fn void PadTools::PadFragment::move(int nbChars);
  */

/** \fn void PadTools::PadFragment::moveEnd(int nbOfChars);
  */

/** \fn void PadTools::PadFragment::setOutputStart(const int pos)
  */

/** \fn void PadTools::PadFragment::setOutputEnd(const int pos)
  */

/** \fn int PadTools::PadFragment::outputStart() const
  Returns the start position in the output QTextDocument. This is defined only after the calling run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const. */

/** \fn int PadTools::PadFragment::outputEnd() const
  Returns the end position in the output QTextDocument. This is defined only after the calling run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const. */

/** \fn void PadTools::PadFragment::resetOutputRange() {_outputStart=-1; _outputEnd=-1; foreach(PadFragment*f,_fragments) f->resetOutputRange();}
  Removes (set to -1) all output limits to the object and its children. */

/** \fn bool PadTools::PadFragment::containsRawPosition(const int pos) const {return (_start <= pos && _end >= pos);}
  Contains raw source \e position */

/** \fn bool PadTools::PadFragment::containsOutputPosition(const int pos) const {return (_outputStart <= pos && _outputEnd >= pos);}
  Contains output \e position */

/** \fn void PadTools::PadFragment::setToolTip(const QString &tooltip)
  */

/** \fn const QString &PadTools::PadFragment::toolTip() const
  */

/** \fn void PadTools::PadFragment::setUserData(const QString &key, const QVariant &value)
  */

/** \fn QVariant PadTools::PadFragment::userData(const QString &key) const
  */

/** \fn virtual QString PadTools::PadFragment::run(QMap<QString,QVariant> &tokens) const = 0;
  Run this fragment over some tokens and returns the result text */

/** \fn virtual void PadTools::PadFragment::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const = 0;
  Run this fragment over some tokens inside QTextDocuments */

/** \fn virtual void PadTools::PadFragment::run(QMap<QString,QVariant> &tokens, PadDocument *document) = 0;
  Run this fragment over some tokens inside the output QTextDocument (which is initially a clone of the source). */

/** \fn virtual QList<PadFragment*> PadTools::PadFragment::children() const
   Returns the list of PadTools::PadFragment children */



#include "pad_fragment.h"

#include <utils/global.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QClipboard>
#include <QApplication>

#include <QDebug>

using namespace PadTools;

PadFragment::PadFragment(PadFragment *parent) :
    _start(-1), _end(-1),
    _outputStart(-1), _outputEnd(-1),
    _parent(parent),
    _id(-1)
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

/**
  * Test if a position is inside a fragment (using the raw source positionning).
  * By default, the start and end position are considered as part of the fragment
  * (meaning that containsRawPosition(start()) == true).
  * This function is used by:
  *    - PadTools::PadFragment::outputPosChanged().
  *    - PadTools::PadFragment::padFragmentForSourcePosition().
*/
bool PadFragment::containsRawPosition(const int pos) const
{
    return IN_RANGE(pos, _start, _end);
}

/**
  * Test if a position is inside a fragment (using the output positionning).
  * By default, the start and end position are considered as part of the fragment
  * (meaning that containsOutputPosition(outputStart()) == true).
  * This function is used by:
  *    - PadTools::PadFragment::outputPosChanged().
  *    - PadTools::PadFragment::padFragmentForOutputPosition().
*/
bool PadFragment::containsOutputPosition(const int pos) const
{
    return IN_RANGE(pos, _outputStart, _outputEnd);
}

/**
  * Test if a position is before a fragment (using the output positionning).
  * By default, the start and end position are considered as part of the fragment
  * (meaning that isBeforeOutputPosition(outputStart()) == false).
  * This function is used by:
  *    - PadTools::PadFragment::translateOutput().
  *    - PadTools::PadFragment::outputPosChanged().
*/
bool PadFragment::isBeforeOutputPosition(const int pos) const
{
    return pos > _outputEnd;
}

/**
  * Test if a position is after a fragment (using the output positionning).
  * By default, the start and end position are considered as part of the fragment
  * (meaning that isAfterOutputPosition(outputEnd()) == false).
  * This function is used by:
  *    - PadTools::PadFragment::translateOutput().
  *    - PadTools::PadFragment::outputPosChanged().
*/
bool PadFragment::isAfterOutputPosition(const int pos) const
{
    return pos < _outputStart;
}

/** Return the smallest PadTools::PadFragment that includes the position. All children are checked. Return 0 if the pos is not included in the fragment. */
PadFragment *PadFragment::padFragmentForSourcePosition(int pos) const
{
    if (!containsRawPosition(pos))
//    if (_start > pos || _end < pos)
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

//    qWarning() << pos << _outputStart << _outputEnd << (!IN_RANGE(pos, _outputStart, _outputEnd)) << (pos <= _outputStart || pos >= _outputEnd);
//    qWarning() << "(" <<  pos  << "<=" <<_outputStart<< "||"  << pos <<">=" <<_outputEnd<<")";

    if (!containsOutputPosition(pos))
//    if (pos <= _outputStart || pos >= _outputEnd)
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

void PadFragment::outputPosChanged(const int oldPos, const int newPos)
{
//    QString debug = QString("outputPosChanged : Fragment %1 (%2:%3)\n"
//                            "    old: %4  new: %5\n")
//                    .arg(_id)
//                    .arg(_outputStart).arg(_outputEnd)
//                    .arg(oldPos).arg(newPos);
    int delta = newPos - oldPos;

    // oldPos inside the fragment
//    debug += QString("    delta: %1\n").arg(delta);

    qWarning() << "outputPosChanged" << containsOutputPosition(oldPos);

    if (containsOutputPosition(oldPos)) {
//    if (_outputStart <= oldPos  && oldPos < _outputEnd) {
//        debug += QString("    oldPos is inside token\n");
        // Remove chars
        if (delta < 0) {
            // Start removed ?
            if (_outputStart > newPos)
                _outputStart = newPos;
            // Remove chars in fragment
            moveOutputEnd(delta);
        } else {
            // Add chars
            moveOutputEnd(delta);
        }
        foreach(PadFragment *f, children()) {
            if (f!=this)
                f->outputPosChanged(oldPos, newPos);
        }
    } else {
//        debug += QString("    move: %1\n").arg((_outputStart > oldPos));
        // oldPos outside fragment
        if (isAfterOutputPosition(oldPos)) {
            translateOutput(delta);
            foreach(PadFragment *f, children()) {
                if (f!=this)
                    f->outputPosChanged(oldPos, newPos);
            }
        }
    }
//    debug += QString("    new: (%1;%2)\n").arg(_outputStart).arg(_outputEnd);

//    qWarning() << debug;
}

///** Insert the content of the PadFragment rawSource to the output */
//void PadFragment::insertFragment(QTextDocument *source, QTextDocument *out) const
//{
//    if (_start>=0) {
//        QTextCursor cursor(source);
//        cursor.setPosition(_start, QTextCursor::MoveAnchor);
//        cursor.setPosition(_end, QTextCursor::KeepAnchor);

//        QTextCursor toCursor(out);
//        toCursor.movePosition(QTextCursor::End);
//        _outputStart = toCursor.position();

//        toCursor.insertHtml(cursor.selection().toHtml());

//        toCursor.movePosition(QTextCursor::End);
//        _outputEnd = toCursor.position();
//    }
//}

///** Insert html at the end of the output \e out QTextDocument and compute fragment output range */
//void PadFragment::insertText(QTextDocument *out, const QString &text) const
//{
//    if (_start>=0) {
//        QTextCursor toCursor(out);
//        toCursor.movePosition(QTextCursor::End);
//        _outputStart = toCursor.position();
//        toCursor.insertHtml(text);
//        toCursor.movePosition(QTextCursor::End);
//        _outputEnd = toCursor.position();
//    }
//}

/** Moves the PadTools::PadFragment from \e nbChars. \e nbChars can be a positive (moving forward) or a negative int (moving backward). Does not manage children fragments.*/
void PadFragment::translateOutput(int nbChars)
{
    _outputStart+=nbChars;
    _outputEnd+=nbChars;
}

/** Move the end tag of the fragment in rawsource and output. Does not manage parent fragment. */
void PadFragment::moveOutputEnd(int nbOfChars)
{
    if (_outputEnd + nbOfChars < _outputStart) {
        _outputEnd = _outputStart;
    } else {
        _outputEnd += nbOfChars;
    }
}
