#include <QString>

#include "pad_item.h"

PadItem::~PadItem()
{
	qDeleteAll(_fragments);
}

void PadItem::addFragment(PadFragment *fragment)
{
	_fragments << fragment;
}

void PadItem::print(int indent) const
{
	QString str(indent, ' ');
	str += "[padItem]";
	qDebug("%s", qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
		fragment->print(indent + 2);
	}
}

QList<PadFragment*> PadItem::getAllFragments() const
{
	QList<PadFragment*> fragments;
	PadItem *padItem;
	fragments.append(_fragments);
	foreach (PadFragment *fragment, _fragments)
	{
		padItem = dynamic_cast<PadItem*>(fragment);
		if (padItem)
			fragments.append(padItem->getAllFragments());
	}
	return fragments;
}

QString PadItem::run(QMap<QString,QVariant> &tokens) const
{
	Q_UNUSED(tokens);

	QString value;
	PadCore *core = getCore();
	QString coreValue;

	// if a core exists, the entire pad expression is optional, depending on the core emptiness
	if (core)
	{
		coreValue = core->run(tokens);
		if (coreValue.isEmpty()) // core empty? so the entire pad will be empty too
			return "";
	}

	foreach (PadFragment *fragment, _fragments)
		value += fragment->run(tokens);

	return value;
}

PadCore *PadItem::getCore() const
{
	PadCore *core;
	foreach (PadFragment *fragment, _fragments)
	{
		core = dynamic_cast<PadCore*>(fragment);
		if (core)
			return core;
	}
	return 0;
}
