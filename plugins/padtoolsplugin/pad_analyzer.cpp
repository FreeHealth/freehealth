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
  \class PadTools::PadAnalyzer
  Analyze a string for tokens.
*/

#include "pad_analyzer.h"
#include "constants.h"

#include <QTextCursor>

#include <QDebug>

enum { WarnLexemMatching = false, WarnIsDelimiterDebug = false };

using namespace PadTools;

static QChar getCharAt(int pos, QTextDocument *doc)
{
    QTextCursor cursor(doc);
    cursor.setPosition(pos);
    if (cursor.atEnd())
        return QChar::Null;
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    if (cursor.selectedText().size() > 0)
        return cursor.selectedText().at(0);
    return QChar::Null;
}

static QString getStringAt(int pos, QTextDocument *doc, int size = 1)
{
    QTextCursor cursor(doc);
    cursor.setPosition(pos);
    if (cursor.atEnd())
        return QString::null;
    cursor.setPosition(pos + size, QTextCursor::KeepAnchor);
    return cursor.selectedText();
}

PadAnalyzer::PadAnalyzer() :
    _text(0),
    _document(0),
    _length(0),
    _curPos(-1) // _curPos == -1 means no current analyze
{
	_lexemNull.type = Lexem_Null;
}

/** Analyzes a mask \e text and return a \e PadTools::Pad pointer. */
PadDocument *PadAnalyzer::analyze(const QString &text)
{
    _document = 0;
    _text = &text;
    _length = text.count();
    return startAnalyze();
}

PadDocument *PadAnalyzer::analyze(QTextDocument *document)
{
    _document = document;
    _text = 0;
    _length = -1;
    return startAnalyze();
}

PadDocument *PadAnalyzer::startAnalyze()
{
	Lexem lex;
    PadDocument *pad;
    if (_text)
        pad = new PadDocument(*_text);
    else
        pad = new PadDocument(_document);
	PadFragment *fragment;
	int pos;
	QMap<QString,QVariant> errorTokens;

	_curPos = 0;
	_lastErrors.clear();
    resetId();

    // Extract lexems in the stream
	while ((lex = nextLexem()).type != Lexem_Null) {
		fragment = 0;
		switch (lex.type) {
		case Lexem_String:
            fragment = new PadString(lex.value);
            fragment->setStart(lex.start);
            fragment->setEnd(lex.end);
            fragment->setId(nextId());
			break;
		case Lexem_PadOpenDelimiter:
			pos = _curPos - 1;
			fragment = nextPadItem();
			if (!fragment) { // bad pad item => turn it into a string fragment
                /** \todo manage error here */
                fragment = new PadString();//text.mid(pos, _curPos - pos));
				fragment->setStart(pos);
				fragment->setEnd(_curPos - 1);
                fragment->setId(nextId());
            }
			break;
		case Lexem_PadCloseDelimiter:
			// raise an error (unexpected close delimiter)
            errorTokens.insert("char", QString(Constants::TOKEN_CLOSE_DELIMITER));// padCloseDelimiter));
			_lastErrors << Core::PadAnalyzerError(Core::PadAnalyzerError::Error_UnexpectedChar,
												  getLine(_curPos - 1),
												  getPos(_curPos - 1),
												  errorTokens);

			// turn it into a string fragment
			pos = _curPos - 1;
            /** \todo manage error here */
            fragment = new PadString();//text.mid(pos, _curPos - pos));
			fragment->setStart(pos);
			fragment->setEnd(_curPos - 1);
            fragment->setId(nextId());
            break;
		case Lexem_CoreDelimiter:
			// raise an error (unexpected core delimiter)
            errorTokens.insert("char", QString(Constants::TOKEN_CORE_DELIMITER));
			_lastErrors << Core::PadAnalyzerError(Core::PadAnalyzerError::Error_UnexpectedChar,
												  getLine(_curPos - 1),
												  getPos(_curPos - 1),
												  errorTokens);
			// turn it into a string fragment
			pos = _curPos - 1;
            /** \todo manage error here */
            fragment = new PadString();//text.mid(pos, _curPos - pos));
            fragment->setStart(pos);
			fragment->setEnd(_curPos - 1);
            fragment->setId(nextId());
            break;
		default:
			// TODO: raise an error (unknown lexem)
			break;
		}
        pad->addChild(fragment);
	}

    pad->print();
	return pad;
}

