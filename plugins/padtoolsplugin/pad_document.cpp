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

/**
 * \class PadTools::Pad
 * Contains all the PadTools::PadFragment of the analyzed \e rawSource.
 */

#include "pad_document.h"
#include "pad_item.h"

#include <QString>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include <QDebug>

using namespace PadTools;

/** Construct an empty PadTools::PadDocument with the \e rawSource text (text is not analyzed). */
PadDocument::PadDocument(const QString &rawSource) :
    _rawSource(rawSource),
    _docSource(0),
    _docOutput(0)
{
}

PadDocument::PadDocument(QTextDocument *output) :
    _docSource(output),
    _docOutput(0)
{}

PadDocument::PadDocument() :
    _docSource(0),
    _docOutput(0)
{}

PadDocument::~PadDocument()
{
	qDeleteAll(_fragments);
}

QString PadDocument::fragmentRawSource(PadFragment *fragment) const
{
    if (!fragment)
        return QString::null;
    if (!_rawSource.isEmpty()) {
        return _rawSource.mid(fragment->start(), fragment->end() - fragment->start());
    }
    if (_docSource) {
        return _docSource->toPlainText().mid(fragment->start(), fragment->end() - fragment->start());
    }
    return QString::null;
}

QString PadDocument::fragmentHtmlOutput(PadFragment *fragment) const
{
    if (!fragment)
        return QString::null;
    if (_docOutput) {
        QTextCursor cursor(_docOutput);
        cursor.setPosition(fragment->outputStart());
        cursor.setPosition(fragment->outputEnd(), QTextCursor::KeepAnchor);
        return cursor.selection().toHtml();
    }
    return QString::null;
}

/** Add string fragment to the object. */
void PadDocument::addFragment(PadFragment *fragment)
{
	_fragments << fragment;
    PadItem *item = dynamic_cast<PadItem *>(fragment);
    if (item)
        _items << item;
}

void PadDocument::print(int indent) const
{
	QString str(indent, ' ');
	str += "[pad]";
	qDebug("%s", qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
		fragment->print(indent + 2);
	}
}

/** Returns all fragment of the analyzed raw source string. \sa PadTools::PadAnalyzer::analyze() */
QList<PadFragment*> PadDocument::getAllFragments() const
{
	QList<PadFragment*> fragments;
	PadItem *item;
	fragments.append(_fragments);
    foreach (PadFragment *fragment, _fragments) {
		item = dynamic_cast<PadItem*>(fragment);
		if (item)
			fragments.append(item->getAllFragments());
	}
	return fragments;
}

PadItem *PadDocument::padItemForOutputPosition(int p) const
{
    foreach(PadItem *item, _items) {
        if (item->outputStart() <= p && item->outputEnd() >= p)
            return item;
    }
    return 0;
}

PadItem *PadDocument::padItemForSourcePosition(int p) const
{
    foreach(PadItem *item, _items) {
        if (item->start() <= p && item->end() >= p)
            return item;
    }
    return 0;
}

/** Run this pad over some tokens and returns the result text */
QString PadDocument::run(QMap<QString,QVariant> &tokens) const
{
	QString value;
	foreach (PadFragment *fragment, _fragments)
		value += fragment->run(tokens);
	return value;
}

void PadDocument::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const
{
    foreach (PadFragment *fragment, _fragments)
        fragment->run(tokens, source, out);
    _docOutput = out;

//    foreach(PadFragment *i, _fragments) {
//        i->print();
//        qWarning() << "\n\n";
//    }
}
