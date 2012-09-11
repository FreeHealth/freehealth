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
 * \class Patients::Internal::IdentityViewerWidget
 * \brief The identity viewer make the coagulum between the Patients::PatientModel data and the patient form's data.
 *
 * The viewer is not dependent of the Core::IPatient. It manages it's own mecanism
 * to get the correct information about patient between what is extracted from the
 * patient model and what is extracted form the patient form's data.
 *
 * \sa Form::FormItemSpec::Spec_IsIdentityForm, Form::FormManager::identityRootForm(), Core::IPatient, Core::ICore::patient()
 */


#include "identityviewerwidget.h"

#include <patientbaseplugin/patientmodel.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/iformitem.h>

#include <coreplugin/ipatient.h>

#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_titles.h>
#include <translationutils/trans_user.h>

#include "ui_identityviewer.h"

#include <QFormLayout>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
class SimpleIdentityWidget : public QWidget
{
public:
    SimpleIdentityWidget(QWidget *parent) : QWidget(parent)
    {
        _grid = new QFormLayout(this);
        _grid->setMargin(0);
        setLayout(_grid);
        _birthName = new QLabel(this);
        _birthNameLabel = new QLabel(this);
        _secondName = new QLabel(this);
        _secondNameLabel = new QLabel(this);
        _firstName = new QLabel(this);
        _firstNameLabel = new QLabel(this);
        _gender = new QLabel(this);
        _genderLabel = new QLabel(this);
        _title = new QLabel(this);
        _titleLabel = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _birthNameLabel->setFont(bold);
        _secondNameLabel->setFont(bold);
        _firstNameLabel->setFont(bold);
        _genderLabel->setFont(bold);
        _titleLabel->setFont(bold);

        _grid->addRow(_titleLabel, _title);
        _grid->addRow(_birthNameLabel, _birthName);
        _grid->addRow(_secondNameLabel, _secondName);
        _grid->addRow(_firstNameLabel, _firstName);
        _grid->addRow(_genderLabel, _gender);

        retranslate();
    }

    void clear()
    {
        _birthName->clear();
        _secondName->clear();
        _firstName->clear();
        _gender->clear();
        _title->clear();
    }

    void setBirthName(const QString &txt)
    {
        _birthName->setText(txt);
    }

    void setSecondName(const QString &txt)
    {
        if (txt.isEmpty()) {
            _secondName->setVisible(false);
            _secondNameLabel->setVisible(false);
            _grid->removeWidget(_secondName);
            _grid->removeWidget(_secondNameLabel);
        } else {
            _grid->insertRow(2, _secondNameLabel, _secondName);
            _secondName->setVisible(true);
            _secondNameLabel->setVisible(true);
            _secondName->setText(txt);
        }
    }

    void setFirstName(const QString &txt)
    {
        _firstName->setText(txt);
    }

    void setGender(const QString &txt)
    {
        _gender->setText(txt);
    }

    void setTitle(const QString &txt)
    {
        _title->setText(txt);
    }

    void retranslate()
    {
        _birthNameLabel->setText(tkTr(Trans::Constants::BIRTHNAME));
        _secondNameLabel->setText(tkTr(Trans::Constants::SECONDNAME));
        _firstNameLabel->setText(tkTr(Trans::Constants::FIRSTNAME));
        _genderLabel->setText(tkTr(Trans::Constants::GENDER));
        _titleLabel->setText(tkTr(Trans::Constants::TITLE));
    }

    void changeEvent(QEvent *e)
    {
        if (e->type()==QEvent::LanguageChange)
            retranslate();
        QWidget::changeEvent(e);
    }

private:
    QFormLayout *_grid;
    QLabel *_title, *_titleLabel;
    QLabel *_birthName, *_birthNameLabel;
    QLabel *_secondName, *_secondNameLabel;
    QLabel *_firstName, *_firstNameLabel;
    QLabel *_gender, *_genderLabel;
};

