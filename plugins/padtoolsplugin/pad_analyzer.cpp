#include "pad_analyzer.h"

PadAnalyzer::PadAnalyzer()
	: _curPos(-1) // _curPos == -1 means no current analyze
{
}

Pad *PadAnalyzer::analyze(const QString &text)
{
	_text = &text;
	_length = text.count();
	_curPos = 0;

	return nextPad(true);
}

bool PadAnalyzer::atEnd()
{
	return _curPos >= _length;
}

Pad *PadAnalyzer::nextPad(bool root)
{
	const QString &text = *_text;
	Pad *pad = new Pad;
	bool specialMode = false;

	pad->setStart(_curPos);

	if (!root){
		// pass the '['
		_curPos++;
	}

	while (!atEnd()){
		if (text[_curPos] == '\\' && !specialMode){
			specialMode = true;
			_curPos++;
		} else if (text[_curPos] == '[' && !specialMode){ // new sub_pad
			pad->addFragment(nextPad());
			if (!atEnd()){ // if not at end, we were on the ']' char => pass it
				_curPos++;
			}
		} else if (text[_curPos] == '$' && !specialMode){ // a core
			pad->addFragment(nextCore());
			if (!atEnd()){ // if not at end, we were on the '$' char => pass it
				_curPos++;
			}
		} else if (text[_curPos] == ']' && !specialMode){ // the end
			pad->setEnd(_curPos);
			pad->setRawValue(text.mid(pad->start(), pad->end() - pad->start() + 1));
			return pad;
		} else{ // a simple string
			if (specialMode){
				_curPos--; // get back to the previous '\'
				specialMode = false;
			}
			pad->addFragment(nextString());
		}
	}
	pad->setEnd(_curPos - 1);
	pad->setRawValue(text.mid(pad->start(), pad->end() - pad->start() + 1));
	return pad;
}

PadCore *PadAnalyzer::nextCore()
{
	const QString &text = *_text;
	PadCore *core = new PadCore;
	QString str;

	core->setStart(_curPos);

	// pass the '$'
	_curPos++;

	while (!atEnd() && text[_curPos] != '$'){
		str += text[_curPos];
		_curPos++;
	}
	core->setValue(str);

	if (!atEnd()){
		core->setEnd(_curPos);
	} else{
		core->setEnd(_curPos - 1);
	}

	core->setRawValue(text.mid(core->start(), core->end() - core->start() + 1));

	return core;
}

PadString *PadAnalyzer::nextString()
{
	const QString &text = *_text;
	QString str;
	bool specialMode = false;
	PadString *padString = new PadString;

	padString->setStart(_curPos);

	// TODO manage '\' escape char
/*	while (!atEnd()){
		if (text[_curPos] == '$' || text[_curPos] == '[' || text[_curPos] == ']'){
			if (specialMode){
				str += text[_curPos];
				specialMode = false;
			} else
				break;
		}
		if (!specialMode && text[_curPos] == '\\'){
			specialMode = true;
			_curPos++;
		}


//		if (specialMode && text[
}*/

	while (!atEnd() && text[_curPos] != '$' && text[_curPos] != '[' && text[_curPos] != ']'){
		str += text[_curPos];
		_curPos++;
	}
	padString->setValue(str);

	if (!atEnd()){
		padString->setEnd(_curPos);
	} else{
		padString->setEnd(_curPos - 1);
	}

	padString->setRawValue(text.mid(padString->start(), padString->end() - padString->start() + 1));

	return padString;
}
