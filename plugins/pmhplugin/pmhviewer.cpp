/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Authors:                                                               *
 *  Eric MAEKER <eric.maeker@gmail.com>                                    *
 *  Jerome PINGUET <jerome@jerome.cc                                       *
 ***************************************************************************/
/**
  \class PMH::PmhViewer
  \brief PMHx viewer widget. Allow to show / modify a PMH.
  This class uses PMH::Internal::PmhData pointers to create the view. It is not
  mapped to the model. You must:
  - set the PMH::Internal::PmhData to use
  - get it back after the edition
  - then send it to the PMH::PmhCategoryModel
  UI form pmhviewer.ui uses QDateEditEx class, a subclass of QDateEdit that
  manages null dates. Use Utils::QDateEditEx as classname in ui form file.
*/

#include "pmhviewer.h"
#include "pmhdata.h"
#include "constants.h"
#include "pmhepisodemodel.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <icdplugin/icdcollectiondialog.h>
#include <icdplugin/icdio.h>

#include <categoryplugin/categoryonlyproxymodel.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>

#include "ui_pmhviewer.h"

#include <QStringListModel>

// TODO: this is old fashion code, ui and object class are manually linked inside
// the widget. Instead, we should use a PmhModel and use a QDataWidgetMapper.

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline PMH::PmhCore *pmhCore() { return PMH::PmhCore::instance(); }


namespace PMH {
namespace Internal {

class PmhViewerPrivate {
public:
    PmhViewerPrivate(PmhViewer *parent) :
        ui(0),
        m_Pmh(0),
        q(parent)
    {
        m_ShowPatient = patient() != 0;
    }

    ~PmhViewerPrivate()
    {
        delete ui; ui=0;
    }

    void setEditMode(PmhViewer::EditMode mode)
    {
        m_Mode = mode;
        Q_ASSERT(ui);
        bool enable = (mode == PmhViewer::ReadWriteMode);
        q->setEnabled(enable);
    }

    void populateUiWithPmh(PmhData *pmh)
    {
        m_Pmh = pmh;
        ui->personalLabel->setText(pmh->data(PmhData::Label).toString());
        ui->userNameLabel->setText(user()->fullNameOfUser(pmh->data(PmhData::UserOwner)));
        QString sCDT = pmh->data(PmhData::CreationDateTime).toString();
        // If there is no creation date, don't display "on" QLabel
        ui->creationDateTimeOn->setVisible(!sCDT.isEmpty());
        QDateTime creationDateTimeUtc = QDateTime::fromString(sCDT, Qt::ISODate);
        QDateTime CDTL = creationDateTimeUtc.toLocalTime();
        QDate CDL = CDTL.date();
        QString sCDL = CDL.toString(Qt::TextDate);
        ui->creationDateTimeLabel->setText(sCDL);
        ui->creationDateTimeLabel->setToolTip(CDTL.toString());
        ui->typeCombo->setCurrentIndex(pmh->data(PmhData::Type).toInt());
        ui->statusCombo->setCurrentIndex(pmh->data(PmhData::State).toInt());
        ui->confIndexSlider->setValue(pmh->data(PmhData::ConfidenceIndex).toInt());
        ui->makePrivateBox->setChecked(pmh->data(PmhData::IsPrivate).toBool());
        ui->comment->setHtml(pmh->data(PmhData::Comment).toString());
        // Get category
        QModelIndex cat = pmhCore()->pmhCategoryModel()->indexForCategory(pmh->category());
        cat = pmhCore()->pmhCategoryModel()->categoryOnlyModel()->mapFromSource(cat);
        ui->categoryTreeView->setCurrentIndex(cat);

        // ExtendedView -> Populate EpisodeView
        ui->episodeViewer->setPmhData(pmh);

        // SimpleView -> Populate SimpleGroup widgets
        ui->simple_start_date->clear();
        ui->simple_end_date->clear();
        m_IcdLabelModel->setStringList(QStringList());
        if (pmh->episodeModel()->rowCount()) {
            // TODO: improve this
            // use only the first row
            ui->simple_start_date->setDate(pmh->episodeModel()->index(0, PmhEpisodeModel::DateStart).data().toDate());
            ui->simple_end_date->setDate(pmh->episodeModel()->index(0, PmhEpisodeModel::DateEnd).data().toDate());
            m_IcdLabelModel->setStringList(pmh->episodeModel()->index(0, PmhEpisodeModel::IcdLabelStringList).data().toStringList());
        }

        // Populate Link viewer

        // Manage ICD availability
        ui->simple_icd10->setEnabled(ICD::IcdIO::isDatabaseInitialized());
    }

