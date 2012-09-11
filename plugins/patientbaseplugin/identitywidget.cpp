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
  \class Patients::IdentityWidget
  \brief Presents the identity of a patient.
*/

#include "identitywidget.h"
#include "patientmodel.h"
#include "patientbase.h"
#include "constants_db.h"

#include "ui_identitywidget.h"
#include "ui_identityviewer.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/iphotoprovider.h>

#include <zipcodesplugin/zipcodescompleters.h>

#include <utils/global.h>
#include <utils/widgets/uppercasevalidator.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <extensionsystem/pluginmanager.h>

#include <QDataWidgetMapper>
#include <QDir>
#include <QFileDialog>
#include <QDateEdit>
#include <QTextBrowser>
#include <QDebug>

#include <pixmapdelegate.h>

using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}

//TODO: Users can add pages in the identity widget using the XMLForm --> create a <Form> named \e Identity

namespace Patients {
namespace Internal {

//TODO: extract this class into Utils?

// Thanks to Davor Josipovic for the custom DataWidgetWrapper code,
// found at http://davor.no-ip.com/blog/2010/05/11/isdirty-class-member-for-qdatawidgetmapper/

/** \brief wrapper function for QDataWidgetMapper with isDirty() method*/
class FMFWidgetMapper: public QDataWidgetMapper
{
public:
    FMFWidgetMapper(QObject *parent = 0) :
        QDataWidgetMapper(parent)
    {}

    bool isDirty() const {
        //TODO: support both orientations
        Q_ASSERT(orientation() == Qt::Horizontal);
        Q_ASSERT(rootIndex() == QModelIndex());

        //        qDebug() << "FMWidgetmapper.isDirty() called:";

        // cycle through all widgets the mapper supports
        for(int i = 0; i < model()->columnCount(); i++) {
            QWidget *mapWidget = mappedWidgetAt(i);
            if (mapWidget){
                QByteArray p = mappedPropertyName(mapWidget);
                QModelIndex idx = model()->index(currentIndex(), i);

//                qDebug() << mapWidget->objectName() << "DB:" << idx.data(Qt::EditRole) << "- Widget value:" << mapWidget->property(p);

                QVariant data = idx.data(Qt::EditRole);
                //                qDebug(mapWidget->metaObject()->className());

                // special case: QDateEdit can not display NULL value. so compare here manually
                if (mapWidget->metaObject()->className() == QString("QDateEdit")) {
                    QDateEdit* dateEdit = qobject_cast<QDateEdit*>(mapWidget);
                    if (dateEdit) {
                        //                        qDebug() << data.toDate();
                        //                        qDebug() << dateEdit->date();
                        //                        qDebug() << dateEdit->minimumDate();
                        if (data.toDate() == QDate() && dateEdit->date() != dateEdit->minimumDate()) {
                            return true;
                        }
                    }
                }
                // if data in model != widget's value, data was modified, page is "dirty"
                if (data != mapWidget->property(p))
                    return true;
            }
        }
        return false;
    }
};

class AgeViewerWidget : public QWidget
{
public:
    AgeViewerWidget(QWidget *parent) : QWidget(parent)
    {
        QFormLayout *grid = new QFormLayout(this);
        setLayout(grid);
        _age = new QLabel(this);
        _ageLabel = new QLabel(this);
        _dob = new QLabel(this);
        _dobLabel = new QLabel(this);
        _dod = new QLabel(this);
        _dodLabel = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _ageLabel->setFont(bold);
        _dobLabel->setFont(bold);
        _dodLabel->setFont(bold);

        grid->addRow(_ageLabel, _age);
        grid->addRow(_dobLabel, _dob);
        grid->addRow(_dodLabel, _dod);

        retranslate();
    }

    void clear()
    {
        _age->clear();
        _dob->clear();
        _dod->clear();
        _dod->setVisible(false);
        _dodLabel->setVisible(false);
    }

    void setAge(const QString &age)
    {
        _age->setText(age);
    }

    void setDateOfBirth(const QString &date)
    {
        _dob->setText(date);
    }

    void setDateOfDeath(const QString &date)
    {
        if (!date.isEmpty()) {
            _dod->setText(date);
            _dod->setVisible(true);
            _dodLabel->setVisible(true);
        }
    }

    void retranslate()
    {
        _ageLabel->setText(tkTr(Trans::Constants::AGE));
        _dobLabel->setText(tkTr(Trans::Constants::DATE_OF_BIRTH));
        _dodLabel->setText(tkTr(Trans::Constants::DATE_OF_DEATH));
    }

