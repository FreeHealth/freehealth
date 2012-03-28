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
#ifndef PAD_DOCUMENT_H
#define PAD_DOCUMENT_H

#include <QList>
#include <QMap>
#include <QVariant>

#include "pad_fragment.h"
#include "pad_core.h"

namespace PadTools {

class PadDocument : public PadFragment
{
public:
    PadDocument(const QString &rawSource);
    PadDocument(QTextDocument *source);
    PadDocument();
    virtual ~PadDocument();

    QString rawSource() const {return _rawSource;}
    QTextDocument *outputDocument() const {return _docOutput;}

    void addFragment(PadFragment *fragment);
    QList<PadFragment*> getAllFragments() const;

	void print(int indent = 0) const;

    QString run(QMap<QString,QVariant> &tokens) const;
    void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const;

private:
	QList<PadFragment*> _fragments;
    QString _rawSource;
    mutable QTextDocument *_docSource, *_docOutput;
};

}  // PadTools


#endif  // PAD_DOCUMENT_H
