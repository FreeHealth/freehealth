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

#include <utils/log.h>

#include <QTextCursor>
#include <QTime>

#include <QDebug>

enum { WarnLexemMatching = false, WarnIsDelimiterDebug = false };

using namespace PadTools;
using namespace Internal;

namespace PadTools {
namespace Internal {
class PadAnalyzerPrivate
{
public:
    enum LexemType {
        Lexem_Null = 0,
        Lexem_String,
        Lexem_PadOpenDelimiter,
        Lexem_PadCloseDelimiter,
        Lexem_CoreDelimiter
    };

    struct Lexem {
        LexemType type;		// type of the lexem
        QString value;		// value (can be empty) of the lexem
        QString rawValue;	// raw value of the lexem (never empty)
        int start;			// start index in the analyzed text
        int end;			// end index in the analyzed text
    };

    PadAnalyzerPrivate(PadAnalyzer *parent):
        _sourceDocument(0),
        _curPos(-1), // _curPos == -1 means no current analyze
        q(parent)
    {
        _lexemNull.type = Lexem_Null;
    }

    QChar getCharAt(int pos, QTextDocument *doc)
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

    QString getStringAt(int pos, QTextDocument *doc, int size = 1)
    {
        QTextCursor cursor(doc);
        cursor.setPosition(pos);
        if (cursor.atEnd())
            return QString::null;
        cursor.setPosition(pos + size, QTextCursor::KeepAnchor);
        return cursor.selectedText();
    }

    PadDocument *startAnalyze(PadDocument *padDocument = 0);

    bool atEnd();
    PadItem *nextPadItem();
    PadCore *nextCore();

    bool isDelimiter(int pos, int *delimiterSize, LexemType *type);

    Lexem nextLexem();

    int nextId() {return ++_id;}
    void resetId() {_id=0;}


public:
    Lexem _lexemNull;
    QTextDocument *_sourceDocument;
    int _curPos; // contains the current position in the analyzed text
    uint _id;
    QList<Core::PadAnalyzerError> _lastErrors;

private:
    PadAnalyzer *q;
};

}
}

// TODO: PadAnalyzer::PadAnalyzer -> make jobs asynchronous and threaded
PadAnalyzer::PadAnalyzer(QObject *parent) :
    QObject(parent),
    d(new Internal::PadAnalyzerPrivate(this))
{
}

PadAnalyzer::~PadAnalyzer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** Analyzes a mask \e text and return a \e PadTools::PadDocument pointer. */
PadDocument *PadAnalyzer::analyze(const QString &source)
{
    if (d->_sourceDocument && d->_sourceDocument->parent()==this) {
        delete d->_sourceDocument;
        d->_sourceDocument = 0;
    }
    d->_sourceDocument = new QTextDocument(this);
    d->_sourceDocument->setPlainText(source);
    return d->startAnalyze();
}

PadDocument *PadAnalyzer::analyze(QTextDocument *source, PadDocument *padDocument)
{
    if (d->_sourceDocument && d->_sourceDocument->parent()==this) {
        delete d->_sourceDocument;
        d->_sourceDocument = 0;
    }
    d->_sourceDocument = source;
    return d->startAnalyze(padDocument);
}

const QList<Core::PadAnalyzerError> PadAnalyzer::lastErrors() const
{
    return d->_lastErrors;
}

PadDocument *PadAnalyzerPrivate::startAnalyze(PadDocument *padDocument)
{
    Lexem lex;
    PadDocument *pad;
    // TODO: where, when and who delete the PadDocument pointer ? */
    padDocument ? pad = padDocument : pad = new PadDocument(q);
    pad->setSource(_sourceDocument);
    pad->beginRawSourceAnalyze();

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
        case Lexem_String: // ignore this kind of fragment
			break;
		case Lexem_PadOpenDelimiter:
			fragment = nextPadItem();
			if (!fragment) { // bad pad item => turn it into a string fragment
                // TODO: manage error here */
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
                                                  _curPos - 1,
                                                  errorTokens);

			// turn it into a string fragment
            pos = _curPos - QString(Constants::TOKEN_CLOSE_DELIMITER).size();
            // TODO: manage error here */
            fragment = new PadString();//text.mid(pos, _curPos - pos));
			fragment->setStart(pos);
			fragment->setEnd(_curPos - 1);
            fragment->setId(nextId());
            break;
		case Lexem_CoreDelimiter:
			// raise an error (unexpected core delimiter)
            errorTokens.insert("char", QString(Constants::TOKEN_CORE_DELIMITER));
			_lastErrors << Core::PadAnalyzerError(Core::PadAnalyzerError::Error_UnexpectedChar,
                                                  _curPos - 1,
												  errorTokens);
			// turn it into a string fragment
			pos = _curPos - 1;
            // TODO: manage error here */
            fragment = new PadString();//text.mid(pos, _curPos - pos));
            fragment->setStart(pos);
			fragment->setEnd(_curPos - 1);
            fragment->setId(nextId());
            break;
		default:
			// TODO: raise an error (unknown lexem)
			break;
		}
        if (fragment)
            pad->addChild(fragment);
	}
    pad->endRawSourceAnalyze();
    return pad;
}

