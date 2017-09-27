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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/*!
 * \class Patients::Internal::IdentityViewerWidget
 * \brief The identity viewer makes the coagulum between the Patients::PatientModel identity data and the patient form's data.
 *
 * The viewer is not dependent on the Core::IPatient. It manages it's own mechanism
 * to get the correct information about the patient between what is extracted from the
 * patient model and what is extracted form the patient form's data.
 *
 * \sa Form::FormItemSpec::Spec_IsIdentityForm, Form::FormManager::identityRootForm(), Core::IPatient, Core::ICore::patient()
 */

#include "identityviewerwidget.h"

#include <patientbaseplugin/patientmodel.h>
#include <patientbaseplugin/constants_settings.h>

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/emailvalidator.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_titles.h>
#include <translationutils/trans_user.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_current.h>

#include "ui_identityviewer.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QUrl>
#include <QDesktopServices>
#include <QStringList>

#include <QDebug>

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

namespace {
class SimpleIdentityWidget : public QWidget
{
public:

    /*! \brief Display patient identity data
     *
     *  Used by IdentityAndAgeDetailsWidget (and indirectly by IdentityViewerWidget
     *  and IdentityViewerWidgetPrivate) to display title, names, gender of the
     *  selected patient under the patients' list.
     */
    SimpleIdentityWidget(QWidget *parent) : QWidget(parent)
    {
        _grid = new QFormLayout(this);
        _grid->setMargin(0);
        setLayout(_grid);
        _usualName = new QLabel(this);
        _usualNameLabel = new QLabel(this);
        _otherNames = new QLabel(this);
        _otherNamesLabel = new QLabel(this);
        _firstName = new QLabel(this);
        _firstNameLabel = new QLabel(this);
        _gender = new QLabel(this);
        _genderLabel = new QLabel(this);
        _title = new QLabel(this);
        _titleLabel = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _usualNameLabel->setFont(bold);
        _otherNamesLabel->setFont(bold);
        _firstNameLabel->setFont(bold);
        _genderLabel->setFont(bold);
        _titleLabel->setFont(bold);

        _grid->addRow(_titleLabel, _title);
        _grid->addRow(_usualNameLabel, _usualName);
        _grid->addRow(_otherNamesLabel, _otherNames);
        _grid->addRow(_firstNameLabel, _firstName);
        _grid->addRow(_genderLabel, _gender);

        retranslate();
    }

    void clear()
    {
        _usualName->clear();
        _otherNames->clear();
        _firstName->clear();
        _gender->clear();
        _title->clear();
    }

    void setUsualName(const QString &txt)
    {
        _usualName->setText(txt);
    }

