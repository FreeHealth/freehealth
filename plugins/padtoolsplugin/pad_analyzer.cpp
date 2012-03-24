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

#include <QDebug>
enum { WarnLexemMatching = false };

using namespace PadTools;

PadAnalyzer::PadAnalyzer()
	: _curPos(-1) // _curPos == -1 means no current analyze
{
	_lexemNull.type = Lexem_Null;
}

/** Analyzes a mask \e text and return a \e PadTools::Pad pointer. */
Pad *PadAnalyzer::analyze(const QString &text)
{
	Lexem lex;
	Pad *pad = new Pad;
	PadFragment *fragment;
	int pos;
	QMap<QString,QVariant> errorTokens;

	_text = &text;
	_length = text.count();
	_curPos = 0;
	_lastErrors.clear();

	// let's extract some lexems in the stream
	while ((lex = nextLexem()).type != Lexem_Null) {
		fragment = 0;
		switch (lex.type) {
		case Lexem_String:
			fragment = new PadString(lex.value);
			break;
		case Lexem_PadOpenDelimiter:
			pos = _curPos - 1;
			fragment = nextPadItem();
			if (!fragment) { // bad pad item => turn it into a string fragment
				fragment = new PadString(text.mid(pos, _curPos - pos));
				fragment->setRawValue(text.mid(pos, _curPos - pos));
				fragment->setStart(pos);
				fragment->setEnd(_curPos - 1);
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
			fragment = new PadString(text.mid(pos, _curPos - pos));
			fragment->setRawValue(text.mid(pos, _curPos - pos));
			fragment->setStart(pos);
			fragment->setEnd(_curPos - 1);
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
			fragment = new PadString(text.mid(pos, _curPos - pos));
			fragment->setRawValue(text.mid(pos, _curPos - pos));
			fragment->setStart(pos);
			fragment->setEnd(_curPos - 1);
			break;
		default:
			// TODO: raise an error (unknown lexem)
			break;
		}
		pad->addFragment(fragment);
	}

	return pad;
}

PadItem *PadAnalyzer::nextPadItem()
{
	const QString &text = *_text;
	PadFragment *fragment;
	Lexem lex;
	PadItem *padItem = new PadItem;
	padItem->setStart(_curPos - 1);

	// we expect strings, pad item, core (uniq) or close delimiter
	while ((lex = nextLexem()).type != Lexem_Null) {
		fragment = 0;
		switch (lex.type) {
		case Lexem_String:
			padItem->addFragment(new PadString(lex.value));
			break;
		case Lexem_PadOpenDelimiter:
			fragment = nextPadItem();
			if (!fragment) { // an error occured, stop all
				delete padItem;
				return 0;
			}
			padItem->addFragment(fragment);
			break;
		case Lexem_PadCloseDelimiter:
			padItem->setEnd(_curPos - 1);
			padItem->setRawValue(text.mid(padItem->start(), padItem->end() - padItem->start() + 1));
			return padItem;
		case Lexem_CoreDelimiter:
			// TODO: raise an error when a core has already been defined
			fragment = nextCore();
			if (!fragment) { // an error occured, stop all
				delete padItem;
				return 0;
			}
			padItem->addFragment(fragment);
			break;
		default:
			// TODO: raise an error (unknown lexem)
			break;
		}
	}
	delete padItem;
	return 0;
}

PadCore *PadAnalyzer::nextCore()
{
	const QString &text = *_text;
	Lexem lex;
	QMap<QString,QVariant> errorTokens;
	PadCore *core = new PadCore;
	core->setStart(_curPos - 1);

	// first, we expect string (or not)
	lex = nextLexem();
	if (lex.type == Lexem_String) {
		core->setValue(lex.value);
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
	core->setRawValue(text.mid(core->start(), core->end() - core->start() + 1));

	return core;
}

bool PadAnalyzer::isSpecial(const QChar &c)
{
    return c == QString(Constants::TOKEN_OPEN_DELIMITER).at(0) ||
            c == QString(Constants::TOKEN_CLOSE_DELIMITER).at(0) ||
            c == QString(Constants::TOKEN_CORE_DELIMITER).at(0);
}

/** Returns true if the string is a delimiter. */
bool PadAnalyzer::isDelimiter(int pos, const QString &c, int *delimiterSize, LexemType *type)
{
    Q_ASSERT(delimiterSize);
    *delimiterSize = 0;
    *type = Lexem_Null;
    if (pos<0 || pos>=c.size())
        return false;

    if (c.at(pos) == Constants::TOKEN_OPEN_DELIMITER[0]) {

//        qWarning() << "  isDelimiter:: OpenDelim found" << c.mid(pos, QString(Constants::TOKEN_OPEN_DELIMITER).size());

        if (c.mid(pos, QString(Constants::TOKEN_OPEN_DELIMITER).size()) == Constants::TOKEN_OPEN_DELIMITER) {
            *delimiterSize = QString(Constants::TOKEN_OPEN_DELIMITER).size();
            *type = Lexem_PadOpenDelimiter;
            return true;
        }
    }
    if (c.at(pos) == Constants::TOKEN_CLOSE_DELIMITER[0]) {

//        qWarning() << "  isDelimiter:: OpenDelim found" << c.mid(pos, QString(Constants::TOKEN_OPEN_DELIMITER).size());

        if (c.mid(pos, QString(Constants::TOKEN_CLOSE_DELIMITER).size()) == Constants::TOKEN_CLOSE_DELIMITER) {
            *delimiterSize = QString(Constants::TOKEN_CLOSE_DELIMITER).size();
            *type = Lexem_PadCloseDelimiter;
            return true;
        }
    }
    if (c.at(pos) == Constants::TOKEN_CORE_DELIMITER[0]) {
        if (c.mid(pos, QString(Constants::TOKEN_CORE_DELIMITER).size()) == Constants::TOKEN_CORE_DELIMITER) {
            *delimiterSize = QString(Constants::TOKEN_CORE_DELIMITER).size();
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
        qWarning() << "Next Lexem" << _curPos << atEnd();

    if (atEnd())
		return _lexemNull;

	Lexem lexem;
	lexem.start = _curPos;
	lexem.end = _curPos;

	const QString &text = *_text;
    int size = 0;
    LexemType type;
    if (isDelimiter(_curPos, text, &size, &type)) {
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
//    if (text[_curPos] == QString(Constants::TOKEN_OPEN_DELIMITER).at(0)) {
//		_curPos++;
//		lexem.type = Lexem_PadOpenDelimiter;
//        lexem.rawValue = Constants::TOKEN_OPEN_DELIMITER;
//		return lexem;
//    } else if (text[_curPos] == QString(Constants::TOKEN_CLOSE_DELIMITER).at(0)) {
//		_curPos++;
//		lexem.type = Lexem_PadCloseDelimiter;
//        lexem.rawValue = Constants::TOKEN_CLOSE_DELIMITER;
//		return lexem;
//    } else if (text[_curPos] == QString(Constants::TOKEN_CORE_DELIMITER).at(0)) {
//		_curPos++;
//		lexem.type = Lexem_CoreDelimiter;
//        lexem.rawValue = Constants::TOKEN_CORE_DELIMITER;
//		return lexem;
//	}

	// string?
	lexem.type = Lexem_String;
    if (WarnLexemMatching)
        qWarning() << "  isString";
    while (!atEnd() && !isDelimiter(_curPos, text, &size, &type)) {
        // special escape char?
        if (text[_curPos] == '\\') {
            _curPos++;
            if (_curPos < _length && isDelimiter(_curPos, text, &size, &type)) { // we've got a special char to escape
                lexem.value += text[_curPos];
                _curPos++;
            } else { // no special => store the '\' itself
                lexem.value += '\\';
            }
        } else
            lexem.value += text[_curPos++];
    }
//    while (!atEnd() && !isSpecial(text[_curPos])) {
//		// special escape char?
//		if (text[_curPos] == '\\') {
//			_curPos++;
//			if (_curPos < _length && isSpecial(text[_curPos])) { // we've got a special char to escape
//				lexem.value += text[_curPos];
//				_curPos++;
//			} else { // no special => store the '\' itself
//				lexem.value += '\\';
//			}
//		} else
//			lexem.value += text[_curPos++];
//	}
    if (WarnLexemMatching)
        qWarning() << "  * value" << lexem.value << "_curPos" << _curPos;
    lexem.end = _curPos - 1;
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

