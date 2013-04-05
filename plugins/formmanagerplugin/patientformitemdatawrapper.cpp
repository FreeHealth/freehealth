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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Form::PatientFormItemDataWrapper
 * Get the patient data (see Core::IPatient::PatientDataRepresentation) from the form items.
 * If a patient data representation is available from the forms, you can get it from here.
 * This 'wrapper' always uses the \b last \b available value in the \b saved episodes.
 * \sa Patients::Internal::PatientModelWrapper, Core::IPatient
 */

#include "patientformitemdatawrapper.h"
#include "formcore.h"
#include "formmanager.h"
#include "iformitem.h"
#include "iformitemdata.h"
#include "episodemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>
#include <translationutils/constants.h>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

namespace Form {
namespace Internal {
class PatientFormItemDataWrapperPrivate
{
public:
    PatientFormItemDataWrapperPrivate(PatientFormItemDataWrapper *parent) :
        q(parent)
    {
    }
    
    ~PatientFormItemDataWrapperPrivate()
    {
    }
    
    // Scan all FormItem for patient data representation and feed _availablePatientData
    void scanFormItemDataForAvailableData(const QList<FormMain *> &emptyRootForms)
    {
        _availablePatientData.clear();
        foreach(FormMain *form, emptyRootForms) {
            foreach(FormItem *item, form->flattenFormItemChildren()) {
                if (item->itemData())
                    _availablePatientData << item->patientDataRepresentation();
            }
        }
        _availablePatientData.removeAll(-1);
    }

    // Clear cache then create all EpisodeModel for each available forms empty root
    void clearCacheAndCreateAllEpisodeModels(const QList<FormMain *> &forms)
    {
        qDeleteAll(_episodeModels.values());
        _episodeModels.clear();
        foreach(Form::FormMain *emptyrootform, forms) {
            foreach(Form::FormMain *form, emptyrootform->flattenFormMainChildren()) {
                EpisodeModel *model = new EpisodeModel(form, q);
                model->initialize();
                _episodeModels.insert(form, model);
            }
        }
    }

    // Populate all available EpisodeModels with their latest saved content
    void populateEpisodeModelsWithLastEpisode()
    {
        foreach(EpisodeModel *model, _episodeModels.values()) {
            if (!model->populateFormWithLatestValidEpisodeContent()) {
                LOG_ERROR_FOR(q, "EpisodeModel can not be populated");
            }
        }
    }

public:
    QList<int> _availablePatientData;
    QHash<Form::FormMain *, EpisodeModel *> _episodeModels;
    
private:
    PatientFormItemDataWrapper *q;
};
} // namespace Internal
} // end namespace Form

/*! Constructor of the Form::PatientFormItemDataWrapper class */
PatientFormItemDataWrapper::PatientFormItemDataWrapper(QObject *parent) :
    QObject(parent),
    d(new PatientFormItemDataWrapperPrivate(this))
{
    setObjectName("Form::PatientFormItemDataWrapper");
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onCurrentPatientFormsLoaded()), Qt::DirectConnection);
}

/*! Destructor of the Form::PatientFormItemDataWrapper class */
PatientFormItemDataWrapper::~PatientFormItemDataWrapper()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PatientFormItemDataWrapper::initialize()
{
    onCurrentPatientChanged();
    return true;
}

bool PatientFormItemDataWrapper::isDataAvailable(int ref) const
{
    return d->_availablePatientData.contains(ref);
}

QVariant PatientFormItemDataWrapper::data(int ref, int role) const
{
    Q_UNUSED(role);
    if (!d->_availablePatientData.contains(ref))
        return QVariant();

    // Find the FormMain parent that contains the item with the correct 'ref'
    const QList<Form::FormMain*> &forms = d->_episodeModels.uniqueKeys();
    foreach(Form::FormMain *main, forms) {
        foreach(Form::FormItem *item, main->flattenFormItemChildren()) {
            if (!item->itemData() || item->patientDataRepresentation() == -1)
                continue;
            // TODO: if the lastepisode does not contain the data, try to find the lastest recorded value
            if (item->patientDataRepresentation() == ref) {
                return item->itemData()->data(ref, Form::IFormItemData::PatientModelRole);
            }
        }
    }
    return QVariant();
}

void PatientFormItemDataWrapper::onCurrentPatientChanged()
{
}

void PatientFormItemDataWrapper::onCurrentPatientFormsLoaded()
{
    // TODO: Disconnect all EpisodeModels

    // Get Duplicates FormCollections
    QList<FormMain *> forms = formManager().allDuplicatesEmptyRootForms();
    d->scanFormItemDataForAvailableData(forms);

    // Create all EpisodeModel
    d->clearCacheAndCreateAllEpisodeModels(forms);

    // Populate each forms with its lastest recorded episode
    d->populateEpisodeModelsWithLastEpisode();

    // TODO: Reconnect all EpisodeModels
}
