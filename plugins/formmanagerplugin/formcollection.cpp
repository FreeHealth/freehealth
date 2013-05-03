/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Form::FormCollection
 * \brief Holds a book of forms (a specific mode, the central form, duplicates or not...).
 * The form books are created and managed by the Form::FormManager object. Books contain
 * all forms available for a specific mode. This object is responsible of the life of
 * the registered Form::FormMain pointers. Every Form::FormMain pointers are deleted here.
 * \sa Form::FormManager
 */

#include "formcollection.h"
#include "iformitem.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Form {
namespace Internal {
class FormCollectionPrivate
{
public:
    FormCollectionPrivate(FormCollection */*parent*/) :
        _isDuplicates(false),
        _type(FormCollection::CompleteForm)  // , q(parent)
    {
    }

    ~FormCollectionPrivate()
    {
    }

public:
    bool _isDuplicates;
    QString _modeUid, _formUid;
    QList<Form::FormMain *> _emptyRootForms;
    FormCollection::CollectionType _type;

private:
//    FormCollection *q;
};
}  // namespace Internal
} // end namespace Form

/*! Constructor of the Form::FormCollection class */
FormCollection::FormCollection() : //(QObject *parent) :
//    QObject(parent),
    d(new FormCollectionPrivate(this))
{
}

/*! Destructor of the Form::FormCollection class */
FormCollection::~FormCollection()
{
    qDeleteAll(d->_emptyRootForms);
    d->_emptyRootForms.clear();
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormCollection::initialize()
{
    return true;
}

void FormCollection::setDuplicates(bool isDuplicates)
{
    d->_isDuplicates = isDuplicates;
}

void FormCollection::setType(CollectionType type)
{
    d->_type = type;
}

bool FormCollection::isNull() const
{
    return d->_modeUid.isEmpty() && d->_formUid.isEmpty() && d->_emptyRootForms.isEmpty();
}

bool FormCollection::isDuplicates() const
{
    return d->_isDuplicates;
}

QString FormCollection::formUid() const
{
    return d->_formUid;
}

QString FormCollection::modeUid() const
{
    return d->_modeUid;
}

FormCollection::CollectionType FormCollection::type() const
{
    return d->_type;
}

void FormCollection::setEmptyRootForms(const QList<Form::FormMain *> &emptyRootForms)
{
    d->_emptyRootForms = emptyRootForms;
    // extract the formuid
    if (emptyRootForms.count()) {
        d->_formUid = emptyRootForms.at(0)->uuid();
        d->_modeUid = emptyRootForms.at(0)->modeUniqueName();
    }
}

void FormCollection::addEmptyRootForm(Form::FormMain *emptyRootForm)
{
    d->_emptyRootForms << emptyRootForm;
}

const QList<FormMain *> &FormCollection::emptyRootForms() const
{
    return d->_emptyRootForms;
}

bool FormCollection::containsFormUid(const QString &formUid) const
{
    return (form(formUid)!=0);
}

bool FormCollection::containsIdentityForm() const
{
    return (identityForm()!=0);
}

Form::FormMain *FormCollection::identityForm() const
{
    for(int i=0; i < d->_emptyRootForms.count(); ++i) {
        FormMain *root = d->_emptyRootForms.at(i);
        if (root->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
            return root;
        foreach(FormMain *form, root->flattenedFormMainChildren()) {
            if (form->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
                return form;
        }
    }
    return 0;
}

/**
 * Returns the Form::FormMain corresponding with the uuid like \e formUid or \e zero
 * if the collection does not include this form.
 */
Form::FormMain *FormCollection::form(const QString &formUid) const
{
    foreach(Form::FormMain *form, d->_emptyRootForms) {
        // Test all empty roots
        if (form->uuid() == formUid)
            return form;
        // Test all children
        foreach(Form::FormMain *test, form->flattenedFormMainChildren()) {
            if (test->uuid() == formUid)
                return test;
        }
    }
    return 0;
}