PadItem *PadAnalyzer::nextPadItem()
{
	PadFragment *fragment;
	Lexem lex;
	PadItem *padItem = new PadItem;
	padItem->setStart(_curPos - 1);
    padItem->setId(nextId());
    int previousType = PadItem::ConditionnalBeforeText;

	// we expect strings, pad item, core (uniq) or close delimiter
	while ((lex = nextLexem()).type != Lexem_Null) {
		fragment = 0;
		switch (lex.type) {
		case Lexem_String:
            fragment = new PadString(lex.value);
            fragment->setStart(lex.start);
            fragment->setEnd(lex.end);
            fragment->setId(nextId());
            fragment->setUserData(Constants::USERDATA_KEY_PADITEM, previousType);
            padItem->addChild(fragment);
            break;
		case Lexem_PadOpenDelimiter:
			fragment = nextPadItem();
			if (!fragment) { // an error occured, stop all
				delete padItem;
				return 0;
			}
            padItem->addChild(fragment);
			break;
		case Lexem_PadCloseDelimiter:
			padItem->setEnd(_curPos - 1);
			return padItem;
		case Lexem_CoreDelimiter:
			// TODO: raise an error when a core has already been defined
			fragment = nextCore();
			if (!fragment) { // an error occured, stop all
				delete padItem;
				return 0;
			}
            padItem->addChild(fragment);
            previousType = PadItem::ConditionnalAfterText;
			break;
		default:
			// TODO: raise an error (unknown lexem)
			break;
		}
	}
	delete padItem;
	return 0;
}

/** Extracts the core of a token: '~CORE~'. This member is called when analyzer finds a starting core delimiter. */
PadCore *PadAnalyzer::nextCore()
{
	Lexem lex;
	QMap<QString,QVariant> errorTokens;
    PadCore *core = new PadCore;
	core->setStart(_curPos - 1);
    core->setId(nextId());
    core->setUserData(Constants::USERDATA_KEY_PADITEM, PadItem::Core);

	// first, we expect string (or not)
	lex = nextLexem();
	if (lex.type == Lexem_String) {
        core->setName(lex.value);
		lex = nextLexem();
	}

	// then comes the core end delimiter
	if (lex.type != Lexem_CoreDelimiter) {
		// raise an error (unexpected core delimiter)
        errorTokens.insert("char", QString(Constants::TOKEN_CORE_DELIMITER));
		_lastErrors << Core::PadAnalyzerError(Core::PadAnalyzerError::Error_CoreDelimiterExpected,
											  getLine(_curPos - 1),
											  getPos(_curPos - 1),
											  errorTokens);
		delete core;
		return 0;
	}
	core->setEnd(_curPos - 1);
	return core;
}

/** Returns true if the string is a delimiter. */
bool PadAnalyzer::isDelimiter(int pos, int *delimiterSize, LexemType *type)
{
    Q_ASSERT(delimiterSize);
    // Some init
    *delimiterSize = 0;
    *type = Lexem_Null;
    if (pos<0)
        return false;

    // Get current char; stop if pos in out of the text
    QChar currentChar;
    if (_text) {
        if (pos >= _text->size())
            return false;
        currentChar = _text->at(pos);
    } else if (_document) {
        currentChar = getCharAt(pos, _document);
    }

    if (WarnIsDelimiterDebug)
        qWarning() << "    isDelimiter(); curChar" << currentChar;

    if (currentChar == Constants::TOKEN_OPEN_DELIMITER[0]) {
        QString tmp;
        int size = QString(Constants::TOKEN_OPEN_DELIMITER).size();
        if (_text)
            tmp = _text->mid(pos, size);
        else if (_document)
            tmp = getStringAt(pos, _document, size);

        if (WarnIsDelimiterDebug)
            qWarning() << "  isDelimiter():: OpenDelim found" << tmp << "delimSize" << size;

        if (tmp == Constants::TOKEN_OPEN_DELIMITER) {
            *delimiterSize = size;
            *type = Lexem_PadOpenDelimiter;
            return true;
        }
    }
    if (currentChar == Constants::TOKEN_CLOSE_DELIMITER[0]) {
        QString tmp;
        int size = QString(Constants::TOKEN_OPEN_DELIMITER).size();
        if (_text)
            tmp = _text->mid(pos, size);
        else if (_document)
            tmp = getStringAt(pos, _document, size);

        if (WarnIsDelimiterDebug)
            qWarning() << "  isDelimiter():: CloseDelim found" << tmp << "delimSize" << size;

        if (tmp == Constants::TOKEN_CLOSE_DELIMITER) {
            *delimiterSize = size;
            *type = Lexem_PadCloseDelimiter;
            return true;
        }
    }
    if (currentChar == Constants::TOKEN_CORE_DELIMITER[0]) {
        QString tmp;
        int size = QString(Constants::TOKEN_CORE_DELIMITER).size();
        if (_text)
            tmp = _text->mid(pos, size);
        else if (_document)
            tmp = getStringAt(pos, _document, size);
        if (tmp == Constants::TOKEN_CORE_DELIMITER) {
            *delimiterSize = size;
            *type = Lexem_CoreDelimiter;
            return true;
        }
    }
    return false;
}

