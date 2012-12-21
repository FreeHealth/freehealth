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
#include <utils/global.h>

#include <QString>
#include <QTextCursor>
#include <QTextDocumentFragment>

#include <QDebug>

using namespace PadTools;
using namespace Internal;

PadConditionnalSubItem::PadConditionnalSubItem(TokenCoreCondition cond, Place place, PadFragment *parent) :
    PadFragment(parent), _coreCond(cond), _place(place)
{}

void PadConditionnalSubItem::addDelimiter(const int posInRaw, const int size)
{
    PadDelimiter delim;
    delim.rawPos = posInRaw;
    delim.size = size;
    _delimiters << delim;
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

    setOutputStart(document->positionTranslator().rawToOutput(start()));
    if (removeMe) {
        // Remove everything
        QTextCursor cursor(document->outputDocument());
        cursor.setPosition(outputStart());
        cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        setOutputEnd(outputStart());
        document->positionTranslator().addOutputTranslation(outputStart(), -rawLength());
        return;
    } else {
//         Remove only delimiters
        foreach(const PadDelimiter &delim, _delimiters) {

//            qWarning() << "SUBITEM DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToOutput(delim.rawPos));

            QTextCursor cursor(document->outputDocument());
            int pos = document->positionTranslator().rawToOutput(delim.rawPos);
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
    setOutputEnd(document->positionTranslator().rawToOutput(end()));
}

void PadConditionnalSubItem::toOutput(Core::ITokenPool *pool, PadDocument *document)
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

//    const QString &value = tokens.value(item->getCore()->name()).toString();
    const QString &value = pool->tokenCurrentValue(item->getCore()->name()).toString();

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

    setOutputStart(document->positionTranslator().rawToOutput(start()));
    if (removeMe) {
        // Remove everything
        QTextCursor cursor(document->outputDocument());
        cursor.setPosition(outputStart());
        cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        setOutputEnd(outputStart());
        document->positionTranslator().addOutputTranslation(outputStart(), -rawLength());
        return;
    } else {
//         Remove only delimiters
        foreach(const PadDelimiter &delim, _delimiters) {

//            qWarning() << "SUBITEM DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToOutput(delim.rawPos));

            QTextCursor cursor(document->outputDocument());
            int pos = document->positionTranslator().rawToOutput(delim.rawPos);
            cursor.setPosition(pos);
            cursor.setPosition(pos + delim.size, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            setOutputEnd(outputEnd() - delim.size);
            document->positionTranslator().addOutputTranslation(delim.rawPos, -delim.size);
        }

        // Run nested fragments
        foreach(PadFragment *frag, _fragments)
            frag->toOutput(pool, document);
    }
    setOutputEnd(document->positionTranslator().rawToOutput(end()));
}

/** Overwrite the position tester, by default a PadTools::PadConditionnalSubItem does not include the outputStart() and outputEnd() position. */
bool PadConditionnalSubItem::containsOutputPosition(const int pos) const
{
    return IN_RANGE_STRICTLY(pos, _outputStart, _outputEnd);
}

/** Overwrite the position tester, by default a PadTools::PadConditionnalSubItem does not include the outputStart() and outputEnd() position. */
bool PadConditionnalSubItem::isBeforeOutputPosition(const int pos) const
{
    return pos >= _outputEnd;
}

/** Overwrite the position tester, by default a PadTools::PadConditionnalSubItem does not include the outputStart() and outputEnd() position. */
bool PadConditionnalSubItem::isAfterOutputPosition(const int pos) const
{
    return pos <= _outputStart;
}


void PadConditionnalSubItem::toRaw(PadDocument *doc)
{
    // TODO: re-compute raw positionning */
    Q_UNUSED(doc);
}

/** Overwrite the position tester, by default a PadTools::PadCore does not include the outputStart() and outputEnd() position. */
bool PadCore::containsOutputPosition(const int pos) const
{
    return IN_RANGE_STRICTLY(pos, _outputStart, _outputEnd);
}

/** Overwrite the position tester, by default a PadTools::PadCore does not include the outputStart() and outputEnd() position. */
bool PadCore::isBeforeOutputPosition(const int pos) const
{
    return pos >= _outputEnd;
}

/** Overwrite the position tester, by default a PadTools::PadCore does not include the outputStart() and outputEnd() position. */
bool PadCore::isAfterOutputPosition(const int pos) const
{
    return pos <= _outputStart;
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
    setOutputStart(document->positionTranslator().rawToOutput(start()));

    // Replace core source
    QTextCursor cursor(document->outputDocument());
    cursor.setPosition(outputStart());
    cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
    QTextCharFormat format = cursor.charFormat();
    cursor.removeSelectedText();
    if (Qt::mightBeRichText(value)) {
        cursor.insertHtml(value);
        setOutputEnd(cursor.selectionEnd());
        cursor.setPosition(outputStart());
        cursor.setPosition(outputEnd(), QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(format);
    } else {
        cursor.insertText(value, format);
        setOutputEnd(outputStart() + value.size());
    }

    // Add translation to document
    int delta = outputLength() - rawLength();
    document->positionTranslator().addOutputTranslation(outputStart(), delta);
}

void PadCore::toOutput(Core::ITokenPool *pool, PadDocument *document)
{
//    qWarning() << "run Core";
    // PadItem calls run on the Core only if core value is defined
//    const QString &value = tokens[_name].toString();
    const QString &value = pool->tokenCurrentValue(_name).toString();
    if (value.isEmpty()) {
        // Define output range
        LOG_ERROR_FOR("PadCore", "token run without value? Check PadItem.");
        return;
    }
    // Compute output positions
    setOutputStart(document->positionTranslator().rawToOutput(start()));

    // Replace core source
    QTextCursor cursor(document->outputDocument());
    cursor.setPosition(outputStart());
    cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
    QTextCharFormat format = cursor.charFormat();
    cursor.removeSelectedText();
    if (Qt::mightBeRichText(value)) {
        cursor.insertHtml(value);
        setOutputEnd(cursor.selectionEnd());
        cursor.setPosition(outputStart());
        cursor.setPosition(outputEnd(), QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(format);
    } else {
        cursor.insertText(value, format);
        setOutputEnd(outputStart() + value.size());
    }

    // Add translation to document
    int delta = outputLength() - rawLength();
    document->positionTranslator().addOutputTranslation(outputStart(), delta);
}

void PadCore::toRaw(PadDocument *doc)
{
//    qWarning() << "core start";
//    debug(2);
    QTextCursor raw(doc->rawSourceDocument());
    int oldLength = _end - _start;

    // recompute raw positions
    setStart(doc->positionTranslator().outputToRaw(_outputStart));
    setEnd(doc->positionTranslator().outputToRaw(_outputEnd));
    int s = QString(Constants::TOKEN_CORE_DELIMITER).size();

    // replace token content to its name
    raw.setPosition(_start);
    raw.setPosition(_end, QTextCursor::KeepAnchor);
    raw.removeSelectedText();
    raw.insertText(_name);
    int newLength = _name.size();
    doc->positionTranslator().addRawTranslation(_start, newLength-oldLength);
    doc->positionTranslator().addRawTranslation(_start, s);

    // add delimiters
    raw.setPosition(_start);
    raw.insertText(Constants::TOKEN_CORE_DELIMITER);

    setEnd(doc->positionTranslator().outputToRaw(_outputEnd));
    doc->positionTranslator().addRawTranslation(_start, s);
    raw.setPosition(_end);
    raw.insertText(Constants::TOKEN_CORE_DELIMITER);
    setEnd(_end + s);
//    qWarning() << "core end";
//    debug(2);
//    doc->positionTranslator().debug();
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

void PadItem::addDelimiter(const int posInRaw, const int size)
{
    PadDelimiter delim;
    delim.rawPos = posInRaw;
    delim.size = size;
    _delimiters << delim;
}

/** Overwrite the position tester, by default a PadTools::PadItem does not include the outputStart() and outputEnd() position. */
bool PadItem::containsOutputPosition(const int pos) const
{
    return IN_RANGE_STRICTLY(pos, _outputStart, _outputEnd);
}

/** Overwrite the position tester, by default a PadTools::PadItem does not include the outputStart() and outputEnd() position. */
bool PadItem::isBeforeOutputPosition(const int pos) const
{
    return pos >= _outputEnd;
}

/** Overwrite the position tester, by default a PadTools::PadItem does not include the outputStart() and outputEnd() position. */
bool PadItem::isAfterOutputPosition(const int pos) const
{
    return pos <= _outputStart;
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

/** Returns the PadTools::PadConditionnalSubItem of the PadTools::PadItem or 0 if not found. */
PadConditionnalSubItem *PadItem::subItem(const PadConditionnalSubItem::TokenCoreCondition cond, const PadConditionnalSubItem::Place place)
{
    PadConditionnalSubItem *sub;
    foreach (PadFragment *fragment, _fragments) {
        sub = dynamic_cast<PadConditionnalSubItem*>(fragment);
        if (sub) {
            if (sub->tokenCoreCondition() == cond && sub->place() == place)
                return sub;
        }
    }
    return 0;
}

void PadItem::run(QMap<QString,QVariant> &tokens, PadDocument *document)
{
//    qWarning() << "run Item: rawStart" << start() << "outputStart" << document->positionTranslator().rawToOutput(start()) << document->positionTranslator().deltaForSourcePosition(start());
    PadCore *core = getCore();
    QString coreValue;

    // if core value is null -> remove it from the output
    if (core) {
        coreValue = tokens.value(core->name()).toString();

        if (coreValue.isEmpty()) {
            // No value -> Remove the entire PadItem from the text output and add a translation
            QTextCursor cursor(document->outputDocument());
            setOutputStart(document->positionTranslator().rawToOutput(start()));
            cursor.setPosition(outputStart());
            cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            setOutputEnd(outputStart());
            document->positionTranslator().addOutputTranslation(outputStart(), -rawLength());
        } else {
            // Remove delimiters before the core
            foreach(const PadDelimiter &delim, _delimiters) {
//                qWarning() << delim.rawPos << core->start();
                if (delim.rawPos >= core->start())
                    continue;

//                qWarning() << "ITEM DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToOutput(delim.rawPos));

                QTextCursor cursor(document->outputDocument());
                int pos = document->positionTranslator().rawToOutput(delim.rawPos);
                cursor.setPosition(pos);
                cursor.setPosition(pos + delim.size, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                document->positionTranslator().addOutputTranslation(pos, -delim.size);
            }

            // Value -> run fragments
            foreach(PadFragment *f, _fragments) {
                f->run(tokens, document);
            }

            // Remove delimiters after the core
            foreach(const PadDelimiter &delim, _delimiters) {
                if (delim.rawPos < core->end())
                    continue;

//                qWarning() << "ITEM DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToOutput(delim.rawPos));

                QTextCursor cursor(document->outputDocument());
                int pos = document->positionTranslator().rawToOutput(delim.rawPos);
                cursor.setPosition(pos);
                cursor.setPosition(pos + delim.size, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                document->positionTranslator().addOutputTranslation(pos, -delim.size);
            }

            setOutputStart(document->positionTranslator().rawToOutput(start()));
            setOutputEnd(document->positionTranslator().rawToOutput(end()));
        }
    }
}

void PadItem::toOutput(Core::ITokenPool *pool, PadDocument *document)
{
//    qWarning() << "run Item: rawStart" << start() << "outputStart" << document->positionTranslator().rawToOutput(start()) << document->positionTranslator().deltaForSourcePosition(start());
    PadCore *core = getCore();
    QString coreValue;

    // if core value is null -> remove it from the output
    if (core) {
//        coreValue = tokens.value(core->name()).toString();
        coreValue = pool->tokenCurrentValue(core->name()).toString();

        if (coreValue.isEmpty()) {
            // No value -> Remove the entire PadItem from the text output and add a translation
            QTextCursor cursor(document->outputDocument());
            setOutputStart(document->positionTranslator().rawToOutput(start()));
            cursor.setPosition(outputStart());
            cursor.setPosition(outputStart() + rawLength(), QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            setOutputEnd(outputStart());
            document->positionTranslator().addOutputTranslation(outputStart(), -rawLength());
        } else {
            // Remove delimiters before the core
            foreach(const PadDelimiter &delim, _delimiters) {
//                qWarning() << delim.rawPos << core->start();
                if (delim.rawPos >= core->start())
                    continue;

//                qWarning() << "ITEM DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToOutput(delim.rawPos));

                QTextCursor cursor(document->outputDocument());
                int pos = document->positionTranslator().rawToOutput(delim.rawPos);
                cursor.setPosition(pos);
                cursor.setPosition(pos + delim.size, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                document->positionTranslator().addOutputTranslation(pos, -delim.size);
            }

            // Value -> run fragments
            foreach(PadFragment *f, _fragments) {
                f->toOutput(pool, document);
            }

            // Remove delimiters after the core
            foreach(const PadDelimiter &delim, _delimiters) {
                if (delim.rawPos < core->end())
                    continue;

//                qWarning() << "ITEM DELIM raw" << delim.rawPos << "size" << delim.size << "output" << (document->positionTranslator().rawToOutput(delim.rawPos));

                QTextCursor cursor(document->outputDocument());
                int pos = document->positionTranslator().rawToOutput(delim.rawPos);
                cursor.setPosition(pos);
                cursor.setPosition(pos + delim.size, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                document->positionTranslator().addOutputTranslation(pos, -delim.size);
            }

            setOutputStart(document->positionTranslator().rawToOutput(start()));
            setOutputEnd(document->positionTranslator().rawToOutput(end()));
        }
    }
}

void PadItem::toRaw(PadDocument *doc)
{
    // add delimiters at the beginning && the end
    QTextCursor raw(doc->rawSourceDocument());
    setStart(doc->positionTranslator().outputToRaw(_outputStart));
    raw.setPosition(_start);
    raw.insertText(Constants::TOKEN_OPEN_DELIMITER);
    int s = QString(Constants::TOKEN_OPEN_DELIMITER).size();
    doc->positionTranslator().addRawTranslation(_start, s);

//    doc->positionTranslator().debug();

    PadCore *core = getCore();
    Q_ASSERT(core);
    if (!core)
        return;
    core->toRaw(doc);

//    doc->positionTranslator().debug();

    setEnd(doc->positionTranslator().outputToRaw(_outputEnd));
    raw.setPosition(_end);
    raw.insertText(Constants::TOKEN_CLOSE_DELIMITER);
    doc->positionTranslator().addRawTranslation(_end, s);
    setEnd(_end + s);

    foreach(PadFragment *f, _fragments) {
        if (f!=core)
            f->toRaw(doc);
    }
//    qWarning() << "end";
//    doc->positionTranslator().debug();
//    debug();
}
