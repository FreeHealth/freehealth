#include <QString>

#include "pad.h"

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
	qDebug(qPrintable(str));
	foreach (PadFragment *fragment, _fragments){
		fragment->print(indent + 2);
	}
}

QList<PadFragment*> Pad::getAllFragments() const
{
	QList<PadFragment*> fragments;
	Pad *pad;
	fragments.append(_fragments);
	foreach (PadFragment *fragment, _fragments)
	{
		pad = dynamic_cast<Pad*>(fragment);
		if (pad)
			fragments.append(pad->getAllFragments());
	}
	return fragments;
}

QString Pad::run(QMap<QString,QVariant> &tokens) const
{
	Q_UNUSED(tokens);

	QString value;
	PadCore *core = getCore();
	QString coreValue;

	// if a core exists, the entire pad expression is optional, depending on the core value
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

PadCore *Pad::getCore() const
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
