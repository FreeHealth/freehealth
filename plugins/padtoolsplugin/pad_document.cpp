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

/**
 * Construct an empty PadTools::PadDocument with the \e rawSource text (text is not analyzed).
 * All PadTools::PadDocument must not have any id set. It should always be equal to '-1'.
 */
PadDocument::PadDocument(const QString &rawSource) :
    _rawSource(rawSource),
    _docSource(0),
    _docOutput(0),
    _tokenModel(0),
    _timer(0)
{
}

/**
 * Construct an empty PadTools::PadDocument with the \e source QTextDocument (document is not analyzed).
 * All PadTools::PadDocument must not have any id set. It should always be equal to '-1'.
 */
PadDocument::PadDocument(QTextDocument *source) :
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
PadDocument::PadDocument() :
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

/** Clear the current PadTools::PadDocument. */
void PadDocument::clear()
{
    qDeleteAll(_fragments);
    _fragments.clear();
    if (_rawSource.isEmpty()) {
//        disconnect(_docSource, SIGNAL(contentsChange(int,int,int)), this, SLOT(rawSourceContentsChanged(int,int,int)));
        _docSource = 0;
    }
    if (_docOutput)
        _docOutput->clear();
    Q_EMIT cleared();
}

/** Defines the source to use as a QString \e rawSource */
void PadDocument::setSource(const QString &rawSource)
{
    clear();
    _rawSource = rawSource;
    _docSource = 0;
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
    if (!_rawSource.isEmpty()) {
        return _rawSource.mid(fragment->start(), fragment->end() - fragment->start());
    }
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

/** Debug to console. */
void PadDocument::print(int indent) const
{
	QString str(indent, ' ');
	str += "[pad]";
	qDebug("%s", qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
		fragment->print(indent + 2);
	}
}

/** Find the PadTools::PadItem for the QTextCursor position in output document. */
PadItem *PadDocument::padItemForOutputPosition(int p) const
{
    foreach(PadItem *item, _items) {
        if (item->outputStart() <= p && item->outputEnd() >= p)
            return item;
    }
    return 0;
}

/** Find the PadTools::PadItem for the QTextCursor position in raw source document. */
PadItem *PadDocument::padItemForSourcePosition(int p) const
{
    foreach(PadItem *item, _items) {
        if (item->start() <= p && item->end() >= p)
            return item;
    }
    return 0;
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

/** Run this pad over some tokens and returns the result text */
QString PadDocument::run(QMap<QString,QVariant> &tokens) const
{
    /** \todo use QtConcurrent to run this part into a specific thread */
	QString value;
	foreach (PadFragment *fragment, _fragments)
		value += fragment->run(tokens);
	return value;
}

/** Run this pad over some tokens and set the result to the \e out QTextDocument */
void PadDocument::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out) const
{
    foreach (PadFragment *fragment, _fragments)
        fragment->run(tokens, source, out);
    _docOutput = out;
}

//void PadDocument::createTimerForDelayedResetOnRawSourceChanged()
//{
//    if (_timer)
//        return;
//    _timer = new QTimer(this);
//    _timer->setSingleShot(true);
//    _timer->stop();
//    connect(_timer, SIGNAL(timeout()), this, SLOT(reset()));
//}

/** Renew the analyze of the source and update the output */
void PadDocument::reset()
{
    QTime c;
    c.start();

    clear();
    PadAnalyzer a;
    a.analyze(_docSource, this);
    if (_tokenModel)
        run(_tokenModel->tokens(), _docSource, _docOutput);

    Utils::Log::logTimeElapsed(c, "PadDocument", "reset");
    return;
}
