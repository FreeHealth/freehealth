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
#include <QObject>
#include <QTimer>

#include "pad_fragment.h"
#include "pad_core.h"
#include "pad_item.h"

namespace PadTools {
class TokenModel;

class PadDocument : public QObject, public PadFragment
{
    Q_OBJECT
public:
    PadDocument(const QString &rawSource);
    PadDocument(QTextDocument *source);
    PadDocument();
    virtual ~PadDocument();

    int id() const {return -1;}
    void setId(int) {}

    void clear();
    void setSource(const QString &rawSource);
    void setSource(QTextDocument *source);
    void setTokenModel(TokenModel *model);

    QString rawSource() const {return _rawSource;}
    QTextDocument *rawSourceDocument() const {return _docSource;}
    QTextDocument *outputDocument() const {return _docOutput;}

    QString fragmentRawSource(PadFragment *fragment) const;
    QString fragmentHtmlOutput(PadFragment *fragment) const;

    void addChild(PadFragment *fragment);

    PadItem *padItemForOutputPosition(int positionInOutputQTextDocument) const;
    PadItem *padItemForSourcePosition(int positionInSourceQTextDocument) const;
    PadFragment *padFragmentForSourcePosition(int p) const;
    PadFragment *padFragmentForOutputPosition(int p) const;

	void print(int indent = 0) const;

    QString run(QMap<QString,QVariant> &tokens) const;
    void run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const;

    // do not return children padfragment
    virtual QList<PadFragment*> children() const {return QList<PadFragment*>();}

Q_SIGNALS:
    void cleared();
    void padFragmentChanged(PadFragment *fragment);

private:
//    void createTimerForDelayedResetOnRawSourceChanged();

private Q_SLOTS:
    void reset();
    void rawSourceContentsChanged(int from, int charsRemoves, int charsAdded);

private:
    QList<PadItem*> _items;
    QString _rawSource;
    mutable QTextDocument *_docSource, *_docOutput;
    TokenModel *_tokenModel;
    QTimer *_timer;
};

}  // PadTools


#endif  // PAD_DOCUMENT_H
