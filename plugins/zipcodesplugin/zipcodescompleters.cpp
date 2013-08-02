/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class ZipCodes::Internal::ZipCountryModel
 * Private model used by the ZipCountryCompleters. It is a kind of proxy model for a QSqlModel
 * that provides easy access for zipcodes, city, province and country data.
 */

/**
 * \class ZipCodes::ZipCountryCompleters
 * The ZipCountryCompleters is an object that eases the input of zipcodes and city names. It
 * works together with a zipcode database. You just have to set the zipcode and city
 * QLineEdit with setZipLineEdit() and setCityLineEdit(). Users will get a completer
 * for the zipcode and the city. \n
 * When the user will select a zipcode or a city name, both line edit will be updated
 * with the selected data.
*/

#include "zipcodescompleters.h"
#include "zipcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>

#include <QLineEdit>
#include <QSqlTableModel>
#include <QLocale>
#include <QCompleter>
#include <QApplication>
#include <QListView>
#include <QDir>
#include <QString>
#include <QComboBox>
#include <QToolButton>
#include <QSqlQuery>

#include <QDebug>

using namespace ZipCodes;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline ZipCodes::ZipCore &zipCore() {return ZipCodes::ZipCore::instance();}

namespace {
const char* const SELECT_IMPORT = "SELECT `ZIP`, `CITY`, `ADMIN_NAME1` FROM `IMPORT`";
const char* const SELECT_PROV_IMPORT = "SELECT DISTINCT `ADMIN_NAME1` FROM `IMPORT`";
const char* const SELECT_LIMIT  = "LIMIT 0, 50";
        const int ZIP_COL = 0;
        const int CITY_COL = 1;
        const int PROVINCE_COL = 2;
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////   ZipCountryModel   /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ZipCountryModel::ZipCountryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    setObjectName("ZipCountryModel");
}

QVariant ZipCountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        // translate ColumnRepresentation values into real columns of the database
        switch (index.column()) {
        case Zip:
            return QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::ZIP_COL));
        case City:
            return QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::CITY_COL));
        case Country:
            return QLocale::countryToString(QLocale(QSqlQueryModel::data(QSqlQueryModel::index(index.row(), Country)).toString()).country());
        case Province:
            return QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::PROVINCE_COL));
        case ZipCity:
        {
            const QString &zip = QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::ZIP_COL)).toString();
            const QString &city = QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::CITY_COL)).toString();
            //: %1 = zip code, %2 = city
            return QString(tr("%1, %2").arg(zip, city));
        }
        case CityZip:
        {
            const QString &zip = QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::ZIP_COL)).toString();
            const QString &city = QSqlQueryModel::data(QSqlQueryModel::index(index.row(), ::CITY_COL)).toString();
            //: %1 = city, %2 = zip
            return QString(tr("%1, %2").arg(city, zip));
        }
        } // switch
    }
    return QVariant();
}

/** Return all available countries included in the zipcode database */
bool ZipCountryModel::isCountryAvailable(const QLocale::Country country) const
{
    if (!zipCore().isDatabaseAvailable())
        return false;

    QString iso = Utils::countryToIso(country).toUpper();
    if (iso.isEmpty())
        return false;
    QString req = QString("SELECT DISTINCT COUNT(`COUNTRY`) FROM `IMPORT` WHERE `COUNTRY`=\"%1\"")
            .arg(iso);
    QSqlQuery query(zipCore().database());
    if (query.exec(req)) {
        if (query.next())
            return (query.value(0).toInt() > 0);
    } else {
        LOG_QUERY_ERROR(query);
    }
    return false;
}

/** Return true if the zipcode \e zip and the \e city name correspond */
bool ZipCountryModel::exists(const QString &countryIso, const QString &city, const QString &zip, const QString &province) const
{
    if (!zipCore().isDatabaseAvailable())
        return false;
    QStringList f;
    if (!countryIso.isEmpty())
        f << QString("`COUNTRY`=\"%1\"").arg(countryIso.toUpper());
    if (!zip.isEmpty())
        f << QString("`ZIP`=\"%1\"").arg(zip.toUpper());
    if (!city.isEmpty())
        f << QString("`CITY`=\"%1\"").arg(city.toUpper());
    if (!province.isEmpty())
        f << QString("`ADMIN_NAME1`=\"%1\"").arg(province.toUpper());
    QString filter = f.join(" AND ");

    QString req = QString("%1 WHERE %2").arg(::SELECT_IMPORT).arg(filter);

    QSqlQuery query(zipCore().database());
    if (query.exec(req)) {
        if (query.next())
            return true;
    } else {
        LOG_QUERY_ERROR(query);
    }
    return false;
}

