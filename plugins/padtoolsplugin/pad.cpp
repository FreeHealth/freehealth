#include <QString>

#include "pad.h"
#include "pad_item.h"

Pad::~Pad()
{
	qDeleteAll(_fragments);
}

void Pad::addFragment(PadFragment *fragment)
{
	_fragments << fragment;
}

void Pad::print(int indent) const
{
	QString str(indent, ' ');
	str += "[pad]";
	qDebug("%s", qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
		fragment->print(indent + 2);
	}
}

QList<PadFragment*> Pad::getAllFragments() const
{
	QList<PadFragment*> fragments;
	PadItem *item;
	fragments.append(_fragments);
	foreach (PadFragment *fragment, _fragments)
	{
		item = dynamic_cast<PadItem*>(fragment);
		if (item)
			fragments.append(item->getAllFragments());
	}
	return fragments;
}

QString Pad::run(QMap<QString,QVariant> &tokens) const
{
	Q_UNUSED(tokens);

	QString value;

	foreach (PadFragment *fragment, _fragments)
		value += fragment->run(tokens);

	return value;
}
