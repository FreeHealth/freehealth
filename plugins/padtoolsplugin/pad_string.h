#ifndef MASK_STRING_H
#define MASK_STRING_H

#include <QString>

#include "pad_fragment.h"

class PadString : public PadFragment
{
public:
	PadString(const QString &string = "") : PadFragment(), _string(string) {}

	const QString &string() const { return _string; }
	void setValue(const QString &string) { _string = string; }

	void print(int indent = 0) const;

	QString run(QMap<QString,QVariant> &) const;
private:
	QString _string;
};

#endif
