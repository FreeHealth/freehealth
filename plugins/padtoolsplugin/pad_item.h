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

#include <QList>
#include <QMap>
#include <QVariant>

#include "pad_fragment.h"
#include "pad_core.h"

namespace PadTools {

/**
 * Contains an entire pad item i.e. a list of fragments
 * @class
 */
class PadItem : public PadFragment
{
public:
    enum PadStringType {
        NoType = 0,
        Core,
        ConditionnalBeforeText,
        ConditionnalAfterText
    };

	PadItem(){}
	virtual ~PadItem();

    void addFragment(PadFragment *fragment, int type = NoType);
	QList<PadFragment*> getAllFragments() const;
    PadFragment *fragment(const int type) const;

	void print(int indent = 0) const;

	QString run(QMap<QString,QVariant> &tokens) const;
    void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const;

private:
	QList<PadFragment*> _fragments;

	PadCore *getCore() const;
};

}  // PadTools

#endif
