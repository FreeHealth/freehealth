#ifndef PAD_FRAGMENT_H
#define PAD_FRAGMENT_H

#include <QString>

/**
 * Represent a string or a token
 */
class PadFragment
{
public:
	PadFragment() {}

	virtual void print(int indent = 0) const = 0;

	int start() const { return _start; }
	void setStart(int start) { _start = start; }
	int end() const { return _end; }
	void setEnd(int end) { _end = end; }

	const QString &rawValue() const { return _rawValue; }
	void setRawValue(const QString &value) { _rawValue = value; }

private:
	int _start; // index of the first char in the text
	int _end; // index of the last char in the text
	QString _rawValue;
};

#endif
