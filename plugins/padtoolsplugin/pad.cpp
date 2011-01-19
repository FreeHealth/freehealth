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
	foreach (PadFragment *fragment, _fragments){
		pad = dynamic_cast<Pad*>(fragment);
		if (pad){
			fragments.append(pad->getAllFragments());
		}
	}
	return fragments;
}