void ZipCountryModel::setCountryFilter(const QString &iso)
{
    _countryIso = iso;
}

void ZipCountryModel::setCityFilter(const QString &city)
{
    _zip.clear();
    _province.clear();
    _city = city;
    refreshQuery();
}

void ZipCountryModel::setZipFilter(const QString &zip)
{
    _zip = zip;
    _province.clear();
    _city.clear();
    refreshQuery();
}

/** Return the WHERE clause according to the internal data (zip, city, province, country) */
QString ZipCountryModel::currentFilter() const
{
    QStringList f;
    if (!_countryIso.isEmpty())
        f << QString("`COUNTRY`=\"%1\"").arg(_countryIso.toUpper());
    if (!_zip.isEmpty())
        f << QString("`ZIP` LIKE \"%1%\"").arg(_zip);
    if (!_city.isEmpty())
        f << QString("`CITY` LIKE \"%1%\"").arg(_city);
    if (!_province.isEmpty())
        f << QString("`ADMIN_NAME1` LIKE \"%1%\"").arg(_province);
    return f.join(" AND ");
}

void ZipCountryModel::refreshQuery()
{
    // Compute where clause
    QString filter = currentFilter();
    if (filter.isEmpty())
        return;
    // Create the SqlQuery command
    QString req = QString("%1 WHERE %2").arg(::SELECT_IMPORT).arg(filter);
    req += " ORDER BY `CITY` ASC ";
    req += ::SELECT_LIMIT;
    if (req == _sqlQuery)
        return;
    _sqlQuery = req;

    setQuery(_sqlQuery, zipCore().database());
    if (!query().isActive()) {
        LOG_QUERY_ERROR(query());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   ZipStateProvinceModel   /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ZipStateProvinceModel::ZipStateProvinceModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

QVariant ZipStateProvinceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    return QSqlQueryModel::data(index, role);
}

void ZipStateProvinceModel::setCountryFilter(const QString &iso)
{
    _countryIso = iso;
}

void ZipStateProvinceModel::setCityFilter(const QString &city)
{
    _zip.clear();
    _province.clear();
    _city = city;
    refreshQuery();
}

void ZipStateProvinceModel::setZipFilter(const QString &zip)
{
    _zip = zip;
    _province.clear();
    _city.clear();
    refreshQuery();
}

void ZipStateProvinceModel::refreshQuery()
{
    // No filter -> nothing to do
    QString filter = currentFilter();
    if (filter.isEmpty())
        return;

    // Create the sql command
    QString req = QString("%1 WHERE %2").arg(::SELECT_PROV_IMPORT).arg(filter);
    req += " ORDER BY `ADMIN_NAME1` ASC ";
    req += ::SELECT_LIMIT;

    // Avoid reselecting current sql command
    if (req == _sqlQuery)
        return;
    _sqlQuery = req;

    // Set the new query
    setQuery(req, zipCore().database());
}

/** Return the WHERE clause according to the internal data (zip, city, province, country) */
QString ZipStateProvinceModel::currentFilter() const
{
    QStringList f;
    if (!_countryIso.isEmpty())
        f << QString("`COUNTRY`=\"%1\"").arg(_countryIso.toUpper());
    if (!_zip.isEmpty())
        f << QString("`ZIP` LIKE \"%1%\"").arg(_zip);
    if (!_city.isEmpty())
        f << QString("`CITY` LIKE \"%1%\"").arg(_city);
    if (!_province.isEmpty())
        f << QString("`ADMIN_NAME1` LIKE \"%1%\"").arg(_province);
    return f.join(" AND ");
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////   ZipCountryCompleters   ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ZipCountryCompleters::ZipCountryCompleters(QObject *parent) :
    QObject(parent),
    m_cityEdit(0),
    m_zipEdit(0),
    m_countryCombo(0),
    m_provinceCombo(0),
    m_ZipModel(0),
    m_CityModel(0),
    m_ProvinceModel(0),
    m_ZipButton(0),
    m_CityButton(0),
    m_DbAvailable(false)
{
    setObjectName("ZipCountryCompleters");
    createModel();

    // Manage datapacks
    connect(&zipCore(), SIGNAL(databaseRefreshed()), this, SLOT(onDatabaseRefreshed()));
}

ZipCountryCompleters::~ZipCountryCompleters()
{
}

/**
 * \internal
 * Creates all needed models
 */
void ZipCountryCompleters::createModel()
{
    // Delete old models
    if (m_ZipModel) {
        delete m_ZipModel;
        m_ZipModel = 0;
    }
    if (m_CityModel) {
        delete m_CityModel;
        m_CityModel = 0;
    }
    if (m_ProvinceModel) {
        delete m_ProvinceModel;
        m_ProvinceModel = 0;
    }
    // Is db available create new modelss
    if (!zipCore().initialize())
        return;
    if (!zipCore().isDatabaseAvailable())
        return;
    m_ZipModel = new ZipCountryModel(this);
    m_CityModel = new ZipCountryModel(this);
    m_ProvinceModel = new ZipStateProvinceModel(this);
}

/*!
 * Define the QComboBox to use as country selector.
 * The combo will be automatically populated and its
 * current index will be set to the current QLocale::Country
 */
void ZipCountryCompleters::setCountryComboBox(Utils::CountryComboBox *box)
{
    m_countryCombo = box;
    connect(m_countryCombo, SIGNAL(currentCountryChanged(QLocale::Country)), this, SLOT(setCountryFilter(QLocale::Country)));
    setCountryFilter(m_countryCombo->currentCountry());
}

/*!
 * Define the QComboBox to use as state/province selector.
 * The combobox will be automatically populated.
 */
void ZipCountryCompleters::setStateProvinceComboBox(QComboBox *box)
{
    m_provinceCombo = box;
    if (m_ProvinceModel) {
        m_provinceCombo->setModel(m_ProvinceModel);
        m_provinceCombo->setModelColumn(0);
        connect(m_provinceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setStateProvinceFilter(int)));
    }
}

/*! Define the QLineEdit to use as city name editor */
void ZipCountryCompleters::setCityLineEdit(Utils::QButtonLineEdit *city)
{
    m_cityEdit = city;
    // Completer
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_CityModel);
    completer->setCompletionColumn(ZipCountryModel::CityZip);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->popup()->setAlternatingRowColors(true);
    m_cityEdit->setCompleter(completer);
    connect(m_cityEdit, SIGNAL(textChanged(QString)), this, SLOT(cityTextChanged()));
    connect(completer, SIGNAL(activated(QModelIndex)), this, SLOT(onCompleterIndexActivated(QModelIndex)));

    // button
    m_CityButton = new QToolButton(m_cityEdit);
    m_CityButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
    m_cityEdit->setRightButton(m_CityButton);
}

