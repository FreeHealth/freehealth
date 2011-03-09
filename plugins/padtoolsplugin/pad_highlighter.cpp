#include "pad_highlighter.h"

void PadHighlighter::highlightBlock(const QString &text)
{
	for (int i = 0; i < text.count(); ++i) {
		if (text[i] == PadAnalyzer::padOpenDelimiter)
			setFormat(i, 1, _padFormat);
		else if (text[i] == PadAnalyzer::padCloseDelimiter)
			setFormat(i, 1, _padFormat);
		else if (text[i] == PadAnalyzer::coreDelimiter)
			setFormat(i, 1, _coreFormat);
	}
}

void PadHighlighter::init()
{
	_padFormat.setForeground(Qt::darkRed);
	_padFormat.setFontWeight(QFont::Bold);
	_coreFormat.setForeground(Qt::darkBlue);
	_coreFormat.setFontWeight(QFont::Bold);
	_prefixFormat.setBackground(QColor(200, 255, 255));
}
