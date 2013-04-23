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
 * \class Form::Internal::EpisodeManager
 * Manages all editing Form::EpisodeModel. The models are created with the editing form collection
 * not the duplicate collection.
 * \sa Form::EpisodeModel, Form::FormManager
 */

#include "episodemanager.h"
#include "episodemodel.h"
#include "formcore.h"
#include "formmanager.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

namespace Form {
namespace Internal {
class EpisodeManagerPrivate
{
public:
    EpisodeManagerPrivate(EpisodeManager */*parent*/) //:q(parent)
    {
    }

    ~EpisodeManagerPrivate()
    {
    }

public:
    QHash<Form::FormMain *, EpisodeModel *> _episodeModels;

private:
//    EpisodeManager *q;
};
}  // namespace Internal
} // end namespace Form

/*! Constructor of the Form::Internal::EpisodeManager class */
EpisodeManager::EpisodeManager(QObject *parent) :
    QObject(parent),
    d(new EpisodeManagerPrivate(this))
{
}

/*! Destructor of the Form::Internal::EpisodeManager class */
EpisodeManager::~EpisodeManager()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool EpisodeManager::initialize()
{
    return true;
}

/**
 * Return the unique Form::EpisodeModel linked to the patient form \e form.
 * Return zero if the form is not null.
 */
EpisodeModel *EpisodeManager::episodeModel(Form::FormMain *form)
{
    if (!form)
        return 0;

    // Not in cache ?
    if (!d->_episodeModels.value(form, 0)) {
        // Create the model
        EpisodeModel *model = new EpisodeModel(form, this);
        model->initialize();
        d->_episodeModels.insert(form, model);
        return model;
    }
    return d->_episodeModels.value(form);
}

/**
 * Return the unique Form::EpisodeModel linked to the patient form identified by \e formUid.
 * Return zero if the form \e formUid is not available.
 */
EpisodeModel *EpisodeManager::episodeModel(const QString &formUid)
{
    return episodeModel(formManager().form(formUid));
}