/** Extract positions of a PadItem (full token) with children from source */
PadItem *PadAnalyzerPrivate::nextPadItem()
{
    PadConditionnalSubItem *fragment;
	Lexem lex;
    PadItem *padItem = new PadItem;
    int s = QString(Constants::TOKEN_OPEN_DELIMITER).size();
    padItem->addDelimiter(_curPos - s, s);
    padItem->setStart(_curPos - s);
    padItem->setId(nextId());
    int previousType = PadItem::DefinedCore_PrependText;

	// we expect strings, pad item, core (uniq) or close delimiter
	while ((lex = nextLexem()).type != Lexem_Null) {
		fragment = 0;
        switch (lex.type) {
        case Lexem_String:
        {
            switch (previousType) {
            case PadItem::DefinedCore_PrependText :
            {
                fragment = new PadConditionnalSubItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend);
//                int s = QString(Constants::TOKEN_OPEN_DELIMITER).size();
                fragment->setStart(lex.start);
                fragment->setEnd(lex.end);

//                qWarning() << "PREPEND" << fragment->start() << fragment->end() << "Delim" << (lex.start - s) << s;

                break;
            }
            case PadItem::DefinedCore_AppendText:
            {
                fragment = new PadConditionnalSubItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append);
                fragment->setStart(lex.start);
                fragment->setEnd(lex.end);

//                qWarning() << "APPEND" << fragment->start() << fragment->end() << "Delim" << lex.end << s;;

                break;
            }
            case PadItem::UndefinedCore_AppendText : break;
            case PadItem::UndefinedCore_PrependText: break;
            default: LOG_ERROR_FOR(q->objectName(), "No type for PadItem subItem");
            }
            fragment->setId(nextId());
            padItem->addChild(fragment);
            break;
        }
        case Lexem_PadOpenDelimiter:
        {
            PadItem *item = nextPadItem();
            if (!item) { // an error occured, stop all
                delete padItem;
                return 0;
            }
            padItem->addChild(item);
            break;
        }
        case Lexem_PadCloseDelimiter:
            s = QString(Constants::TOKEN_CLOSE_DELIMITER).size();
            padItem->addDelimiter(_curPos - s, s);
            padItem->setEnd(_curPos);
            return padItem;
		case Lexem_CoreDelimiter:
        {
            // TODO: raise an error when a core has already been defined */
            PadCore *core = nextCore();
            if (!core) { // an error occured, stop all
				delete padItem;
				return 0;
			}
            padItem->addChild(core);
            previousType = PadItem::DefinedCore_AppendText;
			break;
        }
		default:
			// TODO: raise an error (unknown lexem)
			break;
		}
	}
	delete padItem;
	return 0;
}

/** Extracts the core of a token: '~CORE~'. This member is called when analyzer finds a starting core delimiter. */
PadCore *PadAnalyzerPrivate::nextCore()
{
	Lexem lex;
	QMap<QString,QVariant> errorTokens;
    PadCore *core = new PadCore;
    int size = QString(Constants::TOKEN_CORE_DELIMITER).size();
    core->setStart(_curPos - size);
    core->setId(nextId());

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
                                              _curPos - 1,
                                              errorTokens);
		delete core;
		return 0;
	}
    core->setEnd(_curPos);
    core->setName(getStringAt(core->start() + size, _sourceDocument, core->rawLength() - (size*2)));
    return core;
}

/** Returns true if the string is a delimiter. */
bool PadAnalyzerPrivate::isDelimiter(int pos, int *delimiterSize, LexemType *type)
{
    Q_ASSERT(delimiterSize);
    // Some init
    *delimiterSize = 0;
    *type = Lexem_Null;
    if (pos<0)
        return false;

    // Get current char; stop if pos in out of the text
    QChar currentChar;
    currentChar = getCharAt(pos, _sourceDocument);

    if (WarnIsDelimiterDebug)
        qWarning() << "    isDelimiter(); curChar" << currentChar;

    if (currentChar == Constants::TOKEN_OPEN_DELIMITER[0]) {
        QString tmp;
        int size = QString(Constants::TOKEN_OPEN_DELIMITER).size();
        tmp = getStringAt(pos, _sourceDocument, size);

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
        tmp = getStringAt(pos, _sourceDocument, size);

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
        tmp = getStringAt(pos, _sourceDocument, size);
        if (tmp == Constants::TOKEN_CORE_DELIMITER) {
            *delimiterSize = size;
            *type = Lexem_CoreDelimiter;
            return true;
        }
    }
    return false;
}

/** Returns the next lexem in the stream */
PadAnalyzerPrivate::Lexem PadAnalyzerPrivate::nextLexem()
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

    if (WarnLexemMatching)
        qWarning() << "  * value" << lexem.value << "_curPos" << _curPos << "lex.start" << lexem.start << "lex.end" << lexem.end;

    return lexem;
}

/** Returns true if current position reaches the end of the text. */
bool PadAnalyzerPrivate::atEnd()
{
    QTextCursor cursor(_sourceDocument);
    cursor.setPosition(_curPos);
    return cursor.atEnd();
}