class AgeViewerWidget : public QWidget
{
public:
    AgeViewerWidget(QWidget *parent) : QWidget(parent)
    {
        QFormLayout *grid = new QFormLayout(this);
        grid->setMargin(0);
        setLayout(grid);
        _age = new QLabel(this);
        _ageLabel = new QLabel(this);
        _dob = new QLabel(this);
        _dobLabel = new QLabel(this);
        _dod = new QLabel(this);
        _dodLabel = new QLabel(this);
        _prof = new QLabel(this);
        _profLabel = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _ageLabel->setFont(bold);
        _dobLabel->setFont(bold);
        _dodLabel->setFont(bold);
        _profLabel->setFont(bold);

        grid->addRow(_ageLabel, _age);
        grid->addRow(_dobLabel, _dob);
        grid->addRow(_dodLabel, _dod);
        grid->addRow(_profLabel, _prof);

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

    void setProfession(const QString &txt)
    {
        if (txt.isEmpty())
            _prof->setText("--");
        else
            _prof->setText(txt);
    }

    void retranslate()
    {
        _ageLabel->setText(tkTr(Trans::Constants::AGE));
        _dobLabel->setText(tkTr(Trans::Constants::DATE_OF_BIRTH));
        _dodLabel->setText(tkTr(Trans::Constants::DATE_OF_DEATH));
        _profLabel->setText(tkTr(Trans::Constants::PROFESSION));
    }

    void changeEvent(QEvent *e)
    {
        if (e->type()==QEvent::LanguageChange)
            retranslate();
        QWidget::changeEvent(e);
    }

private:
    QLabel *_age, *_ageLabel, *_dob, *_dobLabel, *_dodLabel, *_dod;
    QLabel *_prof, *_profLabel;
};

class IdentityAndAgeDetailsWidget : public QWidget
{
public:
    IdentityAndAgeDetailsWidget(QWidget *parent) : QWidget(parent)
    {
        QVBoxLayout *vboxIdent = new QVBoxLayout;
        _identity = new SimpleIdentityWidget(this);
        vboxIdent->addWidget(_identity);
        vboxIdent->addStretch(100);

        QVBoxLayout *vboxAge = new QVBoxLayout;
        _age = new AgeViewerWidget(this);
        vboxAge->addWidget(_age);
        vboxAge->addStretch(100);

        QHBoxLayout *hbox = new QHBoxLayout(this);
        hbox->addLayout(vboxIdent);
        hbox->addLayout(vboxAge);
        setLayout(hbox);
    }