/** Define the QLineEdit to use as zip code editor */
void ZipCountryCompleters::setZipLineEdit(Utils::QButtonLineEdit *zip)
{
    m_zipEdit = zip;
    // Completer
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_ZipModel);
    completer->setCompletionColumn(ZipCountryModel::ZipCity);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->popup()->setAlternatingRowColors(true);
    m_zipEdit->setCompleter(completer);
    connect(m_zipEdit, SIGNAL(textChanged(QString)), this, SLOT(zipTextChanged()));
    connect(completer, SIGNAL(activated(QModelIndex)), this, SLOT(onCompleterIndexActivated(QModelIndex)));

    // button
    m_ZipButton = new QToolButton(m_zipEdit);
    m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
    m_zipEdit->setRightButton(m_ZipButton);
}

/*!
 * When user selects a zip or a city this private slot is activated. It causes the line edit
 * to be populated with the selected values.
*/
void ZipCountryCompleters::onCompleterIndexActivated(const QModelIndex &index)
{
    if (!m_ZipModel || !m_CityModel)
        return;
    QAbstractItemModel *model = const_cast<QAbstractItemModel*>(index.model());
    const QString &zip = model->index(index.row(), ZipCountryModel::Zip).data().toString();
    const QString &city = model->index(index.row(), ZipCountryModel::City).data().toString();
    const QString &province = model->index(index.row(), ZipCountryModel::Province).data().toString();
    // Disconnect editors
    m_zipEdit->blockSignals(true);
    m_cityEdit->blockSignals(true);
    m_provinceCombo->blockSignals(true);
    if (m_zipEdit) {
        m_zipEdit->clearFocus();
        m_zipEdit->setText(zip.toUpper());
    }
    if (m_cityEdit) {
        m_cityEdit->clearFocus();
        m_cityEdit->setText(city.toUpper());
    }
    if (m_provinceCombo) {
        m_provinceCombo->clearFocus();
        m_provinceCombo->setCurrentIndex(m_provinceCombo->findText(province));
    }
    // Reconnect editors
    m_zipEdit->blockSignals(false);
    m_cityEdit->blockSignals(false);
    m_provinceCombo->blockSignals(false);
    checkData();
}

