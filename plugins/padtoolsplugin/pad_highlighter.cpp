#include "pad_highlighter.h"

void PadHighlighter::highlightBlock(const QString &text)
{
	Pad *pad = _padAnalyzer.analyze(text);
	QString rawValue;

	foreach (PadFragment *fragment, pad->getAllFragments()){
		rawValue = fragment->rawValue();
		if (dynamic_cast<Pad*>(fragment)){
			if (rawValue.count()){
				if (rawValue[0] == '['){
					setFormat(fragment->start(), 1, _padFormat);
				}
				if (rawValue[rawValue.count() - 1] == ']'){
					setFormat(fragment->end(), 1, _padFormat);
				}
			}
		} else if (dynamic_cast<PadCore*>(fragment)){
			if (rawValue.count()){
				if (rawValue[0] == '$'){
					setFormat(fragment->start(), 1, _coreFormat);
				}
				if (rawValue[rawValue.count() - 1] == '$'){
					setFormat(fragment->end(), 1, _coreFormat);
				}
			}
		}
	}

	delete pad;
}

void PadHighlighter::init()
{
	_padFormat.setForeground(Qt::darkRed);
	_padFormat.setFontWeight(QFont::Bold);
	_coreFormat.setForeground(Qt::darkBlue);
	_coreFormat.setFontWeight(QFont::Bold);
}
