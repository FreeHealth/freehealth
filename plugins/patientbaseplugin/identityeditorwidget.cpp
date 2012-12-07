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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Patients::IdentityEditorWidget
 * \brief Identity editor.
 * This widget allow user to view & edit the identity of a patient.
 * You can define the Patients::PatientModel and the index to use,
 * or just keep the identity always sync with the Core::IPatient current
 * patient.
*/

#include "identityeditorwidget.h"
#include "patientmodel.h"
#include "patientbase.h"
#include "patientcore.h"
#include "constants_db.h"

#include "ui_identitywidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/iphotoprovider.h>
#include <patientbaseplugin/constants_settings.h>

#include <zipcodesplugin/zipcodescompleters.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/uppercasevalidator.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>

#include <QDir>
#include <QFileDialog>
#include <QDateEdit>

#include <QDebug>
#include <QMenu>

using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}

//TODO: Users can add pages in the identity widget using the XMLForm --> create a <Form> named \e Identity

namespace Patients {
namespace Internal {


/**
 * \class Patients::Internal::IsDirtyDataWidgetMapper
 * The data mapper keeps the model original value when you set its current index,
 * then you can use the isDirty() method to make the comparison between cached original
 * values and the widgets current value. \n
 * Note that when the model is submitted, you have to refresh the cache with onModelSubmitted(). \n
 * This make no sense to use this data mapper with a submit policy (setSubmitPolicy())
 * different from QDataWidgetMapper::ManualSubmit.
*/
IsDirtyDataWidgetMapper::IsDirtyDataWidgetMapper(QObject *parent) :
    QDataWidgetMapper(parent)
{
    //TODO: extract this class into Utils?
}

/** Use this method each time the model gets correctly submitted */
void IsDirtyDataWidgetMapper::onModelSubmitted()
{
    refreshCache();
}

/** Return true if the current values of the mapped widget differs from the original model values */
bool IsDirtyDataWidgetMapper::isDirty() const
{
    Q_ASSERT(orientation() == Qt::Horizontal);
    Q_ASSERT(rootIndex() == QModelIndex());

    // cycle through all widgets the mapper supports
    for(int i = 0; i < model()->columnCount(); i++) {
        QWidget *mapWidget = mappedWidgetAt(i);
        if (mapWidget) {
            const QVariant &current = mapWidget->property(mappedPropertyName(mapWidget));
            const QVariant &orig = _original.value(mapWidget);

            // Special case of null original variant
            if (orig.isNull() && current.toString().isEmpty())
                continue;
            if (current != orig) {
//                qWarning() << patient()->enumToString(Core::IPatient::PatientDataRepresentation(i))
//                           << "orig" << orig
//                           << "current" << current;
                return true;
            }
        }
    }
    return false;
}

/** Overload method (creates the internal cache) */
void IsDirtyDataWidgetMapper::setCurrentIndex(int index)
{
    refreshCache();
    QDataWidgetMapper::setCurrentIndex(index);
}

void IsDirtyDataWidgetMapper::refreshCache()
{
    Q_ASSERT(orientation() == Qt::Horizontal);
    Q_ASSERT(rootIndex() == QModelIndex());
    _original.clear();
    // cycle through all widgets the mapper supports
    for(int i = 0; i < model()->columnCount(); i++) {
        QWidget *mapWidget = mappedWidgetAt(i);
        if (mapWidget) {
            _original.insert(mapWidget, model()->data(model()->index(currentIndex(), i)));
        }
    }
}

class IdentityEditorWidgetPrivate
{
public:
    IdentityEditorWidgetPrivate(IdentityEditorWidget *parent, IdentityEditorWidget::EditMode mode) :
        editUi(0),
        m_Mapper(0), m_EditMode(mode),
        m_hasRealPhoto(false),
        q(parent)
    {
        switch (mode) {
        case IdentityEditorWidget::ReadWriteMode: {
            editUi = new Ui::IdentityWidget;
            editUi->setupUi(q);
            editUi->dob->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
            editUi->dob->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEARLINEEDIT));

            editUi->zipcodesWidget->initialize(ZipCodes::ZipCodesWidget::GridLayout);

            editUi->genderCombo->addItems(genders());
            editUi->titleCombo->addItems(titles());

            Utils::UpperCaseValidator *upperVal = new Utils::UpperCaseValidator(q);
            editUi->birthName->setValidator(upperVal);
            editUi->secondName->setValidator(upperVal);

            Utils::CapitalizationValidator *capVal = new Utils::CapitalizationValidator(q);
            editUi->firstname->setValidator(capVal);

            q->connect(editUi->photoButton, SIGNAL(clicked()), q, SLOT(photoButton_clicked()));
            //            q->connect(editUi->genderCombo, SIGNAL(currentIndexChanged(int)), q, SLOT(updateGenderImage()));

            QList<Core::IPhotoProvider *> photoProviderList = pluginManager()->getObjects<Core::IPhotoProvider>();

            if (!photoProviderList.isEmpty()) {
                // sort the PhotoProviders by their priority property - this is done by the IPhotoProvider::operator< and qSort()
                qSort(photoProviderList);

                QAction *photoAction;
                foreach(Core::IPhotoProvider *provider, photoProviderList) {
                    //: which IPhotoProvider to get picture from: from URL, from Webcam, from ...
                    photoAction = new QAction(provider->displayText(), q);
                    q->connect(photoAction, SIGNAL(triggered()), provider, SLOT(startReceivingPhoto()));
                    q->connect(provider, SIGNAL(photoReady(QPixmap)), editUi->photoButton, SLOT(setPixmap(QPixmap)));
                    photoAction->setData(provider->id());
                    editUi->photoButton->addAction(photoAction);
                }
                updateDefaultPhotoAction();

            } else {
                LOG_ERROR_FOR(q, "No photoProvider");
                // buggy: the photo saving does not work ATM!
//                if (editUi->photoButton->pixmap().isNull())
//                    editUi->photoButton->setDisabled(true);
            }
            q->connect(editUi->genderCombo, SIGNAL(currentIndexChanged(int)), q, SLOT(updateGenderImage(int)));
            q->connect(editUi->photoButton->deletePhotoAction(), SIGNAL(triggered()), q, SLOT(updateGenderImage()));
            break;
        }
        case IdentityEditorWidget::ReadOnlyMode: {
            break;
        }
        default:
            break;
        }
    }

