#ifndef PAD_CORE_H
#define PAD_CORE_H

#include <QString>

#include "pad_fragment.h"

class PadCore : public PadFragment
{
public:
/*	enum Type {
		Type_Value
		};*/

	PadCore(){}

	const QString &value() const { return _value; }
	void setValue(const QString &value) { _value = value; }

	void print(int indent = 0) const;

	QString run(QMap<QString,QVariant> &tokens) const;

private:
	QString _value;
};

#endif