    SimpleIdentityWidget *identity() const {return _identity;}
    AgeViewerWidget *age() const {return _age;}

private:
    SimpleIdentityWidget *_identity;
    AgeViewerWidget *_age;
};

class AddressViewerWidget : public QWidget
{
public:
    AddressViewerWidget(QWidget *parent) : QWidget(parent)
    {
        QFormLayout *grid = new QFormLayout(this);
        grid->setMargin(0);
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

class ContactViewerWidget : public QWidget
{
public:
    ContactViewerWidget(QWidget *parent) : QWidget(parent)
    {
        QFormLayout *grid = new QFormLayout(this);
        grid->setMargin(0);
        setLayout(grid);
        _telsLabel = new QLabel(this);
        _faxLabel = new QLabel(this);
        _mailLabel = new QLabel(this);
        _mobileLabel = new QLabel(this);
        _tels = new QLabel(this);
        _fax = new QLabel(this);
        _mail = new QLabel(this);
        _mobile = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _telsLabel->setFont(bold);
        _faxLabel->setFont(bold);
        _mailLabel->setFont(bold);
        _mobileLabel->setFont(bold);

        grid->addRow(_telsLabel, _tels);
        grid->addRow(_faxLabel, _fax);
        grid->addRow(_mailLabel, _mail);
        grid->addRow(_mobileLabel, _mobile);

        retranslate();
    }

    void clear()
    {
        _tels->clear();
        _fax->clear();
        _mail->clear();
        _mobile->clear();
        _tels->clear();
    }

    void setTels(const QString &txt)
    {
        if (txt.isEmpty())
            _tels->setText("--");
        else
            _tels->setText(txt);
    }

    void setFax(const QString &txt)
    {
        if (txt.isEmpty())
            _fax->setText("--");
        else
            _fax->setText(txt);
    }

    void setMail(const QString &txt)
    {
        if (txt.isEmpty())
            _mail->setText("--");
        else
            _mail->setText(txt);
    }

    void setMobile(const QString &txt)
    {
        if (txt.isEmpty())
            _mobile->setText("--");
        else
            _mobile->setText(txt);
    }

    void retranslate()
    {
        _telsLabel->setText(tkTr(Trans::Constants::TELS));
        _faxLabel->setText(tkTr(Trans::Constants::FAX));
        _mailLabel->setText(tkTr(Trans::Constants::MAIL));
        _mobileLabel->setText(tkTr(Trans::Constants::MOBILEPHONE));
    }

    void changeEvent(QEvent *e)
    {
        if (e->type()==QEvent::LanguageChange)
            retranslate();
        QWidget::changeEvent(e);
    }

private:
    QLabel *_telsLabel, *_faxLabel, *_mailLabel, *_mobileLabel;
    QLabel *_tels, *_fax, *_mail, *_mobile;

};

class FullContactViewerDetails : public QWidget
{
public:
    FullContactViewerDetails(QWidget *parent) : QWidget(parent)
    {
        QVBoxLayout *vboxAd = new QVBoxLayout;
        _address = new AddressViewerWidget(this);
        vboxAd->addWidget(_address);
        vboxAd->addStretch(100);

        QVBoxLayout *vboxContact = new QVBoxLayout;
        _contact = new ContactViewerWidget(this);
        vboxContact->addWidget(_contact);
        vboxContact->addStretch(100);

        QHBoxLayout *hbox = new QHBoxLayout(this);
        hbox->addLayout(vboxAd);
        hbox->addLayout(vboxContact);
        setLayout(hbox);
    }

    void clear() {_address->clear(); _contact->clear();}
    AddressViewerWidget *address() const {return _address;}
    ContactViewerWidget *contact() const {return _contact;}

private:
    AddressViewerWidget *_address;
    ContactViewerWidget *_contact;
};

}  // end anonymous namespace

namespace Patients {
namespace Internal {
class IdentityViewerWidgetPrivate
{
public:
    IdentityViewerWidgetPrivate(IdentityViewerWidget *parent) :
        viewUi(0),
        m_IdentityDetails(0),
        m_IdentityWidget(0),
        m_AgeWidget(0),
        m_FullContactWidget(0),
        m_PatientModel(0),
        q(parent)
    {
        viewUi = new Ui::IdentityViewer;
        viewUi->setupUi(q);
        m_IdentityDetails = new IdentityAndAgeDetailsWidget(q);
        m_IdentityWidget = m_IdentityDetails->identity();
        m_AgeWidget = m_IdentityDetails->age();
        m_FullContactWidget = new FullContactViewerDetails(q);
        viewUi->identityDetails->setWidget(m_IdentityDetails);
        viewUi->addressDetails->setWidget(m_FullContactWidget);
    }
    
    ~IdentityViewerWidgetPrivate()
    {
        if (viewUi) {
            delete viewUi;
            viewUi = 0;
        }
    }

    void clearReadOnlyWidget()
    {
        Q_ASSERT(viewUi);
        viewUi->photoLabel->clear();
        m_IdentityWidget->clear();
        m_AgeWidget->clear();
        m_FullContactWidget->clear();
    }