    void setOtherNames(const QString &txt)
    {
        if (txt.isEmpty()) {
            _otherNames->setVisible(false);
            _otherNamesLabel->setVisible(false);
            _grid->removeWidget(_otherNames);
            _grid->removeWidget(_otherNamesLabel);
        } else {
            _grid->insertRow(2, _otherNamesLabel, _otherNames);
            _otherNames->setVisible(true);
            _otherNamesLabel->setVisible(true);
            _otherNames->setText(txt);
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
        _usualNameLabel->setText(tkTr(Trans::Constants::USUALNAME));
        _otherNamesLabel->setText(tkTr(Trans::Constants::OTHERNAMES));
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
    QLabel *_usualName, *_usualNameLabel;
    QLabel *_otherNames, *_otherNamesLabel;
    QLabel *_firstName, *_firstNameLabel;
    QLabel *_gender, *_genderLabel;
};

class AgeViewerWidget : public QWidget
{
public:
    /*! \brief Display patient demographics data
     *
     *  Used by IdentityAndAgeDetailsWidget (and indirectly by IdentityViewerWidget
     *  and IdentityViewerWidgetPrivate) to display age, DoB, social number,
     *  occupation (profession) of the selected patient under the patients' list.
     */
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
        _nss = new QLabel(this);
        _nssLabel = new QLabel(this);

        QFont bold;
        bold.setBold(true);
        _ageLabel->setFont(bold);
        _dobLabel->setFont(bold);
        _dodLabel->setFont(bold);
        _profLabel->setFont(bold);
        _nssLabel->setFont(bold);

        grid->addRow(_ageLabel, _age);
        grid->addRow(_dobLabel, _dob);
        grid->addRow(_dodLabel, _dod);
        grid->addRow(_nssLabel, _nss);
        grid->addRow(_profLabel, _prof);

        retranslate();
    }

    void clearAll()
    {
        _age->clear();
        _dob->clear();
        _dod->clear();
        _dod->setVisible(false);
        _dodLabel->setVisible(false);
        _nss->clear();
        _prof->clear();
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

    void setSocialNumber(const QString &txt)
    {
        if (txt.isEmpty())
            _nss->setText("--");
        else
            _nss->setText(txt);
    }

    void retranslate()
    {
        _ageLabel->setText(tkTr(Trans::Constants::AGE));
        _dobLabel->setText(tkTr(Trans::Constants::DATE_OF_BIRTH));
        _dodLabel->setText(tkTr(Trans::Constants::DATE_OF_DEATH));
        _nssLabel->setText(tkTr(Trans::Constants::SOCIAL_NUMBER));
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
    QLabel *_nss, *_nssLabel;
};

class IdentityAndAgeDetailsWidget : public QWidget
{
public:
    /*! \brief Display patient identity & demographics data
     *
     *  Used by IdentityViewerWidgetPrivate and indirectly by IdentityViewerWidget
     *  to display title, names, gender, age, DoB, Social number and Profession
     *  of the selected patient under the patients' list.
     */
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
    Q_OBJECT
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
        _sendMail = new QToolButton(this);
        _mobile = new QLabel(this);

        QHBoxLayout *mailLayout = new QHBoxLayout(this);
        mailLayout->addWidget(_mail);
        mailLayout->addWidget(_sendMail);

        QFont bold;
        bold.setBold(true);
        _telsLabel->setFont(bold);
        _faxLabel->setFont(bold);
        _mailLabel->setFont(bold);
        _mobileLabel->setFont(bold);
        _sendMail->setToolButtonStyle(Qt::ToolButtonIconOnly);
        _sendMail->setIcon(theme()->icon(Core::Constants::ICONNEWMAIL));
        QObject::connect(_sendMail, SIGNAL(clicked()), this, SLOT(sendMail()));

        grid->addRow(_telsLabel, _tels);
        grid->addRow(_faxLabel, _fax);
        grid->addRow(_mailLabel, mailLayout);
        grid->addRow(_mobileLabel, _mobile);

        retranslate();
    }

    void clear()
    {
        _tels->clear();
        _fax->clear();
        _mail->clear();
        _mobile->clear();
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
        Utils::EmailValidator val;
        int pos = 0;
        QString _m = txt;
        _sendMail->setEnabled(val.validate(_m, pos) == Utils::EmailValidator::Acceptable);
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
        _sendMail->setToolTip(tkTr(Trans::Constants::SENDMAIL));
    }

    void changeEvent(QEvent *e)
    {
        if (e->type()==QEvent::LanguageChange)
            retranslate();
        QWidget::changeEvent(e);
    }

private Q_SLOTS:
    void sendMail()
    {
        if (_mail->text().contains("@"))
            QDesktopServices::openUrl(QUrl(QString("mailto:%1?subject=[%2]")
                                           .arg(_mail->text())
                                           .arg(user()->value(Core::IUser::FullName).toString())));
    }

private:
    QLabel *_telsLabel, *_faxLabel, *_mailLabel, *_mobileLabel;
    QLabel *_tels, *_fax, *_mail, *_mobile;
    QToolButton *_sendMail;
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

class PatientModelIdentityWrapper
{
public:
    PatientModelIdentityWrapper() :
        _patientModel(0),
        _identityForm(0),
        _episodeModel(0),
        _current(-1)
    {
    }

    ~PatientModelIdentityWrapper()
    {
        if (_episodeModel)
            delete _episodeModel;
        _episodeModel = 0;
    }

    void setPatientModel(PatientModel *model) {_patientModel = model;}
    void setIdentityForm(Form::FormMain *form)
    {
        _identityForm = form;
        if (form) {
            // Create a cache to speed up the wrapper
            foreach(Form::FormItem *item, _identityForm->flattenedFormItemChildren()) {
                if (item->itemData() && item->patientDataRepresentation() != -1) {
                    _formItemWithData.insert(item->patientDataRepresentation(), item);
                }
            }
        }
    }

    void setEpisodeModel(Form::EpisodeModel *model)
    {
        // create the episode model from the identity form
        if (_episodeModel) {
            delete _episodeModel;
            _episodeModel = 0;
        }
        _episodeModel = model;
        if (_episodeModel)
            _episodeModel->setUseFormContentCache(false);
    }

    void setCurrentPatient(const int row)
    {
        _current = row;
        // Populate the form duplicate with the episode content
        if (_episodeModel) {
            _episodeModel->setCurrentPatient(_patientModel->index(_current, Core::IPatient::Uid).data().toString());
            _episodeModel->populateFormWithEpisodeContent(_episodeModel->index(0,0), false);
        }
    }

    /*!
     * \brief Returns patient data with given \e column from current patient.
     *
     * This function first queries the PatientModel, if there is found a value, it is returned.
     * If not, it searches for the column in the episode data and returns a valid value.
     * If nothing is found, a QVariant() is returned.
     */
    QVariant data(const int iPatientColumn) const
    {
        // get data from the patient model (value is extracted from database)
        QVariant val = _patientModel->data(_patientModel->index(_current, iPatientColumn));
        if (val.isValid() && !val.toString().isEmpty())
            return val;

        // get data from the episode
        if (_episodeModel) {
            Form::FormItem *item = _formItemWithData.value(iPatientColumn, 0);
            if (item)
                return item->itemData()->data(item->patientDataRepresentation(), Form::IFormItemData::PatientModelRole);
        }
        return QVariant();
    }

private:
    PatientModel *_patientModel;
    Form::FormMain *_identityForm;
    Form::EpisodeModel *_episodeModel;
    QHash<int, Form::FormItem *> _formItemWithData;
    int _current;
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
        _identityForm(0),
        _episodeModel(0),
        m_PatientModel(0),
        _patientModelIdentityWrapper(0),
        q(parent)
    {
        // Manage Ui
        viewUi = new Ui::IdentityViewer;
        viewUi->setupUi(q);
        m_IdentityDetails = new IdentityAndAgeDetailsWidget(q);
        m_IdentityWidget = m_IdentityDetails->identity();
        m_AgeWidget = m_IdentityDetails->age();
        m_FullContactWidget = new FullContactViewerDetails(q);
        viewUi->identityDetails->setWidget(m_IdentityDetails);
        viewUi->addressDetails->setWidget(m_FullContactWidget);

        // Manage Model
        _patientModelIdentityWrapper = new PatientModelIdentityWrapper;
    }

    ~IdentityViewerWidgetPrivate()
    {
        if (viewUi) {
            delete viewUi;
            viewUi = 0;
        }
        if (_patientModelIdentityWrapper) {
            delete _patientModelIdentityWrapper;
            _patientModelIdentityWrapper = 0;
        }
    }

    void clearReadOnlyWidget()
    {
        Q_ASSERT(viewUi);
        viewUi->photoLabel->clear();
        m_IdentityWidget->clear();
        m_AgeWidget->clearAll();
        m_FullContactWidget->clear();
    }

    void populateReadOnlyWidget(const int row)
    {
        QString space = QString::fromUtf8("\u2003");
        QString emdash = QString::fromUtf8("\u2014");
        QString separator = space + emdash + space;

        _patientModelIdentityWrapper->setCurrentPatient(row);
        QPixmap photo = m_PatientModel->index(row, Core::IPatient::Photo_64x64).data().value<QPixmap>();
        if (photo.isNull()) {
            const int gender = m_PatientModel->index(row, Core::IPatient::GenderIndex).data().toInt();
            photo = theme()->defaultGenderPixmap(gender);
        }
        viewUi->photoLabel->setPixmap(photo);

        // name / gender
        const QIcon &icon = m_PatientModel->index(row, Core::IPatient::IconizedGender).data().value<QIcon>();
        const QString &name = m_PatientModel->index(row, Core::IPatient::FullName).data().toString();
        viewUi->identityDetails->setIcon(icon);
        viewUi->identityDetails->setSummaryFontBold(true);
        m_IdentityWidget->clear();
        m_IdentityWidget->setUsualName(m_PatientModel->index(row, Core::IPatient::UsualName).data().toString());
        m_IdentityWidget->setOtherNames(m_PatientModel->index(row, Core::IPatient::OtherNames).data().toString());
        m_IdentityWidget->setFirstName(m_PatientModel->index(row, Core::IPatient::Firstname).data().toString());
        m_IdentityWidget->setGender(m_PatientModel->index(row, Core::IPatient::Gender).data().toString());
        m_IdentityWidget->setTitle(m_PatientModel->index(row, Core::IPatient::Title).data().toString());

        // age / dob / dod / prof / nss
        QString age;
        if (m_PatientModel->index(row, Core::IPatient::YearsOld).data().toInt() < settings()->value(Patients::Constants::S_PEDIATRICSAGELIMIT, 18).toInt()) {
            age = m_PatientModel->index(row, Core::IPatient::Age).data().toString();
        } else {
            int yearsOld = m_PatientModel->index(row, Core::IPatient::YearsOld).data().toInt();
            int n = yearsOld;
            age = QString(QObject::tr("%n year(s)", "", n));
        }
        QString dobShort = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfBirth).data().toDate(), QLocale::ShortFormat);
        QString dobLong = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfBirth).data().toDate(), QLocale::LongFormat);
        if (!name.isEmpty()) {
            viewUi->identityDetails
                    ->setSummaryText(QString("%1%2%3%2%4").arg(name).arg(separator).arg(dobShort).arg(age));
        } else {
            viewUi->identityDetails->setSummaryText("");
        }
        m_AgeWidget->clearAll();
        m_AgeWidget->setAge(age);
        m_AgeWidget->setDateOfBirth(dobLong);
        m_AgeWidget->setProfession(_patientModelIdentityWrapper->data(Core::IPatient::Profession).toString());
        QStringList nss;
        nss << _patientModelIdentityWrapper->data(Core::IPatient::SocialNumber).toString()
            << _patientModelIdentityWrapper->data(Core::IPatient::SocialNumber2).toString()
            << _patientModelIdentityWrapper->data(Core::IPatient::SocialNumber3).toString()
            << _patientModelIdentityWrapper->data(Core::IPatient::SocialNumber4).toString();
        nss.removeAll("");
        m_AgeWidget->setSocialNumber(nss.join("; "));
        if (m_PatientModel->index(row, Core::IPatient::DateOfDeath).data().isValid()) {
            QString dod = QLocale().toString(m_PatientModel->index(row, Core::IPatient::DateOfDeath).data().toDate(), QLocale::LongFormat);
            m_AgeWidget->setDateOfDeath(dod);
        }

