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
  \class PadTools::PadItem
  Represents a full token including:
  - a PadCore (the token name)
  - fragments of conditional texts (before and after)
*/

#include "pad_item.h"
#include "constants.h"

#include <QString>
#include <QTextCursor>
#include <QTextDocumentFragment>

#include <QDebug>

using namespace PadTools;

PadItem::~PadItem()
{
	qDeleteAll(_fragments);
}

void PadItem::addFragment(PadFragment *fragment)
{
	_fragments << fragment;
}

void PadItem::print(int indent) const
{
	QString str(indent, ' ');
    str += QString("[padItem:Source(%1;%2);Output(%3;%4)]")
            .arg(start()).arg(end())
            .arg(outputStart()).arg(outputEnd());
	qDebug("%s", qPrintable(str));
    foreach (PadFragment *fragment, _fragments) {
		fragment->print(indent + 2);
	}
}

/** Find nested PadItem in conditional texts (before and after) */
QList<PadFragment*> PadItem::getAllFragments() const
{
	QList<PadFragment*> fragments;
	PadItem *padItem;
	fragments.append(_fragments);
    foreach (PadFragment *fragment, _fragments) {
		padItem = dynamic_cast<PadItem*>(fragment);
		if (padItem)
			fragments.append(padItem->getAllFragments());
	}
	return fragments;
}

PadCore *PadItem::getCore() const
{
    PadCore *core;
    foreach (PadFragment *fragment, _fragments) {
        core = dynamic_cast<PadCore*>(fragment);
        if (core)
            return core;
    }
    return 0;
}

/**
 * Run this pad over some tokens and returns the result text
 */
QString PadItem::run(QMap<QString,QVariant> &tokens) const
{
	QString value;
	PadCore *core = getCore();
	QString coreValue;

	// if a core exists, the entire pad expression is optional, depending on the core emptiness
    if (core) {
		coreValue = core->run(tokens);
		if (coreValue.isEmpty()) // core empty? so the entire pad will be empty too
			return "";
	}

	foreach (PadFragment *fragment, _fragments)
		value += fragment->run(tokens);

	return value;
}

void PadItem::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const
{
    PadCore *core = getCore();
    QString coreValue;

    // if a core exists, the entire pad expression is optional, depending on the core emptiness
    if (core) {
        coreValue = core->name();
        if (coreValue.isEmpty())
            return;
    }

    int start = -1;
    int end = -1;
    foreach(PadFragment *fragment, _fragments) {
        fragment->run(tokens, source, out);
        if (fragment->outputStart() < start || start == -1)
            start = fragment->outputStart();
        if (fragment->outputEnd() > end)
            end = fragment->outputEnd();
    }

    // Trace position in output
    _outputStart = start;
    _outputEnd = end;

    // Add the format && tooltip
    QTextCursor cursor(out);
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);

    QTextCharFormat format;// = Constants::tokenCharFormat();
    format.setToolTip("Token: " + coreValue);
    cursor.setCharFormat(format);
    return;
}
