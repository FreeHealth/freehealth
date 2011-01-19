#ifndef PAD_ANALYZER_H
#define PAD_ANALYZER_H

#include <QString>
#include <QStack>

#include "pad_fragment.h"
#include "pad_core.h"
#include "pad_string.h"
#include "pad.h"

class PadAnalyzer
{
public:
	PadAnalyzer();

	/**
	 * Analyzes a mask text
	 */
	Pad *analyze(const QString &text);

private:
	const QString *_text;
	int _length;
	int _curPos; // contains the current position in the analyzed text

	bool atEnd(); // return true if current position is at the text end
	Pad *nextPad(bool root = false); // try to parse a mask ("[prefix$...$suffix]")
	PadCore *nextCore(); // try to parse a core ("$...$")
	PadString *nextString();
};

#endif
