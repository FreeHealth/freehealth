#ifndef PAD_HIGHLIGHTER_H
#define PAD_HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "pad_analyzer.h"

struct BlockData : public QTextBlockUserData
{
	enum TokenType {
		Token_OpenPad,
		Token_CoreDelimiter,
		Token_Core
	};
	QVector<TokenType> tokens;

	void eatClosePad();
	void eatCoreDelimiter();
};

class PadHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	PadHighlighter(QObject *parent) : QSyntaxHighlighter(parent) { init(); }
	PadHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) { init(); }
	PadHighlighter(QTextEdit *parent) : QSyntaxHighlighter(parent) { init(); }

protected:
    void highlightBlock(const QString &text);

private:
/*	enum BlockState {
		State_Normal = 0,
		State_Prefix,
		State_Postfix,
		State_Core
		};*/

	PadAnalyzer _padAnalyzer;
	QTextCharFormat _padFormat;
	QTextCharFormat _coreFormat;
	QTextCharFormat _prefixFormat;
	QTextCharFormat _postfixFormat;
	QTextCharFormat _coreTextFormat;

	void init();
};

#endif
