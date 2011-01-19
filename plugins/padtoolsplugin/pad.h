#ifndef PAD_H
#define PAD_H

#include <QList>

#include "pad_fragment.h"

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

private:
	QList<PadFragment*> _fragments;
};

#endif
