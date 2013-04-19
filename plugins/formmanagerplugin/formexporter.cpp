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
#include "episodemodel.h"
#include "formcore.h"
#include "formmanager.h"
#include "episodebase.h"

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
class FormExporterPrivate
{
public:
    FormExporterPrivate(FormExporter *parent) :
        _useCurrent(true),
        _useAll(true),
        q(parent)
    {
    }

    ~FormExporterPrivate()
    {
    }

    QString formOrderedExport(const FormExporterJob &job, const QString &patientUid)
    {
        if (patientUid.isEmpty())
            return QString::null;

        // Change the current patient in order to extract the identity, update the forms, sub-forms...
        patient()->setCurrentPatientUid(patientUid);

        // Get all current forms
        QList<Form::FormMain*> forms = formManager().allDuplicatesEmptyRootForms();

        // Foreach FormMain get all its episode
        QString html;
        QStringList formCss;
        foreach(Form::FormMain *emptyRoot, forms) {
            foreach(Form::FormMain *form, emptyRoot->flattenedFormMainChildren()) {
                // Use EpisodeModel
                EpisodeModel *model = new EpisodeModel(form, q);
                model->initialize();
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
                formCss << Utils::htmlTakeAllCssContent(htmlMask);

                for(int i=0; i < proxy->rowCount(); ++i) {
                    QModelIndex index = proxy->mapToSource(proxy->index(i, 0));

                    // Populate form with data
                    model->populateFormWithEpisodeContent(index, false);

                    // Try to export with the HTML mask
                    if (!htmlMask.isEmpty()) {
                        QString tmpMask = htmlMask;
                        Utils::replaceTokens(tmpMask, formManager().formToTokens(form));
                        patient()->replaceTokens(tmpMask);
                        user()->replaceTokens(tmpMask);
                        html += Utils::htmlBodyContent(tmpMask);
                    } else {
                        // Get the default HTML output
                        html += Utils::htmlBodyContent(form->printableHtml(true));
                    }
                }
            }
        }
        // Re-insert CSS style blocks
        int begin = html.indexOf("<body");
        if (begin == -1) {
            html.prepend(formCss.join("\n"));
        } else {
            begin = html.indexOf(">", begin + 2);
            ++begin;
            html.insert(begin, formCss.join("\n"));
        }
        Utils::saveStringToFile(html, QString("/Users/eric/Desktop/patient_%1.html").arg(patientUid));
        return html;

//                // TODO: get episodes one by one not all in once
//                // Get all its episodes
//                EpisodeBaseQuery query;
//                query.setValidEpisodes(true);
//                query.setPatientUid(_patientUid);
//                query.setFormUids(QStringList() << form->uuid());
//                query.setGetEpisodeContent(true);
//                QList<EpisodeData *> episodes = episodeBase()->getEpisodes(query);

//                // Sort episodes by date
//                qSort(episodes.begin(), episodes.end(), EpisodeData::lessThanByUserDate);

//                // Foreach episode
//                foreach(EpisodeData *episode, episodes) {
//                    // Populate form with the episode

//                    // Create the exportation of the populated form

//                    // Append it to the output
//                }

//            }
//        }
    }

    QString dateOrderedExport()
    {
        //
        return QString::null;
    }

public:
    bool _useCurrent, _useAll;

private:
    FormExporter *q;
};
} // namespace Internal
} // end namespace Form


/*! Constructor of the Form::FormExporter class */
FormExporter::FormExporter(QObject *parent) :
    QObject(parent),
    d(new FormExporterPrivate(this))
{
}

/*! Destructor of the Form::FormExporter class */
FormExporter::~FormExporter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormExporter::initialize()
{
    return true;
}

void FormExporter::setUseCurrentPatientForms(bool useCurrent)
{
    d->_useCurrent = useCurrent;
}

void FormExporter::setUseAllAvailableForms(bool useAll)
{
    d->_useAll = useAll;
}

#include <QTextBrowser>
void FormExporter::startExportation(const FormExporterJob &job)
{
    // Export all recorded non-form data like PMHx
    // Get all emptyRootForms
    if (job.exportGroupmentType() == FormExporterJob::FormOrderedExportation) {
        foreach(const QString &uid, job.patientUids()) {
            QTextBrowser *browser = new QTextBrowser;
            browser->setHtml(d->formOrderedExport(job, uid));
            browser->show();
        }
    } else if (job.exportGroupmentType() == FormExporterJob::DateOrderedExportation) {

    }
//    return QString::null;
}