    ~IdentityEditorWidgetPrivate()
    {
        if (m_Mapper) {
            delete m_Mapper;
            m_Mapper = 0;
        }
        if (editUi) {
            delete editUi;
            editUi = 0;
        }
    }

    void updateDefaultPhotoAction()
    {
        QString defaultId = settings()->value(Patients::Constants::S_DEFAULTPHOTOSOURCE).toString();
        foreach(QAction *action, editUi->photoButton->actions()) {
            if (action->data().toString() == defaultId)
                editUi->photoButton->setDefaultAction(action);
        }
    }

    // Create the mapper over the Core::IPatient
    void createGenericMapper()
    {
        if (m_EditMode == IdentityEditorWidget::ReadWriteMode) {
            if (m_Mapper) {
                delete m_Mapper;
                m_Mapper = 0;
            }
            m_Mapper = new IsDirtyDataWidgetMapper(q);
            m_Mapper->setSubmitPolicy(IsDirtyDataWidgetMapper::ManualSubmit);
            m_Mapper->setModel(patient());
            addMapperMapping();
        }
    }

    // Create the mapper over a specified PatientModel
    void createPatientModelMapper(PatientModel *model)
    {
        if (m_EditMode == IdentityEditorWidget::ReadWriteMode) {
            if (m_Mapper) {
                delete m_Mapper;
                m_Mapper = 0;
            }
            m_Mapper = new IsDirtyDataWidgetMapper(q);
            m_Mapper->setSubmitPolicy(IsDirtyDataWidgetMapper::ManualSubmit);
            m_Mapper->setModel(model);
            addMapperMapping();
        }
    }