        // address
        // TODO: add a preference -> what to show in summarytext: mobile phone? address? tels? email?

        QString fulladdress = m_PatientModel
                ->index(row, Core::IPatient::FullAddress).data().toString();
        QString mobile = _patientModelIdentityWrapper
                ->data(Core::IPatient::MobilePhone).toString();
        QString tel = _patientModelIdentityWrapper
                ->data(Core::IPatient::Tels).toString();
        QString protel = _patientModelIdentityWrapper
                          ->data(Core::IPatient::WorkPhone).toString();
        QString email = _patientModelIdentityWrapper
                ->data(Core::IPatient::Email).toString();
        QStringList detailsList;
        detailsList << fulladdress << mobile << tel << protel << email;

        if (detailsList.length() > 1) {
            for (int i = 1; i < detailsList.length(); ++i) {
                if (!detailsList.at(i).isEmpty())
                    detailsList[i].prepend(separator);
            }
        }
        QString details = detailsList.join("");
        if (details.isEmpty())
            viewUi->addressDetails->setSummaryText(tkTr(Trans::Constants::NO_ADDRESS_RECORDED));
        else
            viewUi->addressDetails->setSummaryText(details);
        viewUi->addressDetails->setSummaryFontBold(true);
        m_FullContactWidget->clear();
        m_FullContactWidget->address()->setAddress(_patientModelIdentityWrapper->data(Core::IPatient::Street).toString());
        m_FullContactWidget->address()->setCity(_patientModelIdentityWrapper->data(Core::IPatient::City).toString());
        m_FullContactWidget->address()->setZipCode(_patientModelIdentityWrapper->data(Core::IPatient::ZipCode).toString());
        m_FullContactWidget->address()->setCountry(_patientModelIdentityWrapper->data(Core::IPatient::Country).toString());
        m_FullContactWidget->contact()->setTels(_patientModelIdentityWrapper->data(Core::IPatient::Tels).toString());
        m_FullContactWidget->contact()->setFax(_patientModelIdentityWrapper->data(Core::IPatient::Faxes).toString());
        m_FullContactWidget->contact()->setMail(_patientModelIdentityWrapper->data(Core::IPatient::Email).toString());
        m_FullContactWidget->contact()->setMobile(_patientModelIdentityWrapper->data(Core::IPatient::MobilePhone).toString());
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

