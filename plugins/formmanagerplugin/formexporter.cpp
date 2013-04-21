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
 * \class Form::FormExporter
 * Export all recorded episode for a collection of forms.
 * \sa Form::FormItemSpec::Spec_HtmlExportMask, Form::FormItemSpec::Spec_PlainTextExportMask
 */

#include "formexporter.h"
#include "iformitem.h"
#include "episodedata.h"
#include "episodemanager.h"
#include "episodemodel.h"
#include "formcore.h"
#include "formmanager.h"
#include "episodebase.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <QSortFilterProxyModel>

#include <QDebug>

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Form::FormCore &formCore() {return Form::FormCore::instance();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

// TODO: manage user rights here +++

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Form {
namespace Internal {

struct FormExportation {
    QStringList css;
    QMultiMap<QDateTime, QString> episodes;
};

class FormExporterPrivate
{
public:
    FormExporterPrivate(FormExporter *parent) :
        _identityOnly(false),
        q(parent)
    {
    }

    ~FormExporterPrivate()
    {
    }

    int countEpisodes()
    {
        int episodes = 0;
        if (_identityOnly) {
            Form::FormMain *form = formManager().identityRootForm();
            if (form)
                episodes = 1;
        } else {
            // Get all current forms
            QList<Form::FormMain*> forms = formManager().allDuplicatesEmptyRootForms();

            // Export all FormMain episodes
            foreach(Form::FormMain *emptyRoot, forms) {
                foreach(Form::FormMain *form, emptyRoot->flattenedFormMainChildren()) {
                    if (form->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
                        continue;
                    EpisodeModel *model = formCore().episodeManager().episodeModel(form);
                    if (model->currentPatientUuid() != patient()->uuid())
                        return 0;
                    // Get *all* episodes
                    QModelIndex fetchIndex = model->index(model->rowCount(), 0);
                    while (model->canFetchMore(fetchIndex)) {
                        model->fetchMore(fetchIndex);
                        fetchIndex = model->index(model->rowCount(), 0);
                    }
                    episodes += model->rowCount();
                }
            }
        }
        return episodes;
    }

    FormExportation formExportation(Form::FormMain *form, const QString &patientUid)
    {
        FormExportation formExport;
        // Use FormManager EpisodeModel
        EpisodeModel *model = formCore().episodeManager().episodeModel(form);
        model->setCurrentPatient(patientUid);

        // Get *all* episodes
        QModelIndex fetchIndex = model->index(model->rowCount(), 0);
        while (model->canFetchMore(fetchIndex)) {
            model->fetchMore(fetchIndex);
            fetchIndex = model->index(model->rowCount(), 0);
        }

        // Add a sortproxymodel wrapper
        QSortFilterProxyModel *proxy = new QSortFilterProxyModel(model);
        proxy->setSourceModel(model);
        proxy->sort(EpisodeModel::UserTimeStamp, Qt::DescendingOrder);

        // Read all rows of the model
        QString htmlMask = Utils::htmlBodyContent(form->spec()->value(FormItemSpec::Spec_HtmlExportMask).toString().simplified());
        formExport.css << Utils::htmlTakeAllCssContent(htmlMask);
        if (htmlMask.isEmpty()) {
            QString tmp = form->printableHtml(true);
            formExport.css << Utils::htmlTakeAllCssContent(tmp);
        }

        // Start episode extraction
        for(int i=0; i < proxy->rowCount(); ++i) {
            QModelIndex index = proxy->mapToSource(proxy->index(i, 0));
            const QDateTime &dt = model->data(model->index(index.row(), EpisodeModel::UserTimeStamp, index.parent())).toDateTime();

            // Populate form with data
            model->populateFormWithEpisodeContent(index, false);

            // Try to export with the HTML mask
            if (!htmlMask.isEmpty()) {
                QString tmpMask = htmlMask;
                Utils::replaceTokens(tmpMask, formManager().formToTokens(form));
                patient()->replaceTokens(tmpMask);
                user()->replaceTokens(tmpMask);
                qWarning() << "\n\n" << form->spec()->label() << "\n" << tmpMask;
                formExport.episodes.insertMulti(dt, Utils::htmlBodyContent(tmpMask));
            } else {
                // Get the default HTML output
                QString tmp = form->printableHtml(true);
                Utils::htmlTakeAllCssContent(tmp);
                qWarning() << "\n\n" << form->spec()->label() << "\n" << tmp;
                formExport.episodes.insertMulti(dt, Utils::htmlBodyContent(tmp));
            }
        }
        return formExport;
    }

    QList<FormExportation> extractFormEpisodes(const Core::PatientDataExporterJob &job, const QString &patientUid)
    {
        QList<FormExportation> formExports;

        if (_identityOnly) {
            Form::FormMain *form = formManager().identityRootForm();
            if (form)
                formExports << formExportation(form, patientUid);
        } else {
            // Get all current forms
            QList<Form::FormMain*> forms = formManager().allDuplicatesEmptyRootForms();

            // Export all FormMain episodes
            foreach(Form::FormMain *emptyRoot, forms) {
                foreach(Form::FormMain *form, emptyRoot->flattenedFormMainChildren()) {
                    if (form->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
                        continue;
                    formExports << formExportation(form, patientUid);
                }
            }
        }
        return formExports;

    }

    QString constructOutputContent(const Core::PatientDataExporterJob &job, const QList<FormExportation> &formExports)
    {

        qWarning() << "----------------- OUTPUT" << formExports.count();

        // Join all css style sheets
        QString html;
        QString css;
        foreach(const FormExportation &exp, formExports)
            css += exp.css.join("\n");
        if (job.exportGroupmentType() == Core::PatientDataExporterJob::DateOrderedExportation) {
            // Join all extracted episodes into one unique map
            QMultiMap<QDateTime, QString> allEpisodes;
            foreach(const FormExportation &exp, formExports) {
                foreach(const QDateTime &dt, exp.episodes.uniqueKeys()) {
                    foreach(const QString &val, exp.episodes.values(dt)) {
                        allEpisodes.insertMulti(dt, val);
                    }
                }
            }

            // Construct HTML
            qWarning() << "-------- episodecount" << allEpisodes.values().count();
            foreach(const QString &val, allEpisodes.values()) {
                html += val;
            }
        } else { // FormOrderedExportation
            // Construct HTML
            foreach(const FormExportation &exp, formExports) {
                foreach(const QString &val, exp.episodes.values()) {
                    html += val;
                }
            }
        }

        // Re-insert CSS style blocks
        int begin = html.indexOf("<body");
        if (begin == -1) {
            html.prepend(css);
        } else {
            begin = html.indexOf(">", begin + 2);
            ++begin;
            html.insert(begin, css);
        }

        Utils::saveStringToFile(html, QString("/Users/eric/Desktop/patient_%1%2.html")
                                .arg(_identityOnly ? "ident_" : "forms_")
                                .arg(patient()->uuid()));
        return html;
    }


public:
    bool _identityOnly;

private:
    FormExporter *q;
};
} // namespace Internal
} // end namespace Form


/*! Constructor of the Form::FormExporter class */
FormExporter::FormExporter(bool identityOnly, QObject *parent) :
    Core::IPatientDataExporter(parent),
    d(new FormExporterPrivate(this))
{
    setObjectName("FormExporter");
    setIdentityOnly(identityOnly);
}

/*! Destructor of the Form::FormExporter class */
FormExporter::~FormExporter()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * Form exporter can manage:
 * - the unique declared identity form
 * - all other forms
 * The exporter never manages both (identity + forms). You need to create two distinct objects.
 */
void FormExporter::setIdentityOnly(bool identityOnly)
{
    d->_identityOnly = identityOnly;
    if (identityOnly)
        setExporterTypes(IdentityExporter);
    else
        setExporterTypes(FormsExporter);
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormExporter::initialize()
{
    return true;
}

bool FormExporter::isBusy() const
{
    return false;
}

#include <QTextBrowser>
Core::PatientDataExtraction *FormExporter::startExportationJob(const Core::PatientDataExporterJob &job)
{
    // Patient to export must be the current patient
    if (job.patientUids().count()==0)
        return 0;

    if (job.patientUids().at(0).isEmpty())
        return 0;

    // Patient to extract must be the current one
    if (job.patientUids().at(0) != patient()->uuid()) {
        LOG_ERROR("Extracting wrong patient");
        return 0;
    }

    Core::PatientDataExtraction *result = 0;

    int count = d->countEpisodes();
    qWarning() << "EXPORTATION" << patient()->uuid() << "EPISODE COUNT:" << count;

    QList<FormExportation> formExports = d->extractFormEpisodes(job, job.patientUids().at(0));
    d->constructOutputContent(job, formExports);

    return result;
}
