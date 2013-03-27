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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_PAD_DOCUMENT_H
#define PADTOOLS_PAD_DOCUMENT_H

#include <padtoolsplugin/pad_fragment.h>
#include <padtoolsplugin/pad_item.h>

#include <QList>
#include <QMultiMap>
#include <QVariant>
#include <QObject>
#include <QTimer>

/**
 * \file itoken.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 17 Jan 2013
*/

namespace Core {
class ITokenPool;
}

namespace PadTools {
namespace Internal {

class PadPositionTranslator
{
public:
    PadPositionTranslator();

    void clear();
    void addOutputTranslation(const int outputPos, const int length);
    void addRawTranslation(const int rawPos, const int length);
    int deltaForSourcePosition(const int rawPos);
    int rawToOutput(const int rawPos);
    int outputToRaw(const int outputPos);

    void debug();

private:
    QMultiMap<int, int> _translations;       //! Key == outputPos (start), Value == length (of translation)
};

class PadDocument : public QObject, public PadFragment
{
    Q_OBJECT
public:
    PadDocument(QTextDocument *source, QObject *parent = 0);
    PadDocument(QObject *parent = 0);
    virtual ~PadDocument();

    // PadDocument must always have an Id == -1
    int id() const {return -1;}
    void setId(int) {}

    // Manage data source
    void clear();
    void setSource(QTextDocument *source);
    void setOutput(QTextDocument *output);
    void setTokenPool(Core::ITokenPool *pool);
    QTextDocument *rawSourceDocument() const {return _docSource;}
    QTextDocument *outputDocument() const {return _docOutput;}

    // Manage children fragments
    void addChild(PadFragment *fragment);
    void removeChild(PadFragment *fragment);
    void removeAndDeleteFragment(PadFragment *fragment);
    void sortChildren();

    // Inform on analyze
    void beginRawSourceAnalyze();
    void endRawSourceAnalyze();

    // Extract text from source && output
    QString fragmentRawSource(PadFragment *fragment) const;
    QString fragmentHtmlOutput(PadFragment *fragment) const;

    // PadFragment/Cursor hunting
    PadItem *padItemForOutputPosition(int positionInOutputQTextDocument) const;
    PadItem *padItemForSourcePosition(int positionInSourceQTextDocument) const;
    PadFragment *padFragmentForSourcePosition(int rawPos) const;
    PadFragment *padFragmentForOutputPosition(int outputPos) const;
    QTextCursor rawSourceCursorForOutputPosition(int outputPos);
    PadPositionTranslator &positionTranslator() {return _posTrans;}
    void outputPosChanged(const int oldPos, const int newPos);

    // Start replacement of tokens  (raw to output)
    // These methods should never be used directly
    void run(QMap<QString,QVariant> &, PadDocument *) {Q_ASSERT(false);/* Should never be used*/}
    void toOutput(Core::ITokenPool *, PadDocument *, TokenReplacementMethod) {Q_ASSERT(false);}
    void toOutput(Core::ITokenPool *pool, TokenReplacementMethod method = ReplaceWithTokenValue);
    void run(QMap<QString,QVariant> &tokens);

    // Start token creation  (output to raw)
    void toRaw(PadDocument *doc = 0);

    // do not return children padfragment
    QList<PadFragment*> children() const {return QList<PadFragment*>();}
    QList<PadItem*> padItems() const {return _items;}

    // Debug
    void debug(int indent = 0) const;

Q_SIGNALS:
    void aboutToClear();
    void cleared();
    void padFragmentChanged(PadFragment *fragment);
    void padFragmentAboutToRemoved(PadFragment *fragment);
    void rawSourceAnalyzeStarted();
    void rawSourceAnalyseFinished();
    void beginTokenReplacement();
    void endTokenReplacement();

public Q_SLOTS:
    void softReset();
    void reset();

private:
    QList<PadItem*> _items;
    mutable QTextDocument *_docSource, *_docOutput;
    Core::ITokenPool *_tokenPool;
    QTimer *_timer;
    PadPositionTranslator _posTrans;
};

} // namespace Internal
} // namespace PadTools

#endif  // PADTOOLS_PAD_DOCUMENT_H