    void populateReadOnlyWidget(const int row)
    {
        // photo
        const QPixmap &photo = m_PatientModel->index(row, Core::IPatient::Photo_64x64).data().value<QPixmap>();
        viewUi->photoLabel->setPixmap(photo);

        // name && gender
        const QIcon &icon = m_PatientModel->index(row, Core::IPatient::IconizedGender).data().value<QIcon>();
        const QString &name = m_PatientModel->index(row, Core::IPatient::FullName).data().toString();
        viewUi->identityDetails->setIcon(icon);
        viewUi->identityDetails->setSummaryFontBold(true);
        m_IdentityWidget->clear();
        m_IdentityWidget->setBirthName(m_PatientModel->index(row, Core::IPatient::BirthName).data().toString());
        m_IdentityWidget->setSecondName(m_PatientModel->index(row, Core::IPatient::SecondName).data().toString());
        m_IdentityWidget->setFirstName(m_PatientModel->index(row, Core::IPatient::Firstname).data().toString());
        m_IdentityWidget->setGender(m_PatientModel->index(row, Core::IPatient::Gender).data().toString());
        m_IdentityWidget->setTitle(m_PatientModel->index(row, Core::IPatient::Title).data().toString());

        // age / dob / dod / prof
        const QString &age = m_PatientModel->index(row, Core::IPatient::Age).data().toString();
        QString dob = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfBirth).data().toDate(), QLocale::LongFormat);
        QString prof = m_PatientModel->index(row, Core::IPatient::Profession).data().toString();
        viewUi->identityDetails->setSummaryText(QString("%1 - %2").arg(name).arg(age));
        m_AgeWidget->clear();
        m_AgeWidget->setAge(age);
        m_AgeWidget->setDateOfBirth(dob);
        m_AgeWidget->setProfession(prof);
        if (m_PatientModel->index(row, Core::IPatient::DateOfDeath).data().isValid()) {
            QString dod = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfDeath).data().toDate(), QLocale::LongFormat);
            m_AgeWidget->setDateOfDeath(dod);
        }

        // address
        // TODO: add a preference -> what to show in summarytext: mobile phone? address? tels? email?
        viewUi->addressDetails->setSummaryText(m_PatientModel->index(row, Core::IPatient::FullAddress).data().toString());
        m_FullContactWidget->clear();
        m_FullContactWidget->address()->setAddress(m_PatientModel->index(row, Core::IPatient::Street).data().toString());
        m_FullContactWidget->address()->setCity(m_PatientModel->index(row, Core::IPatient::City).data().toString());
        m_FullContactWidget->address()->setZipCode(m_PatientModel->index(row, Core::IPatient::ZipCode).data().toString());
        m_FullContactWidget->address()->setCountry(m_PatientModel->index(row, Core::IPatient::Country).data().toString());
        m_FullContactWidget->contact()->setTels(m_PatientModel->index(row, Core::IPatient::Tels).data().toString());
        m_FullContactWidget->contact()->setFax(m_PatientModel->index(row, Core::IPatient::Faxes).data().toString());
        m_FullContactWidget->contact()->setMail(m_PatientModel->index(row, Core::IPatient::Mails).data().toString());
        m_FullContactWidget->contact()->setMobile(m_PatientModel->index(row, Core::IPatient::MobilePhone).data().toString());
    }

public:
    Ui::IdentityViewer *viewUi;
    IdentityAndAgeDetailsWidget *m_IdentityDetails;
    SimpleIdentityWidget *m_IdentityWidget;
    AgeViewerWidget *m_AgeWidget;
    FullContactViewerDetails *m_FullContactWidget;

    Form::FormMain *_identityForm;
    Form::EpisodeModel *_episodeModel;
    PatientModel *m_PatientModel;

private:
    IdentityViewerWidget *q;
};
}  // namespace Internal
} // end namespace Patients

/*! Constructor of the IdentityViewerWidget class */
IdentityViewerWidget::IdentityViewerWidget(QWidget *parent) :
    d(new IdentityViewerWidgetPrivate(this))
{
    setObjectName("Patient::IdentityViewerWidget");
}

/*! Destructor of the IdentityViewerWidget class */
IdentityViewerWidget::~IdentityViewerWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! initializes the IdentityViewerWidget instance with default values */
bool IdentityViewerWidget::initialize()
{
}

/** \brief Define the model to use. */
void IdentityViewerWidget::setCurrentPatientModel(Patients::PatientModel *model)
{
    d->m_PatientModel = model;
}

/** \brief Return the actual PatientModel or 0 if it was not set. */
Patients::PatientModel *IdentityViewerWidget::patientModel() const
{
    return d->m_PatientModel;
}

/** \brief Change the current identity to the QModelIndex \e patientIndex. */
void IdentityViewerWidget::setCurrentIndex(const QModelIndex &patientIndex)
{
    d->clearReadOnlyWidget();
    d->populateReadOnlyWidget(patientIndex.row());
}

//void IdentityWidget::changeEvent(QEvent *e)
//{
//    QWidget::changeEvent(e);
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//        if (d->editUi)
//            d->editUi->retranslateUi(this);
//        break;
//    default:
//        break;
//    }
//}
