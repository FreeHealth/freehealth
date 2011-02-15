/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class PMH::PmhViewer
  \brief PMHx viewer widget. Allow to show / modify a PMH.
  This class uses PMH::Internal::PmhData pointers to create the view. It is not
  mapped to the model. You must:
  - set the PMH::Internal::PmhData to use
  - get it back after the edition
  - then send it to the PMH::PmhCategoryModel
*/

#include "pmhviewer.h"
#include "pmhdata.h"
#include "constants.h"
#include "pmhepisodemodel.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhcategoryonlyproxymodel.h"

#include <utils/global.h>
#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include "ui_pmhviewer.h"

using namespace PMH;
using namespace Internal;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline PMH::PmhCore *pmhCore() { return PMH::PmhCore::instance(); }


namespace PMH {
namespace Internal {

class PmhViewerPrivate {
public:
    PmhViewerPrivate() : ui(0), m_Pmh(0), m_ShowPatient(false) {}
    ~PmhViewerPrivate()
    {
        delete ui; ui=0;
    }

    void setEditMode(PmhViewer::EditMode mode)
    {
        m_Mode = mode;
        Q_ASSERT(ui);
        bool enable = (mode == PmhViewer::ReadWriteMode);
        ui->personalLabel->setEnabled(enable);
        ui->typeCombo->setEnabled(enable);
        ui->statusCombo->setEnabled(enable);
    }

    void populateUiWithPmh(PmhData *pmh)
    {
        m_Pmh = pmh;
        ui->personalLabel->setText(pmh->data(PmhData::Label).toString());
        ui->typeCombo->setCurrentIndex(pmh->data(PmhData::Type).toInt());
        ui->statusCombo->setCurrentIndex(pmh->data(PmhData::State).toInt());
        ui->confIndexSlider->setValue(pmh->data(PmhData::ConfidenceIndex).toInt());
        ui->makePrivateBox->setChecked(pmh->data(PmhData::IsPrivate).toBool());
        ui->comment->setHtml(pmh->data(PmhData::Comment).toString());
        // Category

        // Populate EpisodeView
        ui->episodeViewer->setPmhData(pmh);

        // Populate Link viewer
    }

    void populatePmhWithUi()
    {
        m_Pmh->setData(PmhData::Label, ui->personalLabel->text());
        m_Pmh->setData(PmhData::Type, ui->typeCombo->currentIndex());
        m_Pmh->setData(PmhData::State, ui->statusCombo->currentIndex());
        m_Pmh->setData(PmhData::ConfidenceIndex, ui->confIndexSlider->value());
        m_Pmh->setData(PmhData::Comment, ui->comment->textEdit()->toHtml());
        m_Pmh->setData(PmhData::IsPrivate, ui->makePrivateBox->isChecked());

//        m_Pmh->setData(PmhData::CategoryId, ui->categoryTreeview);
    }


public:
    Internal::Ui::PmhViewer *ui;
    PmhViewer::EditMode m_Mode;
    PmhData *m_Pmh;
    bool m_ShowPatient;
};

} // End namespace Internal
} // End namespace PMH


/** \brief Creates a new PMH::PmhViewer with the specified \e editMode. \sa PMH::PmhViewer::EditMode */
PmhViewer::PmhViewer(QWidget *parent, EditMode editMode) :
    QWidget(parent), d(new PmhViewerPrivate)
{
    // Create Ui
    d->ui = new Internal::Ui::PmhViewer;
    d->ui->setupUi(this);

    // Populate combos
    d->ui->typeCombo->addItems(Constants::availableTypes());
    d->ui->statusCombo->addItems(Constants::availableStatus());

    // adjust tabwidget
    d->ui->tabWidget->setCurrentWidget(d->ui->categoryTab);
    d->ui->comment->toogleToolbar(true);

    // Manage the Edit Mode
    d->setEditMode(editMode);
    setShowPatientInformations(d->m_ShowPatient);

    // add models to views
    d->ui->categoryTreeview->setModel(pmhCore()->pmhCategoryModel()->categoryOnlyModel());
}

PmhViewer::~PmhViewer()
{
    delete d; d = 0;
}

/** \brief Show or hide the patient informations (title, name, age) inside the dialog. */
void PmhViewer::setShowPatientInformations(bool show)
{
    if (show) {
        d->ui->patientGroup->show();
        QString text = "<b>"+ patient()->data(Core::IPatient::Title).toString();;
        text += " " + patient()->data(Core::IPatient::FullName).toString();
        text += ", " + patient()->data(Core::IPatient::Age).toString();
        text += "</b>";
        d->ui->patientInfos->setText(text);
    } else {
        d->ui->patientGroup->hide();
    }
}

/** Defines the edit mode to use (Read only or Read Write). \sa PMH::PmhViewer::EditMode */
void PmhViewer::setEditMode(EditMode mode)
{
    d->setEditMode(mode);
}

/** \brief Define the PMH::Internal::PmhData pointer to use in the view. */
void PmhViewer::setPmhData(Internal::PmhData *pmh)
{
    if (d->m_Pmh) {
        if (d->m_Pmh == pmh)
            return;
    }
    d->populateUiWithPmh(pmh);
}

/** \brief Create a new PMH::Internal::PmhData pointer and use it in the view. Equivalent to setPmhData(new PMH::Internal::PmhData) */
void PmhViewer::createNewPmh()
{
    if (d->m_Pmh) {
        Utils::warningMessageBox(tr("Replacing pmh data"),"","");
    }
    PmhData *pmh = new PmhData;
    pmh->populateWithCurrentData();
    d->populateUiWithPmh(pmh);
}

/** \brief Removes any editing and restore values to the originals ones of the PmhData. */
void PmhViewer::revert()
{
    if (d->m_Pmh)
        d->populateUiWithPmh(d->m_Pmh);
}

/** \brief Return the PMH::Internal::PmhData pointer modified or not according to the actual EditMode of the viewer. */
Internal::PmhData *PmhViewer::modifiedPmhData() const
{
    // Read only == return the unchanged PmhData
    if (d->m_Mode==ReadOnlyMode) {
        return d->m_Pmh;
    }
    // Apply changes to PmhData
    d->populatePmhWithUi();
    return d->m_Pmh;
}

void PmhViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
