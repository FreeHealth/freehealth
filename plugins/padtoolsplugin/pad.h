#ifndef PAD_H
#define PAD_H

#include <QList>
#include <QMap>
#include <QVariant>

#include "pad_fragment.h"
#include "pad_core.h"

/**
 * Contains an entire pad i.e. a list of fragments
 * @class
 */
class Pad : public PadFragment
{
public:
	Pad(){}
	virtual ~Pad();

	void addFragment(PadFragment *fragment);

	QList<PadFragment*> getAllFragments() const;

	void print(int indent = 0) const;

	/**
	 * Run this pad over some tokens and returns the result text
	 */
	QString run(QMap<QString,QVariant> &tokens) const;

private:
	QList<PadFragment*> _fragments;

	// return the core of the pad (can be 0)
	PadCore *getCore() const;
};

#endif