/** Returns the next lexem in the stream */
PadAnalyzer::Lexem PadAnalyzer::nextLexem()
{
    if (WarnLexemMatching)
        qWarning() << "Next Lexem; curpos" << _curPos <<  "atEnd" << atEnd();

    if (atEnd())
		return _lexemNull;

	Lexem lexem;
	lexem.start = _curPos;
	lexem.end = _curPos;

    int size = 0;
    LexemType type;
    if (isDelimiter(_curPos, &size, &type)) {
        if (WarnLexemMatching)
            qWarning() << "    isDelimiter" << _curPos << size << type;

        _curPos += size;
        lexem.type = type;
        lexem.end = _curPos;
        switch (type) {
        case Lexem_PadOpenDelimiter: lexem.rawValue = QString(Constants::TOKEN_OPEN_DELIMITER); break;
        case Lexem_PadCloseDelimiter: lexem.rawValue = QString(Constants::TOKEN_CLOSE_DELIMITER); break;
        case Lexem_CoreDelimiter: lexem.rawValue = QString(Constants::TOKEN_CORE_DELIMITER); break;
        default: break;
        }
        if (WarnLexemMatching)
            qWarning() << "  found delim" << lexem.rawValue << lexem.start << lexem.end;
        return lexem;
    }

	// string?
	lexem.type = Lexem_String;
    if (WarnLexemMatching)
        qWarning() << "  isString";

    while (!atEnd() && !isDelimiter(_curPos, &size, &type)) {
        ++_curPos;
    }
    lexem.end = _curPos;

    if (_text) {
        lexem.value = _text->mid(lexem.start, lexem.end - lexem.start);
    } else if (_document) {
        lexem.value = getStringAt(lexem.start, _document, lexem.end - lexem.start);
    }

    if (WarnLexemMatching)
        qWarning() << "  * value" << lexem.value << "_curPos" << _curPos << "lex.start" << lexem.start << "lex.end" << lexem.end;

    return lexem;
}

int PadAnalyzer::getLine(int curPos) const
{
	const QString &text = *_text;
	int line = 0;

	curPos = curPos == -1 ? _curPos : curPos;

	for (int i = 0; i < _length; ++i) {
		if (i == curPos)
			return line;
		if (text[i] == '\n')
			line++;
	}
	return line;
}

int PadAnalyzer::getPos(int curPos) const
{
	const QString &text = *_text;

	curPos = curPos == -1 ? _curPos : curPos;

	int pos = curPos;
	while (pos >= 0) {
		pos--;
		if (pos >= 0 && text[pos] == '\n')
			break;
	}
	return curPos - pos - 1;
}

/** Returns true if current position reaches the end of the text. */
bool PadAnalyzer::atEnd()
{
    if (_text) {
        return _curPos >= _text->count();
    } else {
        QTextCursor cursor(_document);
        cursor.setPosition(_curPos);
        return cursor.atEnd();
    }
}