    void changeEvent(QEvent *e)
    {
        if (e->type()==QEvent::LanguageChange)
            retranslate();
        QWidget::changeEvent(e);
    }

private:
    QLabel *_age, *_ageLabel, *_dob, *_dobLabel, *_dodLabel, *_dod;
};

class ContactViewerWidget : public QWidget
{
public:
    ContactViewerWidget(QWidget *parent) : QWidget(parent)
    {
        QFormLayout *grid = new QFormLayout(this);
        setLayout(grid);
        _addressLabel = new QLabel(this);
        _cityLabel = new QLabel(this);
        _zipLabel = new QLabel(this);
        _countryLabel = new QLabel(this);
        _address = new QLabel(this);
        _city = new QLabel(this);
        _zip = new QLabel(this);
        _country = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _addressLabel->setFont(bold);
        _cityLabel->setFont(bold);
        _zipLabel->setFont(bold);
        _countryLabel->setFont(bold);

        grid->addRow(_addressLabel, _address);
        grid->addRow(_cityLabel, _city);
        grid->addRow(_zipLabel, _zip);
        grid->addRow(_countryLabel, _country);

        retranslate();
    }

    void clear()
    {
        _address->clear();
        _city->clear();
        _zip->clear();
        _country->clear();
    }

    void setAddress(const QString &txt)
    {
        _address->setText(txt);
    }

    void setCity(const QString &txt)
    {
        _city->setText(txt);
    }

    void setZipCode(const QString &txt)
    {
        _zip->setText(txt);
    }

    void setCountry(const QString &txt)
    {
        _country->setText(txt);
    }

    void retranslate()
    {
        _addressLabel->setText(tkTr(Trans::Constants::ADDRESS));
        _cityLabel->setText(tkTr(Trans::Constants::CITY));
        _zipLabel->setText(tkTr(Trans::Constants::ZIPCODE));
        _countryLabel->setText(tkTr(Trans::Constants::COUNTRY));
    }

    void changeEvent(QEvent *e)
    {
        if (e->type()==QEvent::LanguageChange)
            retranslate();
        QWidget::changeEvent(e);
    }

private:
    QLabel *_addressLabel, *_cityLabel, *_zipLabel, *_countryLabel;
    QLabel *_address, *_city, *_zip, *_country;

};


class IdentityWidgetPrivate
{
public:
    IdentityWidgetPrivate(IdentityWidget *parent, IdentityWidget::EditMode mode) :
        editUi(0), viewUi(0),
        m_Mapper(0), m_EditMode(mode),
        zipCompleter(0),
        m_hasRealPhoto(false),
        m_AgeWidget(0),
        m_ContactWidget(0),
        q(parent)
    {
        switch (mode) {
        case IdentityWidget::ReadOnlyMode: {
            viewUi = new Ui::IdentityViewer;
            viewUi->setupUi(q);
            m_AgeWidget = new AgeViewerWidget(q);
            m_ContactWidget = new ContactViewerWidget(q);
            viewUi->ageDetails->setWidget(m_AgeWidget);
            viewUi->addressDetails->setWidget(m_ContactWidget);
            break;
        }
        case IdentityWidget::ReadWriteMode: {
            editUi = new Ui::IdentityWidget;
            editUi->setupUi(q);
            editUi->dob->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
            editUi->dob->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEARLINEEDIT));

            editUi->genderCombo->addItems(genders());
            editUi->titleCombo->addItems(titles());

            Utils::UpperCaseValidator *upperVal = new Utils::UpperCaseValidator(q);
            editUi->birthName->setValidator(upperVal);
            editUi->secondName->setValidator(upperVal);

            Utils::CapitalizationValidator *capVal = new Utils::CapitalizationValidator(q);
            editUi->firstname->setValidator(capVal);

            zipCompleter = new ZipCodes::ZipCountryCompleters(q);
            zipCompleter->setCityLineEdit(editUi->city);
            zipCompleter->setZipLineEdit(editUi->zipcode);
            zipCompleter->setCountryComboBox(editUi->country);

            q->connect(editUi->photoButton, SIGNAL(clicked()), q, SLOT(photoButton_clicked()));
//            q->connect(editUi->genderCombo, SIGNAL(currentIndexChanged(int)), q, SLOT(updateGenderImage()));
            break;
        }
        }
    }

    ~IdentityWidgetPrivate()
    {
        if (m_Mapper) {
            delete m_Mapper;
            m_Mapper = 0;
        }
        if (editUi) {
            delete editUi;
            editUi = 0;
        }
        if (viewUi) {
            delete viewUi;
            viewUi = 0;
        }
    }

