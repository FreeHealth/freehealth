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
#include <QString>

#include "pad.h"
#include "pad_item.h"

Pad::~Pad()
{
	qDeleteAll(_fragments);
}

void Pad::addFragment(PadFragment *fragment)
{
	_fragments << fragment;
}

void Pad::print(int indent) const
{
	QString str(indent, ' ');
	str += "[pad]";
	qDebug("%s", qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
		fragment->print(indent + 2);
	}
}

QList<PadFragment*> Pad::getAllFragments() const
{
	QList<PadFragment*> fragments;
	PadItem *item;
	fragments.append(_fragments);
	foreach (PadFragment *fragment, _fragments)
	{
		item = dynamic_cast<PadItem*>(fragment);
		if (item)
			fragments.append(item->getAllFragments());
	}
	return fragments;
}

QString Pad::run(QMap<QString,QVariant> &tokens) const
{
	Q_UNUSED(tokens);

	QString value;

	foreach (PadFragment *fragment, _fragments)
		value += fragment->run(tokens);

	return value;
}