    PatientModelIdentityWrapper *_patientModelIdentityWrapper;

private:
    IdentityViewerWidget *q;
};
}  // namespace Internal
} // end namespace Patients

/*! Constructor of the IdentityViewerWidget class */
IdentityViewerWidget::IdentityViewerWidget(QWidget *parent) :
    QWidget(parent),
    d(new IdentityViewerWidgetPrivate(this))
{
    setObjectName("Patient::IdentityViewerWidget");
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(getPatientForms()));
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
    return true;
}

void IdentityViewerWidget::getPatientForms()
{
    // Get the inerte identity form from formmanager
    // This form can be populated by any value without any modification of the
    // current patient data
    Form::FormMain *form = formManager().identityRootForm();
    if (form) {
        d->_patientModelIdentityWrapper->setIdentityForm(form);
        d->_patientModelIdentityWrapper->setEpisodeModel(new Form::EpisodeModel(form, this));
    } else {
        d->_patientModelIdentityWrapper->setIdentityForm(0);
        d->_patientModelIdentityWrapper->setEpisodeModel(0);
    }
}

/** \brief Define the model to use. */
void IdentityViewerWidget::setCurrentPatientModel(Patients::PatientModel *model)
{
    d->m_PatientModel = model;
    d->_patientModelIdentityWrapper->setPatientModel(model);
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

#include "identityviewerwidget.moc"