    void createMapper()
    {
        if (m_EditMode == IdentityWidget::ReadWriteMode) {
            if (m_Mapper) {
                delete m_Mapper;
                m_Mapper = 0;
            }
            m_Mapper = new FMFWidgetMapper(q);
            m_Mapper->setSubmitPolicy(FMFWidgetMapper::ManualSubmit);
            m_Mapper->setModel(m_PatientModel);
            m_Mapper->setItemDelegate(new PixmapDelegate(q));
            m_Mapper->addMapping(editUi->birthName, Core::IPatient::BirthName, "text");
            m_Mapper->addMapping(editUi->secondName, Core::IPatient::SecondName, "text");
            m_Mapper->addMapping(editUi->firstname, Core::IPatient::Firstname, "text");
            m_Mapper->addMapping(editUi->genderCombo, Core::IPatient::GenderIndex, "currentIndex");
            m_Mapper->addMapping(editUi->titleCombo, Core::IPatient::TitleIndex, "currentIndex");
            m_Mapper->addMapping(editUi->dob, Core::IPatient::DateOfBirth, "date");

            m_Mapper->addMapping(editUi->street, Core::IPatient::Street, "plainText");
            m_Mapper->addMapping(editUi->city, Core::IPatient::City, "text");
            m_Mapper->addMapping(editUi->zipcode, Core::IPatient::ZipCode, "text");
            m_Mapper->addMapping(editUi->photoButton, Core::IPatient::Photo_64x64, "pixmap");

            //FIXME: buggy: country widget has FR(,DE,AT,...) as value while model holds a NULL
            // this prevents m_Mapper.isDirty from working correctly!
            //            m_Mapper->addMapping(editUi->country, Core::IPatient::Country, "currentIsoCountry");
            m_Mapper->toFirst();
        }
    }

    void clearReadOnlyWidget()
    {
        Q_ASSERT(viewUi);
        viewUi->name->clear();
        viewUi->photoLabel->clear();
        viewUi->ageDOB->clear();
        viewUi->sex->clear();
        m_AgeWidget->clear();
        m_ContactWidget->clear();
    }

    void populateReadOnlyWidget(const int row)
    {
        // name && gender
        const QIcon &icon = m_PatientModel->index(row, Core::IPatient::IconizedGender).data().value<QIcon>();
        viewUi->sex->setPixmap(icon.pixmap(QSize(16,16)));
        const QString &name = m_PatientModel->index(row, Core::IPatient::FullName).data().toString();
        viewUi->name->setText(name);

        // photo
        const QPixmap &photo = m_PatientModel->index(row, Core::IPatient::Photo_64x64).data().value<QPixmap>();
        viewUi->photoLabel->setPixmap(photo);

        // age && dob
        const QString &age = m_PatientModel->index(row, Core::IPatient::Age).data().toString();
        QString dob = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfBirth).data().toDate(), QLocale::LongFormat);
        viewUi->ageDetails->setSummaryText(age);
        m_AgeWidget->setAge(age);
        m_AgeWidget->setDateOfBirth(dob);
        if (m_PatientModel->index(row, Core::IPatient::DateOfDeath).data().isValid()) {
            QString dod = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfDeath).data().toDate(), QLocale::LongFormat);
            m_AgeWidget->setDateOfDeath(dod);
        }

        // address
        // TODO: add a preference -> what to show in summarytext: mobile phone? address? tels? email?
        viewUi->addressDetails->setSummaryText(m_PatientModel->index(row, Core::IPatient::FullAddress).data().toString());
        m_ContactWidget->setAddress(m_PatientModel->index(row, Core::IPatient::Street).data().toString());
        m_ContactWidget->setCity(m_PatientModel->index(row, Core::IPatient::City).data().toString());
        m_ContactWidget->setZipCode(m_PatientModel->index(row, Core::IPatient::ZipCode).data().toString());
        m_ContactWidget->setCountry(m_PatientModel->index(row, Core::IPatient::Country).data().toString());
    }

public:
    Ui::IdentityWidget *editUi;
    Ui::IdentityViewer *viewUi;
    FMFWidgetMapper *m_Mapper;
    Patients::PatientModel *m_PatientModel;
    IdentityWidget::EditMode m_EditMode;
    ZipCodes::ZipCountryCompleters *zipCompleter;
    QPixmap m_Photo;
    bool m_hasRealPhoto;
    AgeViewerWidget *m_AgeWidget;
    ContactViewerWidget *m_ContactWidget;