/*!
 * \internal
 * Slot connect to the country combo. Update the model filter according
 * to its content.
*/
void ZipCountryCompleters::setCountryFilter(const QLocale::Country country)
{
    if (m_CityModel)
        m_CityModel->setCountryFilter(Utils::countryToIso(country));
    if (m_ZipModel)
        m_ZipModel->setCountryFilter(Utils::countryToIso(country));
    if (m_ProvinceModel)
        m_ProvinceModel->setCountryFilter(Utils::countryToIso(country));
    checkData();
}

/*!
 * \internal
 * Slot connect to the Zipcode textedit. Update the model filter according
 * to its content.
*/
void ZipCountryCompleters::zipTextChanged()
{
    if (m_ZipModel)
        m_ZipModel->setZipFilter(m_zipEdit->completer()->completionPrefix());
    if (m_ProvinceModel)
        m_ProvinceModel->setZipFilter(m_zipEdit->completer()->completionPrefix());
    checkData();
}

/*!
 * \internal
 * Slot connect to the City textedit. Update the model filter according
 * to its content.
*/
void ZipCountryCompleters::cityTextChanged()
{
    if (m_CityModel)
        m_CityModel->setCityFilter(m_cityEdit->completer()->completionPrefix());
    if (m_ProvinceModel)
        m_ProvinceModel->setCityFilter(m_cityEdit->completer()->completionPrefix());
    checkData();
}

/*!
 * \internal
 * Slot connect to the ProvinceState combo. Update the model filter according
 * to its content.
*/
void ZipCountryCompleters::setStateProvinceFilter(int index)
{
    Q_UNUSED(index);
//    if (m_Model)
//        m_Model->setProvinceFilter(m_provinceCombo->itemText(index));
    checkData();
}

/*!
 * \internal
 * Checks validity of country/city/province/zipcode association
 * and sets icons according to the current status
 */
void ZipCountryCompleters::checkData()
{
    if (!m_ZipModel || !m_CityModel)
        return;
    if (!m_zipEdit || !m_cityEdit) {
        return;
    }
    // countrycombo should never be empty
    if (!m_countryCombo) {
        m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONCRITICAL));
        m_CityButton->setIcon(theme()->icon(Core::Constants::ICONCRITICAL));
        m_ZipButton->setToolTip(tr("No country selected"));
        m_CityButton->setToolTip(tr("No country selected"));
        return;
    }
    // zipcodes available for selected country
    if (!m_ZipModel->isCountryAvailable(m_countryCombo->currentCountry())) {
        m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
        m_CityButton->setIcon(theme()->icon(Core::Constants::ICONHELP));
        //: %1 is a country
        m_ZipButton->setToolTip(tr("Autocompletion of zipcodes for %1 not available").
                                arg(QLocale::countryToString(m_countryCombo->currentCountry())));
        //: %1 is a country
        m_CityButton->setToolTip(tr("Autocompletion of cities for %1 not available").
                                 arg(QLocale::countryToString(m_countryCombo->currentCountry())));
        return;
    }

    // zip && city not empty -> check the couple with the model
    if (!m_zipEdit->text().isEmpty() && !m_cityEdit->text().isEmpty()) {
        // We assume that the filter is up to date
        QString province;
        if (m_provinceCombo)
            province = m_provinceCombo->currentText();
        if (m_ZipModel->exists(m_countryCombo->currentIsoCountry(), m_cityEdit->text(), m_zipEdit->text(), province)) {
            m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            m_CityButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            m_ZipButton->setToolTip(tr("Zip/city/country association checked"));
            m_CityButton->setToolTip(tr("Zip/city/country association checked"));
        } else {
            m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONWARNING));
            m_CityButton->setIcon(theme()->icon(Core::Constants::ICONWARNING));
            m_ZipButton->setToolTip(tr("Wrong zip/city/country association"));
            m_CityButton->setToolTip(tr("Wrong zip/city/country association"));
        }
    }
    if (m_zipEdit->text().isEmpty() && m_cityEdit->text().isEmpty()) {
        m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONOK));
        m_CityButton->setIcon(theme()->icon(Core::Constants::ICONOK));
        m_ZipButton->setToolTip(tr("Zip/city/country information available"));
        m_CityButton->setToolTip(tr("Zip/city/country information available"));
    }
}

void ZipCountryCompleters::onDatabaseRefreshed()
{
    createModel();
    if (m_cityEdit)
        m_cityEdit->completer()->setModel(m_ZipModel);
    if (m_zipEdit)
        m_zipEdit->completer()->setModel(m_CityModel);
    checkData();
}
