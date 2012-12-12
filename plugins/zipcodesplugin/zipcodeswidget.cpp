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
 * \class ZipCodes::ZipCodesWidget
 * Address widget with
 * - street,
 * - country,
 * - state/province,
 * - city
 * - zipcode
 * - zipcode/city/state/province auto-completer.
 */

#include "zipcodeswidget.h"
#include "zipcodescompleters.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/widgets/countrycombobox.h>
#include <utils/widgets/uppercasevalidator.h>
#include <utils/widgets/qbuttonlineedit.h>
#include <translationutils/constants.h>

#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QDataWidgetMapper>

#include <QDebug>

using namespace ZipCodes;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace ZipCodes {
namespace Internal {
class ZipCodesWidgetPrivate
{
public:
    ZipCodesWidgetPrivate(ZipCodesWidget *parent) :
        _grid(0),
        _form(0),
        _cityLabel(0),
        _countryLabel(0),
        _zipLabel(0),
        _stateLabel(0),
        _streetLabel(0),
        _street(0),
        _city(0),
        _zip(0),
        _stateCombo(0),
        _country(0),
        _zipCompleter(0),
        q(parent)
    {
    }
    
    ~ZipCodesWidgetPrivate()
    {
    }

    void createLayout(ZipCodesWidget::TypeOfLayout type)
    {
        _typeOfLayout = type;
        if (_typeOfLayout==ZipCodesWidget::FormLayout) {
            _form = new QFormLayout(q);
            q->setLayout(_form);
        } else {
            _grid = new QGridLayout(q);
            q->setLayout(_grid);
        }
    }

    void createWidgetsAndObjects()
    {
        _cityLabel = new QLabel(q);
        _countryLabel = new QLabel(q);
        _stateLabel = new QLabel(q);
        _zipLabel = new QLabel(q);
        _streetLabel = new QLabel(q);

        _street = new QTextEdit(q);
        _city = new Utils::QButtonLineEdit(q);
        _zip = new Utils::QButtonLineEdit(q);
        _stateCombo = new QComboBox(q);
        _country = new Utils::CountryComboBox(q);
        _country->setFlagPath(settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/");
        _country->initialize();

        _zipCompleter = new ZipCodes::ZipCountryCompleters(q);
        _zipCompleter->setCityLineEdit(_city);
        _zipCompleter->setZipLineEdit(_zip);
        _zipCompleter->setCountryComboBox(_country);
        _zipCompleter->setStateProvinceComboBox(_stateCombo);
    }

    void populateLayout()
    {
        if (_typeOfLayout==ZipCodesWidget::FormLayout) {
            _form->addRow(_cityLabel, _city);
            _form->addRow(_stateLabel, _stateCombo);
            _form->addRow(_zipLabel, _zip);
            _form->addRow(_countryLabel, _country);
        } else {
            int row = 0;
            _grid->addWidget(_streetLabel, row, 0, 1, 1);
            _grid->addWidget(_street, row, 1, 1, 1);
            ++row;
            _grid->addWidget(_cityLabel, row, 0, 1, 1);
            _grid->addWidget(_city, row, 1, 1, 1);
            ++row;
            _grid->addWidget(_stateLabel, row, 0, 1, 1);
            _grid->addWidget(_stateCombo, row, 1, 1, 1);
            ++row;
            _grid->addWidget(_zipLabel, row, 0, 1, 1);
            _grid->addWidget(_zip, row, 1, 1, 1);
            ++row;
            _grid->addWidget(_countryLabel, row, 0, 1, 1);
            _grid->addWidget(_country, row, 1, 1, 1);
        }
    }

    void setTabOrder()
    {
        QWidget::setTabOrder(_street, _city);
        QWidget::setTabOrder(_city, _stateCombo);
        QWidget::setTabOrder(_stateCombo, _zip);
        QWidget::setTabOrder(_zip, _country);
    }
    
public:
    QGridLayout *_grid;
    QFormLayout *_form;
    QLabel *_cityLabel, *_countryLabel, *_zipLabel, *_stateLabel, *_streetLabel;
    QTextEdit *_street;
    Utils::QButtonLineEdit *_city, *_zip;
    QComboBox *_stateCombo;
    Utils::CountryComboBox *_country;
    ZipCountryCompleters *_zipCompleter;
    ZipCodesWidget::TypeOfLayout _typeOfLayout;

private:
    ZipCodesWidget *q;
};
}  // namespace Internal
} // end namespace ZipCodes

/*! Constructor of the ZipCodes::ZipCodesWidget class. Object must be initialize with a specific layout */
ZipCodesWidget::ZipCodesWidget(QWidget *parent) :
    QWidget(parent),
    d(new ZipCodesWidgetPrivate(this))
{
}

/*! Destructor of the ZipCodes::ZipCodesWidget class */
ZipCodesWidget::~ZipCodesWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ZipCodesWidget::initialize(TypeOfLayout layout)
{
    d->createLayout(layout);
    d->createWidgetsAndObjects();
    d->populateLayout();
    d->setTabOrder();
    return true;
}

/** Add mapping to a QDataWidgetMapper for a specific \e section and a specific \e Mapping */
void ZipCodesWidget::addMapping(QDataWidgetMapper *mapper, const int section, Mapping mapping) const
{
    switch (mapping) {
    case StreetPlainText: mapper->addMapping(d->_street, section, "plainText"); break;
    case CityPlainText: mapper->addMapping(d->_city, section, "text"); break;
    case CountryIso: mapper->addMapping(d->_country, section, "currentIsoCountry"); break;
    case CountryLocale: mapper->addMapping(d->_country, section, "currentCountry"); break;
    case StateProvincePlainText: mapper->addMapping(d->_stateCombo, section, "currentText"); break;
    case ZipcodePlainText: mapper->addMapping(d->_zip, section, "text"); break;
    }
}

/** Retranslate the UI */
void ZipCodesWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        d->_streetLabel->setText(tr("Street"));
        d->_cityLabel->setText(tr("City"));
        d->_countryLabel->setText(tr("Country"));
        d->_stateLabel->setText(tr("State/Province"));
        d->_zipLabel->setText(tr("Zipcode"));
    }
}
