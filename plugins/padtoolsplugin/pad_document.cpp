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
 * \class PadTools::PadDocument
 * Contains all the PadTools::PadFragment of the analyzed \e rawSource.
 */

/**
 * \fn void padFragmentChanged(PadFragment *fragment)
 * Signal emitted when a PadTools::PadFragment ranges or position was internally changed.
 */

#include "pad_document.h"
#include "pad_item.h"
#include "pad_string.h"
#include "pad_analyzer.h"
#include "tokenmodel.h"

#include <utils/log.h>
#include <utils/global.h>

#include <QString>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include <QDebug>

using namespace PadTools;
using namespace Internal;

// TODO: documentation */
PadPositionTranslator::PadPositionTranslator()
{
}

void PadPositionTranslator::clear()
{
    _translations.clear();
}

void PadPositionTranslator::addOutputTranslation(const int outputPos, const int length)
{
    _translations.insertMulti(outputPos, length);
}

void PadPositionTranslator::addRawTranslation(const int rawPos, const int length)
{
    int output = rawToOutput(rawPos);
    addOutputTranslation(output, -length);
}

int PadPositionTranslator::deltaForSourcePosition(const int outputPos)
{
    int delta = 0;
    foreach(int begin, _translations.uniqueKeys()) {
        // translation is after the required position
        if (begin > outputPos)
            break;

        foreach(int size, _translations.values(begin)) {
            // if rawPos inside the delta
            int end = begin + size;
            if ((outputPos >= begin) && (outputPos <= end)) {
                delta += outputPos - begin;
                continue;
            }
            if (begin <= outputPos)
                delta += size;
        }
    }
    return delta;
}

int PadPositionTranslator::rawToOutput(const int rawPos)
{
    int pos = rawPos;
    foreach(int begin, _translations.uniqueKeys()) {
        if (pos > begin) {
            foreach(int size, _translations.values(begin)) {
                if (pos + size < begin)
                    pos = begin;
                else
                    pos += size;
            }
        }
    }
    return pos>0 ? pos : 0;
}

int PadPositionTranslator::outputToRaw(const int outputPos)
{
    int pos = outputPos - deltaForSourcePosition(outputPos);
    return pos>0 ? pos : 0;
}

void PadPositionTranslator::debug()
{
    qWarning() << "Translations" << _translations;
}

/**
 * Construct an empty PadTools::PadDocument with the \e source QTextDocument (document is not analyzed).
 * All PadTools::PadDocument must not have any id set. It should always be equal to '-1'.
 */
PadDocument::PadDocument(QTextDocument *source, QObject *parent) :
    QObject(parent),
    _docSource(source),
    _docOutput(new QTextDocument(this)),
    _tokenModel(0),
    _timer(0)
{
}

/**
 * Construct an empty PadTools::PadDocument.
 * All PadTools::PadDocument must not have any id set. It should always be equal to '-1'.
 */
PadDocument::PadDocument(QObject *parent) :
    QObject(parent),
    _docSource(0),
    _docOutput(new QTextDocument(this)),
    _tokenModel(0),
    _timer(0)
{
}

/** Destructor, deletes all included PadTools::PadFragment objects */
PadDocument::~PadDocument()
{
}

/** Clear the current PadTools::PadDocument. Removes all fragments && sources. */
void PadDocument::clear()
{
    Q_EMIT aboutToClear();
    qDeleteAll(_fragments);
    _fragments.clear();
    _items.clear();
    if (_docOutput)
        _docOutput->clear();
    _posTrans.clear();
    Q_EMIT cleared();
}

/** Defines the source to use as a QTextDocument \e source */
void PadDocument::setSource(QTextDocument *source)
{
    clear();
    _docSource = source;
}

/** Set the PadTools::TokenModel to use in this object. The model is mainly used by reset(). */
void PadDocument::setTokenModel(TokenModel *model)
{
    _tokenModel = model;
}

/** Returns the raw content of a fragment (extracted from the raw source). */
QString PadDocument::fragmentRawSource(PadFragment *fragment) const
{
    if (!fragment)
        return QString::null;
    if (_docSource) {
        return _docSource->toPlainText().mid(fragment->start(), fragment->end() - fragment->start());
    }
    return QString::null;
}

/** Returns the html output content of a fragment (extracted from the output document). */
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

