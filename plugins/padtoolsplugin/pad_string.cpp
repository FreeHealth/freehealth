#include "pad_string.h"

void PadString::print(int indent) const
{
	QString str(indent, ' ');
	str += _string;
	qDebug(qPrintable(str));
}