    void populatePmhWithUi()
    {
        m_Pmh->setData(PmhData::Label, ui->personalLabel->text());
        m_Pmh->setData(PmhData::Type, ui->typeCombo->currentIndex());
        m_Pmh->setData(PmhData::State, ui->statusCombo->currentIndex());
        m_Pmh->setData(PmhData::ConfidenceIndex, ui->confIndexSlider->value());
        m_Pmh->setData(PmhData::Comment, ui->comment->textEdit()->toHtml());
        m_Pmh->setData(PmhData::IsPrivate, ui->makePrivateBox->isChecked());
        // Get category
        QModelIndex cat = pmhCore()->pmhCategoryModel()->categoryOnlyModel()->mapToSource(ui->categoryTreeView->currentIndex());
        cat = pmhCore()->pmhCategoryModel()->index(cat.row(), PmhCategoryModel::Id, cat.parent());
        m_Pmh->setData(PmhData::CategoryId, cat.data().toInt());
        // TODO: improve this : pmhx only manages one episode
        if (m_Pmh->episodeModel()->rowCount() == 0) {
            m_Pmh->episodeModel()->insertRow(0);
        }
        // use only the first row
        m_Pmh->episodeModel()->setData(m_Pmh->episodeModel()->index(0, PmhEpisodeModel::DateStart), ui->simple_start_date->date());
        m_Pmh->episodeModel()->setData(m_Pmh->episodeModel()->index(0, PmhEpisodeModel::DateEnd), ui->simple_end_date->date());
        m_Pmh->episodeModel()->setData(m_Pmh->episodeModel()->index(0, PmhEpisodeModel::IcdLabelStringList), m_IcdLabelModel->stringList());
        m_Pmh->episodeModel()
                ->setData(m_Pmh->episodeModel()
                          ->index(0, PmhEpisodeModel::Comment), ui->comment->textEdit()->toHtml());
    }

    void clearUi()
    {
        ui->personalLabel->clear();
        ui->typeCombo->clear();
        ui->statusCombo->clear();
        ui->categoryTreeView->setModel(0);
        ui->episodeViewer->clear();
        // Manage ICD availability
        ui->simple_icd10->setEnabled(ICD::IcdIO::isDatabaseInitialized());
    }

public:
    Internal::Ui::PmhViewer *ui;
    PmhViewer::EditMode m_Mode;
    PmhViewer::ViewMode m_ViewMode;
    PmhData *m_Pmh;
    bool m_ShowPatient;
    QStringListModel *m_IcdLabelModel;

private:
    PmhViewer *q;
};

} // End namespace Internal
} // End namespace PMH


/** \brief Creates a new PMH::PmhViewer with the specified \e editMode.
 *
 * ViewMode can be set to SimpleMode or ExtendedMode (with widgetBar:
 * Episodes, Management, Contacts, Links, Comment)
 * By default ViewMode is set to ExtendedMode.
 * tabWidget is hidden for now until we fully implement episode management.
 * \sa PMH::PmhViewer::EditMode
*/
PmhViewer::PmhViewer(QWidget *parent, EditMode editMode, ViewMode viewMode) :
    QWidget(parent), d(new PmhViewerPrivate(this))
{
    // Create Ui
    d->ui = new Internal::Ui::PmhViewer;
    d->ui->setupUi(this);
    // QDateEditEx
    d->ui->simple_start_date->setNullable(true);
    d->ui->simple_start_date->setCalendarPopup(true);
    d->ui->simple_start_date->setDate(QDate()); // Set date as null with QDate()
    d->ui->simple_end_date->setNullable(true);
    d->ui->simple_end_date->setCalendarPopup(true);
    d->ui->simple_end_date->setDate(QDate()); // Set date as null with QDate()
    d->ui->simple_icd10->setEnabled(ICD::IcdIO::isDatabaseInitialized());
    d->m_IcdLabelModel = new QStringListModel(this);

    // Populate combos
    d->ui->typeCombo->addItems(Constants::availableTypes());
    d->ui->statusCombo->addItems(Constants::availableStatus());

    // adjust tabwidget
    d->ui->tabWidget->setCurrentWidget(d->ui->episodesTab);
    d->ui->comment->toogleToolbar(true);

    // Manage the Edit Mode
    d->setEditMode(editMode);
    setPatientInfoVisible(d->m_ShowPatient);

    // Manage View Mode
    d->m_ViewMode = viewMode;
    if (viewMode==ExtendedMode) {
        connect(d->ui->personalLabel, SIGNAL(textChanged(QString)), this, SLOT(onSimpleViewLabelChanged(QString)));
        d->ui->tabWidget->setTabEnabled(0, false); // Episodes tab disabled
        d->ui->tabWidget->setTabEnabled(1, false); // Management tab disabled
        d->ui->tabWidget->setTabEnabled(2, false); // Contacts tab disabled
        d->ui->tabWidget->setTabEnabled(3, false); // Links tab disabled
        d->ui->tabWidget->hide();
        d->ui->icdCodes->setModel(d->m_IcdLabelModel);
    } else {  // SimpleMode
        d->ui->tabWidget->hide();
        d->ui->icdCodes->setModel(d->m_IcdLabelModel);
        connect(d->ui->personalLabel, SIGNAL(textChanged(QString)), this, SLOT(onSimpleViewLabelChanged(QString)));
    }

    // add models to views
    d->ui->categoryTreeView->setModel(pmhCore()->pmhCategoryModel()->categoryOnlyModel());
    d->ui->categoryTreeView->expandAll();
    connect(pmhCore()->pmhCategoryModel()->categoryOnlyModel(), SIGNAL(layoutChanged()),
            d->ui->categoryTreeView, SLOT(expandAll()));
    connect(d->ui->simple_icd10, SIGNAL(clicked()), this, SLOT(onSimpleViewIcdClicked()));
}

