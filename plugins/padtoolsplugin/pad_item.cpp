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
  \class PadTools::PadItem
  Represents a full token including:
  - a PadCore (the token name)
  - fragments of conditional texts (before and after)
*/

#include "pad_item.h"
#include "constants.h"
#include "pad_document.h"

#include <utils/log.h>

#include <QString>
#include <QTextCursor>
#include <QTextDocumentFragment>

#include <QDebug>

using namespace PadTools;

PadConditionnalSubItem::PadConditionnalSubItem(TokenCoreCondition cond, Place place, PadFragment *parent) :
    PadFragment(parent), _coreCond(cond), _place(place)
{}

void PadConditionnalSubItem::addDelimiter(const int posInRaw, const int size)
{
    Delimiter delim;
    delim.rawPos = posInRaw;
    delim.size = size;
    _delimiters << delim;
}

void PadConditionnalSubItem::run(QMap<QString,QVariant> &tokens)
{
//    return QString();
}

void PadConditionnalSubItem::debug(int indent) const
{
    QString str(indent, ' ');
    str += QString("[padSubItem:Source(%1;%2);Output(%3;%4)]")
            .arg(start()).arg(end())
            .arg(outputStart()).arg(outputEnd());
    qDebug("%s", qPrintable(str));
    foreach (PadFragment *fragment, _fragments) {
        fragment->debug(indent + 2);
    }
}

void PadConditionnalSubItem::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out)
{}

void PadConditionnalSubItem::run(QMap<QString,QVariant> &tokens, PadDocument *document)
{
//    qWarning() << "run PadConditionnalSubItem";
    // Get parent PadItem
    PadFragment *f = parent();
    PadItem *item = 0;
    while (f) {
        item = dynamic_cast<PadItem*>(f);
        if (item)
            break;
    }

    if (!item) {
        LOG_ERROR_FOR("PadConditionnalSubItem", "No PadItem parent");
        return;
    }

    const QString &value = tokens.value(item->getCore()->name()).toString();

//    const QString &value = tokens[_name].toString();
    bool removeMe = false;
    if (value.isEmpty()) {
        // Keep Type::Undefined / Before && After
        // Remove Type::Defined / Before && After
        removeMe = (_coreCond == Defined);
    } else {
        // Keep Type::Defined / Before && After
        // Remove Type::Undefined / Before && After
        removeMe = (_coreCond == Undefined);
    }

    setOutputStart(document->positionTranslator().rawToSource(start()));
    if (removeMe) {
        // Remove everything
        QTextCursor cursor(document->outputDocument());
        cursor.setPosition(outputStart());
        cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        setOutputEnd(outputStart());
        document->positionTranslator().addOutputTranslation(outputStart(), rawLength());
        return;
    } else {
//         Remove only delimiters
        foreach(const Delimiter &delim, _delimiters) {

//            qWarning() << "DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToSource(delim.rawPos));

            QTextCursor cursor(document->outputDocument());
            int pos = document->positionTranslator().rawToSource(delim.rawPos);
            cursor.setPosition(pos);
            cursor.setPosition(pos + delim.size, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            setOutputEnd(outputEnd() - delim.size);
            document->positionTranslator().addOutputTranslation(delim.rawPos, -delim.size);
        }

        // Run nested fragments
        foreach(PadFragment *frag, _fragments)
            frag->run(tokens, document);
    }
    setOutputEnd(document->positionTranslator().rawToSource(end()));
}


void PadCore::debug(int indent) const
{
    QString str(indent, ' ');
    str += QString("[padCore:Source(%1;%2);Output(%3;%4)]: %5")
            .arg(start()).arg(end())
            .arg(outputStart()).arg(outputEnd())
            .arg(_name);
    qDebug("%s", qPrintable(str));
}

void PadCore::run(QMap<QString,QVariant> &tokens)
{
//    /** \todo use this output only if HTML is requested */
//    const QString &value = tokens[_name].toString();
//    if (start() > 0 && !value.isEmpty()) {
//        return QString(Constants::TOKEN_AND_POSITION_TAG)
//                .arg(value).arg(id());
//    }
//    return value;
}

void PadCore::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *output)
{
    const QString &value = tokens[_name].toString();
    if (!value.isEmpty()) {
        // insert core value
        insertText(output, value);
        // apply charFormat from the source
        QTextCursor cursor(source);
        cursor.setPosition(_start);
        cursor.setPosition(_end, QTextCursor::KeepAnchor);
        QTextCharFormat format = cursor.charFormat();
        QTextCursor cout(output);
        cout.setPosition(_outputStart);
        cout.setPosition(_outputEnd, QTextCursor::KeepAnchor);
        cout.setCharFormat(format);
    }
}

void PadCore::run(QMap<QString,QVariant> &tokens, PadDocument *document)
{
//    qWarning() << "run Core";
    // PadItem calls run on the Core only if core value is defined
    const QString &value = tokens[_name].toString();
    if (value.isEmpty()) {
        // Define output range
        LOG_ERROR_FOR("PadCore", "token run without value? Check PadItem.");
        return;
    }
    // Compute output positions
    setOutputStart(document->positionTranslator().rawToSource(start()));
    setOutputEnd(outputStart() + value.size());

    // Replace core source
    QTextCursor cursor(document->outputDocument());
    cursor.setPosition(outputStart());
    cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
    QTextCharFormat format = cursor.charFormat();
    cursor.removeSelectedText();
    cursor.insertText(value, format);

    // Add translation to document
    int delta = outputLength() - rawLength();
    document->positionTranslator().addOutputTranslation(outputStart(), delta);
}