    // Add mapping to the mapper
    void addMapperMapping()
    {
        m_Mapper->addMapping(editUi->birthName, Core::IPatient::BirthName, "text");
        m_Mapper->addMapping(editUi->secondName, Core::IPatient::SecondName, "text");
        m_Mapper->addMapping(editUi->firstname, Core::IPatient::Firstname, "text");
        m_Mapper->addMapping(editUi->genderCombo, Core::IPatient::GenderIndex, "currentIndex");
        m_Mapper->addMapping(editUi->titleCombo, Core::IPatient::TitleIndex, "currentIndex");
        m_Mapper->addMapping(editUi->dob, Core::IPatient::DateOfBirth, "date");

        editUi->zipcodesWidget->addMapping(m_Mapper, Core::IPatient::Street, ZipCodes::ZipCodesWidget::StreetPlainText);
        editUi->zipcodesWidget->addMapping(m_Mapper, Core::IPatient::City, ZipCodes::ZipCodesWidget::CityPlainText);
        editUi->zipcodesWidget->addMapping(m_Mapper, Core::IPatient::ZipCode, ZipCodes::ZipCodesWidget::ZipcodePlainText);
        editUi->zipcodesWidget->addMapping(m_Mapper, Core::IPatient::StateProvince, ZipCodes::ZipCodesWidget::StateProvincePlainText);
        //FIXME: buggy: country widget has FR(,DE,AT,...) as value while model holds a NULL this prevents m_Mapper.isDirty from working correctly!
        editUi->zipcodesWidget->addMapping(m_Mapper, Core::IPatient::Country, ZipCodes::ZipCodesWidget::CountryIso);
    }

public:
    Ui::IdentityWidget *editUi;
    IsDirtyDataWidgetMapper *m_Mapper;
    Patients::PatientModel *m_PatientModel;
    IdentityEditorWidget::EditMode m_EditMode;
    QPixmap m_Photo;
    bool m_hasRealPhoto;

private:
    QAction *m_deletePhotoAction;
    IdentityEditorWidget *q;
};

}  // namespace Internal
}  // namespace Patients

/**
 * \brief Create an Identity viewer with the specific \e mode of edition.
 * You must specify the PatientModel to use
 * \sa IdentityEditorWidget::EditMode, IdentityEditorWidget::setCurrentPatientModel
*/
IdentityEditorWidget::IdentityEditorWidget(QWidget *parent, EditMode mode) :
    QWidget(parent),
    d(new Internal::IdentityEditorWidgetPrivate(this, mode))
{
    setObjectName("Patient::IdentityEditorWidget");
    d->createGenericMapper();
    d->m_Mapper->toFirst();
    updateGenderImage();
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
}

IdentityEditorWidget::~IdentityEditorWidget()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * If you don't want to use the identity editor over the Core::IPatient
 * (which represents the current patient), you can set your own Patients::PatientModel.
 * The mapper is auto-selecting the first row of the model.\n
 * Use the setCurrentIndex() to set the current index of the current editing index.
 * \sa setCurrentIndex()
 */
void IdentityEditorWidget::setPatientModel(PatientModel *model)
{
    d->createPatientModelMapper(model);
    d->m_Mapper->toFirst();
    updateGenderImage();
}

/**
 * If you don't want to use the identity editor over the Core::IPatient
 * (which represents the current patient), you can set your own Patients::PatientModel.
 * Use the setPatientModel() and setCurrentIndex() to set the current index of
 * the current editing index.
 * \sa setPatientModel()
 */
void IdentityEditorWidget::setCurrentIndex(const QModelIndex &patientIndex)
{
    if (patientIndex.model() == d->m_Mapper->model())
        d->m_Mapper->setCurrentModelIndex(patientIndex);
}

/** \brief Test the validity of the "actually shown" identity. */
bool IdentityEditorWidget::isIdentityValid() const
{
    if (d->editUi->birthName->text().isEmpty()) {
        Utils::warningMessageBox(tr("You must specify a birthname."),
                                 tr("You can not create a patient without a birthname"),
                                 "", tr("No birthname"));
        d->editUi->birthName->setFocus();
        return false;
    }
    if (d->editUi->firstname->text().isEmpty()) {
        Utils::warningMessageBox(tr("You must specify a first name."),
                                 tr("You can not create a patient without a first name"),
                                 "", tr("No firstname"));
        d->editUi->firstname->setFocus();
        return false;
    }
    if (d->editUi->dob->date().isNull()) {
        Utils::warningMessageBox(tr("You must specify a date of birth."),
                                 tr("You can not create a patient without a date of birth"),
                                 "", tr("No date of birth"));
        d->editUi->dob->setFocus();
        return false;
    }
    if (d->editUi->genderCombo->currentIndex() == -1) {
        Utils::warningMessageBox(tr("You must specify a gender."),
                                 tr("You can not create a patient without a gender"),
                                 "", tr("No gender"));
        d->editUi->genderCombo->setFocus();
        return false;
    }
    return true;
}

