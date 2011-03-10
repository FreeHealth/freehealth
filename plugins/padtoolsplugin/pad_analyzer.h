#ifndef PAD_ANALYZER_H
#define PAD_ANALYZER_H

#include <QString>
#include <QStack>

#include "pad_fragment.h"
#include "pad_core.h"
#include "pad_string.h"
#include "pad.h"
#include "pad_item.h"

class PadAnalyzer
{
public:
	static const char coreDelimiter = '~';
	static const char padOpenDelimiter = '[';
	static const char padCloseDelimiter = ']';

	enum Error {
		Error_NoError
	};

	PadAnalyzer();

	/**
	 * Analyzes a mask text
	 */
	Pad *analyze(const QString &text);

	/**
	 * Returns the last parsing error
	 */
	Error lastParseError() const { return _lastParseError; }

private:
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

	Lexem _lexemNull;

	const QString *_text;
	int _length;
	int _curPos; // contains the current position in the analyzed text
	Error _lastParseError;

	bool atEnd(); // returns true if current position is at the text end
	PadItem *nextPadItem();
	PadCore *nextCore(); // tries to parse a core ("~...~")

	static bool isSpecial(const QChar &c);

	// returns the next lexem in the stream
	Lexem nextLexem();
};

#endif
