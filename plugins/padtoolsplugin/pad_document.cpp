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

#include <QString>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include <QDebug>

using namespace PadTools;

/** \todo documentation */
PadPositionTranslator::PadPositionTranslator()
{
}

void PadPositionTranslator::clear()
{
    _translations.clear();
}

void PadPositionTranslator::addOutputTranslation(const int pos, const int length)
{
    _translations.insert(pos, length);
}

//void PadPositionTranslator::addTranslation(const int rawPos, const int outputPos)
//{
//}

int PadPositionTranslator::deltaForSourcePosition(const int pos)
{
    int delta = 0;
    QMapIterator<int, int> i(_translations);
    while (i.hasNext()) {
        i.next();
        if (i.key() < pos)
            delta += i.value();
    }
    return delta;
}

int PadPositionTranslator::rawToSource(const int rawPos)
{
    int pos = rawPos + deltaForSourcePosition(rawPos);
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
    _docOutput(0),
    _tokenModel(0),
    _timer(0)
{
//    connect(_docSource, SIGNAL(contentsChange(int,int,int)), this, SLOT(rawSourceContentsChanged(int,int,int)));
}

/**
 * Construct an empty PadTools::PadDocument.
 * All PadTools::PadDocument must not have any id set. It should always be equal to '-1'.
 */
PadDocument::PadDocument(QObject *parent) :
    QObject(parent),
    _docSource(0),
    _docOutput(0),
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
    qDeleteAll(_fragments);
    _fragments.clear();
//    if (_rawSource.isEmpty()) {
////        disconnect(_docSource, SIGNAL(contentsChange(int,int,int)), this, SLOT(rawSourceContentsChanged(int,int,int)));
//        _docSource = 0;
//    }
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
//    connect(_docSource, SIGNAL(contentsChange(int,int,int)), this, SLOT(rawSourceContentsChanged(int,int,int)));
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

/** Add string fragment to the object. */
void PadDocument::addChild(PadFragment *fragment)
{
    PadItem *item = dynamic_cast<PadItem *>(fragment);
    if (item)
        _items << item;
    PadFragment::addChild(fragment);
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
    PadFragment::removeAndDeleteFragment(fragment);
}

///** Removes char at \e position in output and keep raw source sync */
//void PadDocument::removeOutputCharAt(int position, int length)
//{
//}

///** Inserts char at \e position in output and keep raw source sync */
//void PadDocument::insertOutputCharAt(const QChar &c,int position)
//{
//}

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
    PadItem *item = dynamic_cast<PadItem*>(fragment);
    PadFragment *parent = fragment;
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

/** Find fragment for the QTextCursor position in raw source document. */
PadFragment *PadDocument::padFragmentForSourcePosition(int p) const
{
    foreach(PadFragment *fragment, _fragments) {
        if (fragment->start() <= p && fragment->end() >= p)
            return fragment->padFragmentForSourcePosition(p);
    }
    return 0;
}

/** Find fragment for the QTextCursor position in output document. */
PadFragment *PadDocument::padFragmentForOutputPosition(int p) const
{
    foreach(PadFragment *fragment, _fragments) {
        if (fragment->outputStart() <= p && fragment->outputEnd() >= p)
            return fragment->padFragmentForOutputPosition(p);
    }
    return 0;
}

/** Returns the QTextCursor in the rawSource corresponding to the position \e p in the output document. */
QTextCursor PadDocument::rawSourceCursorForOutputPosition(int p) const
{
    Q_ASSERT(_docSource);
    Q_ASSERT(_docOutput);

    PadFragment *outFragment = padFragmentForOutputPosition(p);
    if (!outFragment) {
        // return start or end
        QTextCursor cursor(_docSource);
        QTextCursor out(_docOutput);
        out.setPosition(p);
        out.atEnd() ? cursor.movePosition(QTextCursor::End) : cursor.movePosition(QTextCursor::Start);
        return cursor;
    }

    int posInFragment = p - outFragment->outputStart();
    PadFragment *sourceFragment = padFragmentForSourcePosition(outFragment->start());
    int posInRaw = sourceFragment->start() + posInFragment;
    if (!sourceFragment) {
        // return start or end
        QTextCursor cursor(_docSource);
        QTextCursor out(_docOutput);
        out.setPosition(p);
        if (out.atEnd())
            cursor.movePosition(QTextCursor::End);
        else
            cursor.movePosition(QTextCursor::Start);
        return cursor;
    }

    QTextCursor cursor(_docSource);
    cursor.setPosition(posInRaw);
    return cursor;
}

///** Run this pad over some tokens and returns the result text */
//QString PadDocument::run(QMap<QString,QVariant> &tokens) const
//{
//    /** \todo use QtConcurrent to run this part into a specific thread */
//    /** \todo transform _rawSource into a QTextDocument and run tokens on it */
////    QTextDocument *source = 0;
////    if (!_rawSource.isEmpty()) {
////        // Run throught raw string source
////        source = new QTextDocument(this);
////        source->setPlainText(_rawSource);
////    } else {
////        source = _docSource;
////    }

//    QString value;
//    foreach (PadFragment *fragment, _fragments)
//		value += fragment->run(tokens);
//	return value;
//}

/** Run this pad over some tokens and set the result to the \e out QTextDocument */
void PadDocument::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out)
{
    foreach (PadFragment *fragment, _fragments)
        fragment->run(tokens, source, out);
    _docOutput = out;

    debug();
}

static void syncRange(PadFragment *f)
{
    f->setOutputStart(f->start());
    f->setOutputEnd(f->end());
    foreach(PadFragment *frag, f->children())
        syncRange(frag);
}

void PadDocument::run(QMap<QString,QVariant> &tokens)
{
//    if (_docOutput && _docOutput->parent() == this)
//        delete _docOutput;
    _docOutput = _docSource->clone(this);

    // sync raw && output ranges of all fragments
    foreach (PadFragment *fragment, _fragments)
        syncRange(fragment);

    // run tokens on fragments
    foreach (PadFragment *fragment, _fragments)
        fragment->run(tokens, this);

    positionTranslator().debug();
    debug();
}

/** Clear the PadDocument without deleting sources. */
void PadDocument::softReset()
{
    QTime c;
    c.start();

    qDeleteAll(_fragments);
    _fragments.clear();
    _docOutput->clear();

    PadAnalyzer a;
    a.analyze(_docSource, this);
    if (_tokenModel)
        run(_tokenModel->tokens(), _docSource, _docOutput);

    Utils::Log::logTimeElapsed(c, "PadDocument", "reset");
}

/** Renew the analyze of the source and update the output. Clears the object first \sa clear(), softReset() */
void PadDocument::reset()
{
    clear();
    softReset();
    return;
}
