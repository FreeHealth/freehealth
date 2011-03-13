#include "pad_analyzer.h"

PadAnalyzer::PadAnalyzer()
	: _curPos(-1) // _curPos == -1 means no current analyze
{
	_lexemNull.type = Lexem_Null;
}

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
			errorTokens.insert("char", QString(padCloseDelimiter));
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
			errorTokens.insert("char", QString(coreDelimiter));
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

bool PadAnalyzer::atEnd()
{
	return _curPos >= _length;
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
		errorTokens.insert("char", QString(coreDelimiter));
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
	return c == padOpenDelimiter ||
		c == padCloseDelimiter ||
		c == coreDelimiter;
}

PadAnalyzer::Lexem PadAnalyzer::nextLexem()
{
	if (atEnd()) // no more lexem to read
		return _lexemNull;

	Lexem lexem;
	lexem.start = _curPos;
	lexem.end = _curPos;

	const QString &text = *_text;
	if (text[_curPos] == padOpenDelimiter) {
		_curPos++;
		lexem.type = Lexem_PadOpenDelimiter;
		lexem.rawValue = padOpenDelimiter;
		return lexem;
	} else if (text[_curPos] == padCloseDelimiter) {
		_curPos++;
		lexem.type = Lexem_PadCloseDelimiter;
		lexem.rawValue = padCloseDelimiter;
		return lexem;
	} else if (text[_curPos] == coreDelimiter) {
		_curPos++;
		lexem.type = Lexem_CoreDelimiter;
		lexem.rawValue = coreDelimiter;
		return lexem;
	}

	// string?
	lexem.type = Lexem_String;
	while (!atEnd() && !isSpecial(text[_curPos])) {
		// special escape char?
		if (text[_curPos] == '\\') {
			_curPos++;
			if (_curPos < _length && isSpecial(text[_curPos])) { // we've got a special char to escape
				lexem.value += text[_curPos];
				_curPos++;
			} else { // no special => store the '\' itself
				lexem.value += '\\';
			}
		} else
			lexem.value += text[_curPos++];
	}
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