private:
    IdentityWidget *q;
};

}  // namespace Internal
}  // namespace Patients

bool IdentityWidget::hasPhoto() const
{
    return d->m_hasRealPhoto;
}


/**
 * \brief Create an Identity viewer with the specific \e mode of edition.
 * You must specify the PatientModel to use
 * \sa IdentityWidget::EditMode, IdentityWidget::setCurrentPatientModel
*/
IdentityWidget::IdentityWidget(QWidget *parent, EditMode mode) :
    QWidget(parent),
    d(new Internal::IdentityWidgetPrivate(this, mode))
{
    setObjectName("Patient::IdentityWidget");
}

IdentityWidget::~IdentityWidget()
{
    delete d;
}

/** \brief Define the model to use. */
void IdentityWidget::setCurrentPatientModel(Patients::PatientModel *model)
{
    d->m_PatientModel = model;
    if (d->m_EditMode == ReadWriteMode)
        d->createMapper();
}

/** \brief Return the actual PatientModel or 0 if it was not set. */
Patients::PatientModel *IdentityWidget::patientModel() const
{
    return d->m_PatientModel;
}

/** \brief Change the current identity to the QModelIndex \e patientIndex. */
void IdentityWidget::setCurrentIndex(const QModelIndex &patientIndex)
{
    switch (d->m_EditMode) {
    case ReadWriteMode:
    {
        Q_ASSERT(d->m_Mapper);
        d->m_Mapper->setCurrentModelIndex(patientIndex);
        d->zipCompleter->checkData();
        break;
    }
    case ReadOnlyMode:
    {
        //read only mode, no QDataWidgetMapper needed
        d->clearReadOnlyWidget();
        d->populateReadOnlyWidget(patientIndex.row());
        break;
    }
    }  // switch (d->m_EditMode)
}

/** \brief Test the validity of the "actually shown" identity. */
bool IdentityWidget::isIdentityValid() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!
    if (d->editUi->birthName->text().isEmpty()) {
        Utils::warningMessageBox(tr("You must specify a birthname."),
                                 tr("You can not create a patient without a birthname"),
                                 "", tr("No birthname"));
        return false;
    }
    if (d->editUi->firstname->text().isEmpty()) {
        Utils::warningMessageBox(tr("You must specify a first name."),
                                 tr("You can not create a patient without a first name"),
                                 "", tr("No firstname"));
        return false;
    }
    if (d->editUi->dob->date().isNull()) {
        Utils::warningMessageBox(tr("You must specify a date of birth."),
                                 tr("You can not create a patient without a date of birth"),
                                 "", tr("No date of birth"));
        return false;
    }
    if (d->editUi->genderCombo->currentIndex() < 0) {
        Utils::warningMessageBox(tr("You must specify a gender."),
                                 tr("You can not create a patient without a gender"),
                                 "", tr("No gender"));
        return false;
    }
    return true;
}

/** \brief Avoid duplicates. */
bool IdentityWidget::isIdentityAlreadyInDatabase() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!

    // check database for double entries
    QString where = QString("`%1`='%2' AND ").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME), d->editUi->birthName->text());
    if (!d->editUi->secondName->text().isEmpty())
        where += QString("`%1`='%2' AND ").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_SECONDNAME), d->editUi->secondName->text());
    where += QString("`%1`='%2'").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_FIRSTNAME), d->editUi->firstname->text());
    return (patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME, where)>0);
}

/** \brief Identity has been modified by the user? */
bool IdentityWidget::isModified() const
{
    if (d->m_EditMode==ReadOnlyMode)
        return false;
    //TODO: return right value
    return d->m_Mapper->isDirty();
}

/** Return the current editing value */
QString IdentityWidget::currentBirthName() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!
    return d->editUi->birthName->text();
}

/** Return the current editing value */
QString IdentityWidget::currentSecondName() const
{
    Q_ASSERT(d->m_EditMode == ReadWriteMode); //FIXME!
    return d->editUi->secondName->text();
}

/** Return the current editing value */
QString IdentityWidget::currentFirstName() const
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
QString IdentityWidget::currentGender() const
{
    int genderIndex = -1;

    switch (d->m_EditMode) {

    case ReadOnlyMode: {
//        we must query the model here because the viewUi doesn't provide a good input here.
        const QModelIndex index = d->m_PatientModel->currentPatient();
        if (!index.isValid())
            return QString();

        genderIndex = d->m_PatientModel->index(index.row(), Core::IPatient::GenderIndex).data().toInt();
        break;
    }
    case ReadWriteMode:
        genderIndex = d->editUi->genderCombo->currentIndex();
        break;
    }

    switch (genderIndex) {
    case 0: return "M";
    case 1: return "F";
    case 2: return "H";
    }
    return QString();
}


