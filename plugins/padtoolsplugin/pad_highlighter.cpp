#include "pad_highlighter.h"

void PadHighlighter::highlightBlock(const QString &text)
{
/*	BlockState state = (BlockState) previousBlockState();
	if (state == -1)
	state = State_Normal;*/

	for (int i = 0; i < text.count(); ++i) {
		if (text[i] == PadAnalyzer::padOpenDelimiter) {
			setFormat(i, 1, _padFormat);
/*			if (state != State_Core)
			state = State_Prefix;*/
		} else if (text[i] == PadAnalyzer::padCloseDelimiter) {
			setFormat(i, 1, _padFormat);
		} else if (text[i] == PadAnalyzer::coreDelimiter)
			setFormat(i, 1, _coreFormat);/*
		else if (state == State_Prefix)
			setFormat(i, 1, _prefixFormat);
		else if (state == State_Postfix)
			setFormat(i, 1, _prefixFormat);
		else if (state == State_Core)
		setFormat(i, 1, _coreTextFormat);*/
	}

/*	setCurrentBlockState(state);*/
}

void PadHighlighter::init()
{
	_padFormat.setForeground(Qt::darkRed);
	_padFormat.setFontWeight(QFont::Bold);
	_coreFormat.setForeground(Qt::darkBlue);
	_coreFormat.setFontWeight(QFont::Bold);
	_prefixFormat.setBackground(QColor(200, 255, 255));
	_coreTextFormat.setBackground(QColor(255, 200, 255));
}
