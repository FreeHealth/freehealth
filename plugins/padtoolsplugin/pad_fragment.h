/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_PAD_FRAGMENT_H
#define PADTOOLS_PAD_FRAGMENT_H

#include <coreplugin/ipadtools.h>

#include <QString>
#include <QHash>
#include <QVariant>
#include <QTextDocument>

/**
 * \file pad_fragment.h
 * \author Eric Maeker, Guillaume Denry
 * \version 0.8.2
 * \date 25 Dec 2012
*/

namespace PadTools {
namespace Internal {
class PadDocument;

class PadFragment
{
public:
    enum TokenReplacementMethod {
        ReplaceWithTokenDisplayName = 0,
        ReplaceWithTokenTestingValue,
        ReplaceWithTokenValue,
        ReplaceWithTokenUuid
    };

    PadFragment(PadFragment *parent = 0);
    virtual ~PadFragment();

    void clear();

    virtual void setParent(PadFragment *parent) {_parent = parent;}
    virtual PadFragment *parent() const {return _parent;}

    virtual void debug(int indent = 0) const = 0;

    virtual int id() const { return _id; }
    virtual void setId(int id) { _id = id; }

	int start() const { return _start; }
    void setStart(int start) { _start = start; }
    int end() const { return _end; }
    void setEnd(int end) { _end = end; }

    int rawLength() const {return _end - _start;}
    int outputLength() const {return _outputEnd - _outputStart;}

    void translateOutput(int nbChars);
    void moveOutputEnd(int nbOfChars);

    void setOutputStart(const int pos) {_outputStart = pos;}
    void setOutputEnd(const int pos) {_outputEnd = pos;}
    int outputStart() const {return _outputStart;}
    int outputEnd() const {return _outputEnd;}
    void resetOutputRange() {_outputStart=-1; _outputEnd=-1; foreach(PadFragment*f,_fragments) f->resetOutputRange();}

    virtual bool containsRawPosition(const int pos) const;
    virtual bool containsOutputPosition(const int pos) const;
    virtual bool isBeforeOutputPosition(const int pos) const;
    virtual bool isAfterOutputPosition(const int pos) const;

    void setToolTip(const QString &tooltip) {_toolTip = tooltip;}
    const QString &toolTip() const {return _toolTip;}

    virtual void run(QMap<QString,QVariant> &tokens, PadDocument *document) = 0;
    virtual void toOutput(Core::ITokenPool *pool, PadDocument *document, TokenReplacementMethod method = ReplaceWithTokenValue) = 0;
    virtual void toRaw(PadDocument *doc) = 0;

    virtual void addChild(PadFragment *fragment);
    virtual void removeChild(PadFragment *fragment);
    virtual void removeAndDeleteFragment(PadFragment *fragment);
    virtual QList<PadFragment*> children() const {return _fragments;}
    virtual void sortChildren();

    virtual PadFragment *padFragmentForSourcePosition(int pos) const;
    virtual PadFragment *padFragmentForOutputPosition(int pos) const;

    virtual void outputPosChanged(const int oldPos, const int newPos);

    static bool lessThan(PadFragment *first, PadFragment *second);

protected:
    QList<PadFragment *> _fragments;                    //! List of children */
    static QList<PadFragment *> _fragmentsToDelete;     //! List of fragment to delete after the PadTools::PadFragment::outputPosChanged() is called */

    int _start;                 //! Index of the first char of the fragment in the source document */
    int _end;                   //! Index of the last char of the fragment in the source document */
    mutable int _outputStart;   //! Index of the first char of the fragment in the output document */
    mutable int _outputEnd;     //! Index of the last char of the fragment in the output document */

private:
    PadFragment *_parent;
    long long _id; // unique identifier
    QString _toolTip;
};

} // namespace Internal
} // namespace PadTools

#endif  // PADTOOLS_PAD_FRAGMENT