/** Return the current editing value */
QDate IdentityWidget::currentDateOfBirth() const
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
QPixmap IdentityWidget::currentPhoto() const
{
    QPixmap photo;

    switch (d->m_EditMode) {
    case ReadWriteMode:
        photo = d->m_hasRealPhoto ? d->editUi->photoButton->pixmap() : QPixmap();
        break;
    case ReadOnlyMode:
        photo = d->m_hasRealPhoto ? *d->viewUi->photoLabel->pixmap() : QPixmap();
        break;
    }
    return photo;
}

/** \brief Submit the Identity to the model and the database if in ReadWriteMode. */
bool IdentityWidget::submit()
{
    if ((d->m_EditMode == ReadWriteMode) && d->m_Mapper) {
        return d->m_Mapper->submit();
    }
    return false;
}

//void IdentityWidget::updateGenderImage()
//{
//    switch(d->m_EditMode) {
//    case ReadWriteMode:
//        Q_ASSERT(d->m_Mapper);

//        // check if photoButton has a real pixmap ATM
//        if (d->editUi->photoButton->pixmap().isNull()) {

//            // if not, set default gendered icon
//            setDefaultGenderPhoto();
//        }
//        break;
//    case ReadOnlyMode:
//        if (!d->viewUi->photoLabel->pixmap()) {
//            setDefaultGenderPhoto();
//        }
//    }

//}

void IdentityWidget::changeEvent(QEvent *e)
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

/** \brief Patient's Photograph selector. */
void IdentityWidget::photoButton_clicked()
{
    if (d->m_EditMode != ReadWriteMode)
        return;

    QPixmap photo;

    // TODO: right click: menu -> "delete photo" (or how to do better?)
    // TODO: if a photo is already there -> ask user what to do

    // get a list of plugin implementations that provide a photo
    QList<Core::IPhotoProvider *> photoProviders = ExtensionSystem::PluginManager::instance()->getObjects<Core::IPhotoProvider>();

    if (!photoProviders.isEmpty()) { // call the plugin

        // TODO: implement code to allow having more than one photoProvider plugins
        // and configurations to select the default one.
        // by now just get first plugin

        Core::IPhotoProvider *photoProvider = photoProviders.first();
        photo = photoProvider->recievePhoto();
    } else {   // if no plugins installed/active, fall back to default behaviour

        QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a photo"),
                                                        QDir::homePath(),
                                                        "Image (*.png *.jpg *.gif *.tiff)");
        if (fileName.isEmpty()) {
            return;
        }
        photo.load(fileName);
    }

    if (photo.isNull()) {
//        setDefaultGenderPhoto();
        d->m_hasRealPhoto = false;
        return;
    }

    // resize pixmap
    photo = photo.scaled(QSize(64,64), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    d->m_hasRealPhoto = true;

    // change button pixmap
    d->editUi->photoButton->setPixmap(photo);
}

///*!
// * \brief Sets a default gender pixmap as photo, regardless if there is already a photo.
// */
//void IdentityWidget::setDefaultGenderPhoto()
//{
////    const QModelIndex patientIndex = d->m_PatientModel->currentPatient();
////    if (patientIndex != QModelIndex()) {
////        if (d->m_PatientModel->index(patientIndex.row(), Core::IPatient::Photo_64x64) != QModelIndex())
////            // no photo was saved yet in database!
////            return;
////    }

//    // get current gender from genderCombo (NOT from database, only Widget here!)
//    const QString gender = currentGender();

//    QIcon icon;
//    if (gender == "F") {
//        icon = QIcon(QPixmap(theme()->iconFullPath(Core::Constants::ICONFEMALE, Core::ITheme::BigIcon)));
//    } else if (gender == "H") {
//        icon = QIcon(QPixmap(theme()->iconFullPath(Core::Constants::ICONHERMAPHRODISM , Core::ITheme::BigIcon)));
//    } else  if (gender == "M") {
//        icon = QIcon(QPixmap(theme()->iconFullPath(Core::Constants::ICONMALE, Core::ITheme::BigIcon)));
//    } else
//        icon = QIcon();
//    //    set an empty underlying pixmap, but set the displayed button icon to the default placeholder icon
//    d->editUi->photoButton->setPixmap(QPixmap());
//    d->editUi->photoButton->setIcon(icon);
//}

