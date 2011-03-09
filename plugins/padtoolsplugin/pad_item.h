#ifndef PAD_ITEM_H
#define PAD_ITEM_H

#include <QList>
#include <QMap>
#include <QVariant>

#include "pad_fragment.h"
#include "pad_core.h"

/**
 * Contains an entire pad item i.e. a list of fragments
 * @class
 */
class PadItem : public PadFragment
{
public:
	PadItem(){}
	virtual ~PadItem();

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
