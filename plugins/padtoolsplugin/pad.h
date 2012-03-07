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
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#ifndef PAD_H
#define PAD_H

#include <QList>
#include <QMap>
#include <QVariant>

#include "pad_fragment.h"
#include "pad_core.h"

/**
 * Contains an entire pad i.e. a list of fragments
 * @class
 */
class Pad : public PadFragment
{
public:
	Pad(){}
	virtual ~Pad();

	void addFragment(PadFragment *fragment);

	QList<PadFragment*> getAllFragments() const;

	void print(int indent = 0) const;

	/**
	 * Run this pad over some tokens and returns the result text
	 */
	QString run(QMap<QString,QVariant> &tokens) const;

private:
	QList<PadFragment*> _fragments;
};

#endif