/** Add fragments to the object. Manages the padItems() list too. */
void PadDocument::addChild(PadFragment *fragment)
{
    PadItem *item = dynamic_cast<PadItem *>(fragment);
    if (item)
        _items << item;
    PadFragment::addChild(fragment);
}

/** Removes fragments to the object. Manages the padItems() list too. */
void PadDocument::removeChild(PadFragment *fragment)
{
    PadItem *item = dynamic_cast<PadItem *>(fragment);
    if (item)
        _items.removeAll(item);
    PadFragment::removeChild(fragment);
}

/** Remove \e fragment content in raw source && in output and delete the fragment */
void PadDocument::removeAndDeleteFragment(PadFragment *fragment)
{
    if (_docSource) {
        QTextCursor cursor(_docSource);
        cursor.setPosition(fragment->start());
        cursor.setPosition(fragment->end(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    if (_docOutput) {
        QTextCursor cursor(_docOutput);
        cursor.setPosition(fragment->outputStart());
        cursor.setPosition(fragment->outputEnd(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    PadItem *item = dynamic_cast<PadItem*>(fragment);
    if (item)
        _items.removeAll(item);
    PadFragment::removeAndDeleteFragment(fragment);
}

void PadDocument::sortChildren()
{
    PadFragment::sortChildren();
    qSort(_items);
}

void PadDocument::beginRawSourceAnalyze()
{
    Q_EMIT rawSourceAnalyzeStarted();
}

void PadDocument::endRawSourceAnalyze()
{
    Q_EMIT rawSourceAnalyseFinished();
}

/** Debug to console. */
void PadDocument::debug(int indent) const
{
	QString str(indent, ' ');
	str += "[pad]";
	qDebug("%s", qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
        fragment->debug(indent + 2);
	}
}

/** Find the PadTools::PadItem for the QTextCursor position in output document. */
PadItem *PadDocument::padItemForOutputPosition(int p) const
{
    PadFragment *fragment = padFragmentForOutputPosition(p);
    PadFragment *parent = fragment;
    PadItem *item = dynamic_cast<PadItem*>(fragment);
    while (parent && !item) {
        parent = parent->parent();
        item = dynamic_cast<PadItem*>(parent);
    }
    return item;
}

/** Find the PadTools::PadItem for the QTextCursor position in raw source document. */
PadItem *PadDocument::padItemForSourcePosition(int p) const
{
    PadFragment *fragment = padFragmentForSourcePosition(p);
    PadItem *item = dynamic_cast<PadItem*>(fragment);
    PadFragment *parent = fragment;
    while (parent && !item) {
        parent = parent->parent();
        item = dynamic_cast<PadItem*>(parent);
    }
    return item;
}

/** Find fragment for the QTextCursor position in raw source document. Return 0 if no fragment matches. */
PadFragment *PadDocument::padFragmentForSourcePosition(int rawPos) const
{
    if (_fragments.isEmpty()) {
//        if (_start < rawPos && rawPos < _end)
        if (IN_RANGE_STRICTLY(rawPos, _start, _end))
            return (PadFragment *)this;
    } else {
        foreach(PadFragment *fragment, _fragments) {
            if (fragment->start() < rawPos && rawPos < fragment->end())
                return fragment->padFragmentForSourcePosition(rawPos);
        }
    }
    return 0;
}

/** Find fragment for the QTextCursor position in output document. Return 0 if no fragment matches. */
PadFragment *PadDocument::padFragmentForOutputPosition(int outputPos) const
{
    if (_fragments.isEmpty()) {
        if (IN_RANGE_STRICTLY(outputPos, _outputStart, _outputEnd)) // _outputStart < outputPos && outputPos < _outputEnd)
            return (PadFragment *)this;
    } else {
        foreach(PadFragment *fragment, _fragments) {
            //            if (fragment->outputStart() < outputPos && fragment->outputEnd() > outputPos)
            if (IN_RANGE_STRICTLY(outputPos, fragment->outputStart(), fragment->outputEnd()))
                return fragment->padFragmentForOutputPosition(outputPos);
        }
    }
    return 0;
}

/**
 * Returns the QTextCursor in the rawSource corresponding to the position \e p in the output document.
 * \sa PadTools::PadDocument::positionTranslator(), PadTools::PadPositionTranslator
 */
QTextCursor PadDocument::rawSourceCursorForOutputPosition(int outputPos)
{
    Q_ASSERT(_docSource);
    Q_ASSERT(_docOutput);

    QTextCursor cursor(_docSource);
    cursor.setPosition(positionTranslator().outputToRaw(outputPos));
    return cursor;
}

void PadDocument::outputPosChanged(const int oldPos, const int newPos)
{
    // manage positionning of fragment
    foreach(PadItem *item, _items)
        item->outputPosChanged(oldPos, newPos);

    // manage deletion of fragments if needed
    foreach(PadFragment *f, _fragmentsToDelete) {
        if (f->parent())
            f->parent()->removeChild(f);
    }
    qDeleteAll(_fragmentsToDelete);
    _fragmentsToDelete.clear();
}

static void syncOutputRange(PadFragment *f)
{
    f->setOutputStart(f->start());
    f->setOutputEnd(f->end());
    foreach(PadFragment *frag, f->children())
        syncOutputRange(frag);
}

/** Starts the replacement of tokens. This function will recreate the output QTextDocument. This one will be cleared. */
void PadDocument::run(QMap<QString,QVariant> &tokens)
{
    Q_ASSERT(_docSource);
    if (!_docSource)
        return;
    Q_EMIT beginTokenReplacement();

    if (!_docOutput) {
        _docOutput = new QTextDocument(this);
    }
    _docOutput->clear();
    _docOutput->setHtml(_docSource->toHtml());

    // sync raw && output ranges of all fragments
    foreach (PadFragment *fragment, _fragments)
        syncOutputRange(fragment);

    // run tokens on fragments
    foreach (PadFragment *fragment, _fragments)
        fragment->run(tokens, this);

    //    positionTranslator().debug();
//    debug();

    // emit end signal
    Q_EMIT endTokenReplacement();
}

/** Starts the replacement of tokens. This function will recreate the output QTextDocument. This one will be cleared. */
void PadDocument::toOutput(Core::ITokenPool *pool, TokenReplacementMethod method)
{
    Q_ASSERT(_docSource);
    if (!_docSource)
        return;
    Q_EMIT beginTokenReplacement();

    if (!_docOutput) {
        _docOutput = new QTextDocument(this);
    }
    _docOutput->clear();
    _docOutput->setHtml(_docSource->toHtml());

    // sync raw && output ranges of all fragments
    foreach (PadFragment *fragment, _fragments)
        syncOutputRange(fragment);

    // run tokens on fragments
    foreach (PadFragment *fragment, _fragments)
        fragment->toOutput(pool, this, method);

    //    positionTranslator().debug();
//    debug();

    // emit end signal
    Q_EMIT endTokenReplacement();
}

static void syncRawRange(PadFragment *f)
{
    f->setStart(f->outputStart());
    f->setEnd(f->outputEnd());
    foreach(PadFragment *frag, f->children())
        syncRawRange(frag);
}

void PadDocument::toRaw(PadDocument *doc)
{
    // Doc must be null cause we are using the current object
    Q_ASSERT(!doc);
    if (doc)
        return;
    Q_ASSERT(_docOutput);
    if (!_docOutput)
        return;
    Q_ASSERT(_docSource);
    if (!_docSource)
        return;

    _docSource->clear();
    _posTrans.clear();
    _docSource->setHtml(_docOutput->toHtml());

    // sync raw && output ranges of all fragments
    foreach (PadFragment *fragment, _fragments)
        syncRawRange(fragment);

    // run tokens on fragments
    foreach (PadFragment *fragment, _fragments)
        fragment->toRaw(this);
}

/** Clear the PadDocument without deleting sources. */
void PadDocument::softReset()
{
    QTime c;
    c.start();

//    clear();  // Don't clean cause it deletes some pointers that we need
    qDeleteAll(_fragments);
    _fragments.clear();
    _items.clear();
    _docOutput->clear();

    PadAnalyzer a;
    a.analyze(_docSource, this);
    // TODO: use ITokenPool */
    if (_tokenModel)
        toOutput(_tokenModel->tokenPool());
//        run(_tokenModel->tokens());//, _docSource, _docOutput);

    Utils::Log::logTimeElapsed(c, "PadTools::PadDocument", "reset");
}

/** Renew the analyze of the source and update the output. Clears the object first \sa clear(), softReset() */
void PadDocument::reset()
{
    clear();
    softReset();
    return;
}
