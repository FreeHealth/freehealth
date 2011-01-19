#include "pad_core.h"

void PadCore::print(int indent) const
{
	QString str(indent, ' ');
	str += "$" + _value + "$";
	qDebug(qPrintable(str));
}