/** \brief Avoid duplicates. */
bool IdentityEditorWidget::isIdentityAlreadyInDatabase() const
{
    // check database for double entries
    QString where = QString("`%1`='%2' AND ").
            arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME),
                d->editUi->birthName->text());
    if (!d->editUi->secondName->text().isEmpty())
        where += QString("`%1`='%2' AND ").
                arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_SECONDNAME),
                    d->editUi->secondName->text());
    where += QString("`%1`='%2'").
            arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_FIRSTNAME),
                d->editUi->firstname->text());
    return (patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME, where)>0);
}

/** \brief Identity has been modified by the user? */
bool IdentityEditorWidget::isModified() const
{
    if (d->m_EditMode==ReadOnlyMode)
        return false;
    // TODO: return right value
    return d->m_Mapper->isDirty();
}

/** Return the current editing value */
QString IdentityEditorWidget::currentBirthName() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!
    return d->editUi->birthName->text();
}

/** Return the current editing value */
QString IdentityEditorWidget::currentSecondName() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!
    return d->editUi->secondName->text();
}

/** Return the current editing value */
QString IdentityEditorWidget::currentFirstName() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!
    switch (d->m_EditMode) {
    case ReadOnlyMode:
        return QString(); //FIXME: return right value
    case ReadWriteMode:
        return d->editUi->firstname->text();
    }
    return QString();
}

/** Return the current editing value */
QString IdentityEditorWidget::currentGender() const
{
    int genderIndex = -1;

    switch (d->m_EditMode) {
    case ReadOnlyMode: {
        //        we must query the model here because the viewUi doesn't provide a good input here.
//        const QModelIndex index = d->m_PatientModel->currentPatient();
//        if (!index.isValid())
//            return QString();

//        genderIndex = d->m_PatientModel->index(index.row(), Core::IPatient::GenderIndex).data().toInt();
        genderIndex = patient()->data(Core::IPatient::GenderIndex).toInt();
        break;
    }
    case ReadWriteMode:
        genderIndex = d->editUi->genderCombo->currentIndex();
    } // switch

    if (genderIndex > 0 && genderIndex < genders().count())
        return genders()[genderIndex];

    return QString();
}

/** Return the current editing value */
QDate IdentityEditorWidget::currentDateOfBirth() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode);
    return d->editUi->dob->date();
}

/*!
 * \brief Returns current widget photo of patient
 *
 * \returns QPixmap current widget photo of patient.
 * If patient has no photo in the current widget (this function does NOT query the database!),
 * it returns a QPixmap()
 */
QPixmap IdentityEditorWidget::currentPhoto() const
{
    QPixmap photo;

    switch (d->m_EditMode) {
    case ReadWriteMode:
        photo = hasPhoto() ? d->editUi->photoButton->pixmap() : QPixmap();
        break;
    default: break;
    }
    return photo;
}

bool IdentityEditorWidget::hasPhoto() const
{
    Q_ASSERT(d->editUi);
    return (!d->editUi->photoButton->pixmap().isNull());
}

/** \brief Submit the Identity to the model and the database if in ReadWriteMode. */
bool IdentityEditorWidget::submit()
{
    if ((d->m_EditMode == ReadWriteMode) && d->m_Mapper) {
        bool ok = d->m_Mapper->submit();
        if (ok) {
            d->m_Mapper->onModelSubmitted();
            patientCore()->refreshAllPatientModel();
        }
    }
    return false;
}

void IdentityEditorWidget::updateGenderImage()
{
    updateGenderImage(d->editUi->genderCombo->currentIndex());
}

void IdentityEditorWidget::updateGenderImage(int genderIndex)
{
    switch(d->m_EditMode) {
    case ReadWriteMode:
        d->editUi->photoButton->setGenderImage(genderIndex);
        break;
    default: break;
    }
}

void IdentityEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (d->editUi)
            d->editUi->retranslateUi(this);
        break;
    default:
        break;
    }
}

/** \brief Triggers the default action of the photo button. */
void IdentityEditorWidget::photoButton_clicked()
{
    QAction *action = d->editUi->photoButton->defaultAction();
    if (action)
        action->trigger();
}

/** Force UI to update with the new current patient data */
void IdentityEditorWidget::onCurrentPatientChanged()
{
    d->m_Mapper->setCurrentModelIndex(QModelIndex());
    d->m_Mapper->setCurrentModelIndex(patient()->currentPatientIndex());
    updateGenderImage();
}