/**
 * \class PadTools::PadItem
 * Contains an entire pad item i.e. a list of fragments.
 */

PadItem::~PadItem()
{
}

/** Debug to console */
void PadItem::debug(int indent) const
{
	QString str(indent, ' ');
    str += QString("[padItem:Source(%1;%2);Output(%3;%4)]")
            .arg(start()).arg(end())
            .arg(outputStart()).arg(outputEnd());
	qDebug("%s", qPrintable(str));
    foreach (PadFragment *fragment, _fragments) {
        fragment->debug(indent + 2);
	}
}

/** Find nested PadItem in conditional texts (before and after) */
QList<PadFragment*> PadItem::children() const
{
	QList<PadFragment*> fragments;
	PadItem *padItem;
	fragments.append(_fragments);
    foreach (PadFragment *fragment, _fragments) {
		padItem = dynamic_cast<PadItem*>(fragment);
		if (padItem)
            fragments.append(padItem->children());
	}
	return fragments;
}

/** Find PadFragment according to its \e type. \sa PadItem::PadStringType */
PadFragment *PadItem::fragment(const int type) const
{
    foreach (PadFragment *fragment, _fragments) {
//        if (fragment->userData(Constants::USERDATA_KEY_PADITEM).toInt() == type)
//            return fragment;
    }
    return 0;
}

/** Returns the PadTools::PadCore of the PadTools::PadItem. If no core is found, 0 is returned. */
PadCore *PadItem::getCore() const
{
    PadCore *core;
    foreach (PadFragment *fragment, _fragments) {
        core = dynamic_cast<PadCore*>(fragment);
        if (core)
            return core;
    }
    return 0;
}

/** Run this pad over some tokens and returns the result text */
void PadItem::run(QMap<QString,QVariant> &tokens)
{
//	QString value;
//	PadCore *core = getCore();
//	QString coreValue;

//	// if a core exists, the entire pad expression is optional, depending on the core emptiness
//    if (core) {
//        coreValue = tokens.value(core->name());
//		if (coreValue.isEmpty()) // core empty? so the entire pad will be empty too
//			return "";
//	}

//	foreach (PadFragment *fragment, _fragments)
//		value += fragment->run(tokens);

//	return value;
}

void PadItem::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out)
{
    PadCore *core = getCore();
    QString coreValue;

    // if a core exists, the entire pad expression is optional, depending on the core emptiness
    if (core) {
        coreValue = tokens.value(core->name()).toString();
        if (coreValue.isEmpty())
            return;
    }

    int start = -1;
    int end = -1;
    foreach(PadFragment *fragment, _fragments) {
        fragment->run(tokens, source, out);
        if (fragment->outputStart() < start || start == -1)
            start = fragment->outputStart();
        if (fragment->outputEnd() > end)
            end = fragment->outputEnd();
    }

    // Trace position in output
    _outputStart = start;
    _outputEnd = end;

    // Add anchors to the output QTextDocument
    QTextCursor cursor(out);
    cursor.setPosition(_outputStart);
    cursor.setPosition(_outputEnd, QTextCursor::KeepAnchor);
    QTextCharFormat f;
    f.setAnchor(true);
    f.setAnchorNames(QStringList() << Constants::ANCHOR_ITEM << QString::number(id()));
    cursor.mergeCharFormat(f);

    // Add the format && tooltip
    int count = _outputEnd - _outputStart;
    for(int i=0; i < count; ++i) {
        cursor.setPosition(start + i);
        cursor.setPosition(start + i + 1, QTextCursor::KeepAnchor);
        QTextCharFormat format;
        format.setToolTip("Token: " + coreValue);
        cursor.mergeCharFormat(format);
    }
}

void PadItem::run(QMap<QString,QVariant> &tokens, PadDocument *document)
{
//    qWarning() << "run Item";
    PadCore *core = getCore();
    QString coreValue;

    // if core value is null -> remove it from the output
    if (core) {
        coreValue = tokens.value(core->name()).toString();


        if (coreValue.isEmpty()) {
            // No value -> Remove the entire PadItem from the text output and add a translation
            QTextCursor cursor(document->outputDocument());
            setOutputStart(document->positionTranslator().rawToSource(start()));
            cursor.setPosition(outputStart());
            cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            setOutputEnd(outputStart());
            document->positionTranslator().addOutputTranslation(outputStart(), rawLength());
        } else {
            // Value -> run fragments
            foreach(PadFragment *f, _fragments) {
                f->run(tokens, document);
            }
            // Recompute output positions

//            qWarning() << "PadItem--> " << start() << document->positionTranslator().rawToSource(start());

            setOutputStart(document->positionTranslator().rawToSource(start()));
            setOutputEnd(document->positionTranslator().rawToSource(end()));
        }
    }
}