PmhViewer::~PmhViewer()
{
    delete d;
    d = 0;
}

/** \brief Show or hide the patient information (title, name) inside the dialog. */
void PmhViewer::setPatientInfoVisible(bool visible)
{
    QString text;
    if (visible) {
        text = QString("%1 %2").arg(
                    patient()->data(Core::IPatient::FullName).toString(),
                    patient()->data(Core::IPatient::DateOfBirth).toString());
    } else {
        text = tkTr(Trans::Constants::PASTMEDICALHISTORY);
    }
    d->ui->titleLabel->setText(text);
}

/** Defines the edit mode to use (Read only or Read Write). \sa PMH::PmhViewer::EditMode */
void PmhViewer::setEditMode(EditMode mode)
{
    d->ui->simple_icd10->setEnabled(ICD::IcdIO::isDatabaseInitialized());
    d->setEditMode(mode);
}

/** \brief Define the PMH::Internal::PmhData pointer to use in the view. */
void PmhViewer::setPmhData(Internal::PmhData *pmh)
{
    if (!pmh) {
        d->m_Pmh = 0;
    }
    if (d->m_Pmh) {
        if (d->m_Pmh == pmh)
            return;
    }
    d->populateUiWithPmh(pmh);
}

void PmhViewer::setCategoryForPmh(Category::CategoryItem *category)
{
    Q_ASSERT(d->m_Pmh);
    if (!d->m_Pmh)
        return;
    d->m_Pmh->setCategory(category);
    // update ui
    // Get category
    QModelIndex cat = pmhCore()->pmhCategoryModel()->indexForCategory(d->m_Pmh->category());
    cat = pmhCore()->pmhCategoryModel()->categoryOnlyModel()->mapFromSource(cat);
    d->ui->categoryTreeView->setCurrentIndex(cat);
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

void PmhViewer::onSimpleViewIcdClicked()
{
    // create an ICD10 collection dialog
    ICD::IcdCollectionDialog dlg(this);
    // get the XML ICD10 coding
    PmhEpisodeModel *model = d->m_Pmh->episodeModel();
    QString xml = model->index(0, PmhEpisodeModel::IcdXml).data(Qt::EditRole).toString();
//    LOG(xml);
    dlg.setXmlIcdCollection(xml);
    if (dlg.exec()==QDialog::Accepted) {
        // retrieve selected codes to the PmhEpisodeModel
        d->m_Pmh->episodeModel()->setData(model->index(0, PmhEpisodeModel::IcdXml), dlg.xmlIcdCollection());
        // update the icdCodesLineEdit
        const QStringList &icdLabels = d->m_Pmh->episodeModel()->index(0, PmhEpisodeModel::IcdLabelStringList).data().toStringList();
        d->m_IcdLabelModel->setStringList(icdLabels);
        // if pmh does not have a label -> populate label with ICD labels
        if (d->m_Pmh->data(PmhData::Label).toString().isEmpty() && d->ui->personalLabel->text().isEmpty()) {
            d->m_Pmh->setData(PmhData::Label, icdLabels.join(", "));
            d->ui->personalLabel->setText(d->m_Pmh->data(PmhData::Label).toString());
        }
    }
}

void PmhViewer::onSimpleViewLabelChanged(const QString &text)
{
    d->m_Pmh->episodeModel()->setData(d->m_Pmh->episodeModel()->index(0, PmhEpisodeModel::Label), text);
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
