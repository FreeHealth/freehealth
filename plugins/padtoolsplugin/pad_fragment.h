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
#include <QMap>
#include <QVariant>

/**
 * Represent a string or a token
 */
class PadFragment
{
public:
	PadFragment() {}

	virtual void print(int indent = 0) const = 0;

	int start() const { return _start; }
	void setStart(int start) { _start = start; }
	int end() const { return _end; }
	void setEnd(int end) { _end = end; }

	const QString &rawValue() const { return _rawValue; }
	void setRawValue(const QString &value) { _rawValue = value; }

	/**
	 * Run this fragment over some tokens and returns the result text
	 */
	virtual QString run(QMap<QString,QVariant> &tokens) const = 0;

private:
	int _start; // index of the first char in the text
	int _end; // index of the last char in the text
	QString _rawValue;
};

#endif
