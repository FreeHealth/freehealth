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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
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

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <utils/log.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

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
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

ZipCountryModel::ZipCountryModel(QObject *parent, QSqlDatabase _db, bool dbAvailable) :
    QSqlQueryModel(parent),
    m_DbAvailable(dbAvailable)
{
    db = _db;
    m_countryIso = Utils::countryToIso(QLocale().country()).toLower();

    if (m_DbAvailable) {  // BUG with if (m_DbAvailable && db.isOpen()) --> returning false with (true && true) ????
        if (db.isOpen()) {
            setQuery("SELECT ZIP, CITY FROM ZIPS LIMIT 0, 1", _db);
            if (!query().isActive()) {
                LOG_QUERY_ERROR(query());
            }
        }
    }
}

QVariant ZipCountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole) {

        // translate ColumnRepresentation values into real columns of the database
        switch (index.column()) {
        case Zip:
            return QSqlQueryModel::data(QSqlQueryModel::index(index.row(), 0));
        case City:
            return QSqlQueryModel::data(QSqlQueryModel::index(index.row(), 1));
        case Country:
            return QLocale::countryToString(QLocale(QSqlQueryModel::data(QSqlQueryModel::index(index.row(), Country)).toString()).country());
        case ZipCity:
        {
            const QString &zip = QSqlQueryModel::data(QSqlQueryModel::index(index.row(), 0)).toString();
            const QString &city = QSqlQueryModel::data(QSqlQueryModel::index(index.row(), 1)).toString();
            return QString(tr("%1, %2").arg(zip, city));
        }
        }

    }
    return QVariant();
}

/** Return all available countries included in the zipcode database */
bool ZipCountryModel::countryAvailable(const QLocale::Country country) const
{
    if (!m_DbAvailable) {  // BUG with if (m_DbAvailable && db.isOpen()) --> returning false with (true && true) ????
        if (!db.isOpen()) {
            return false;
        }
    }
    QString req = QString("SELECT DISTINCT COUNT(COUNTRY) FROM ZIPS WHERE `COUNTRY`=\"%1\"")
            .arg(Utils::countryToIso(country).toLower());
    QSqlQuery query(db);
    if (query.exec(req)) {
        if (query.next())
            return query.value(0).toInt();
    } else {
        LOG_QUERY_ERROR(query);
    }
    return false;
}

/** Return true if the zipcode \e zip and the \e city name correspond */
bool ZipCountryModel::coupleExists(const QString &zip, const QString &city) const
{
    if (!m_DbAvailable) {  // BUG with if (m_DbAvailable && db.isOpen()) --> returning false with (true && true) ????
        if (!db.isOpen()) {
            return false;
        }
    }
    QString req = QString("SELECT COUNT(ZIP) FROM ZIPS WHERE `COUNTRY`=\"%1\" AND `CITY`=\"%2\" AND ZIP=\"%3\"")
            .arg(m_countryIso, city, zip);
    QSqlQuery query(db);
    if (query.exec(req)) {
        if (query.next())
            return query.value(0).toInt();
    } else {
        LOG_QUERY_ERROR(query);
    }
    return false;
}

/** Filter the model with the \e city */
void ZipCountryModel::setCityFilter(const QString &city)
{
    if (!m_DbAvailable) {  // BUG with if (m_DbAvailable && db.isOpen()) --> returning false with (true && true) ????
        if (!db.isOpen()) {
            return;
        }
    }
    if (m_City==city)
        return;

    QString tmpcity = city;
    m_City = tmpcity.remove("'");

    QString req = QString("SELECT ZIP, CITY FROM ZIPS WHERE `COUNTRY`=\"%1\" "
                          "AND `CITY` like \"%2%\" ORDER BY CITY ASC LIMIT 0, 20")
            .arg(m_countryIso, m_City);
    setQuery(req, db);
    if (!query().isActive()) {
        LOG_QUERY_ERROR(query());
    }
}

/** Filter with the zipcode \e zipCode */
void ZipCountryModel::setZipCodeFilter(const QString &zipCode)
{
    if (!m_DbAvailable) {  // BUG with if (m_DbAvailable && db.isOpen()) --> returning false with (true && true) ????
        if (!db.isOpen()) {
            return;
        }
    }
    if (m_Zip==zipCode)
        return;

    QString zip = zipCode;
    m_Zip = zip.remove("'");
    QString req = QString("SELECT ZIP, CITY FROM ZIPS WHERE `COUNTRY`=\"%1\" "
                          "AND `ZIP` like \"%2%\" ORDER BY ZIP LIMIT 0, 20")
            .arg(m_countryIso, m_Zip);
    setQuery(req, db);
    if (!query().isActive()) {
        LOG_QUERY_ERROR(query());
    }
}

/** Filter with the country iso code (two letters) \e countryIso */
void ZipCountryModel::setCountryIsoFilter(const QString &countryIso)
{
    // strip possible SQL injection char
    QString iso = countryIso;
    iso = iso.remove("'");

    // basic check if param is a valid country ISO filter
    if (m_countryIso == iso|| iso.length() != 2)
        return;

    m_countryIso = iso.toLower();
}

// Find the database to use. In priority order:
// - User datapack
// - Application installed datapack
static QString databasePath()
{
    QString dbRelPath = "/zipcodes/zipcodes.db";
    QString tmp;
    tmp = settings()->dataPackInstallPath() + dbRelPath;
    if (QFileInfo(tmp).exists())
        return settings()->dataPackInstallPath();
    tmp = settings()->dataPackApplicationInstalledPath() + dbRelPath;
    return settings()->dataPackApplicationInstalledPath();
}

static QString databaseFileName()
{
    return databasePath() + QDir::separator() + "zipcodes" + QDir::separator() + "zipcodes.db";
}

ZipCountryCompleters::ZipCountryCompleters(QObject *parent) :
    QObject(parent),
    m_cityEdit(0),
    m_zipEdit(0),
    m_countryCombo(0),
    m_Model(0),
    m_ZipButton(0),
    m_CityButton(0),
    m_DbAvailable(false)
{
    setObjectName("ZipCountryCompleters");
    createModel();

    // Manage datapacks
    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
//    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
}

ZipCountryCompleters::~ZipCountryCompleters()
{
}

void ZipCountryCompleters::createModel()
{
    if (m_Model) {
        delete m_Model;
        m_Model = 0;
    }
    QSqlDatabase db;
    if (QSqlDatabase::connectionNames().contains("ZIPS")) {
        db = QSqlDatabase::database("ZIPS");
    } else {
        LOG(QString("Trying to open ZipCode database from %1").arg(databaseFileName()));
        db = QSqlDatabase::addDatabase("QSQLITE", "ZIPS");
        if (QFileInfo(databaseFileName()).exists()) {
            db.setDatabaseName(databaseFileName());
            m_DbAvailable = true;
        } else {
            m_DbAvailable = false;
        }
    }
    if (m_DbAvailable) {
        if (!db.open()) {
            LOG_ERROR("Unable to open Zip database");
            m_DbAvailable = false;
        } else {
            m_Model = new ZipCountryModel(this, db, m_DbAvailable);
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg("zipcodes").arg("sqlite"));
        }
    }
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
void setStateProvinceComboBox(QComboBox *box)
{
    Q_UNUSED(box);
}

/*! Define the QLineEdit to use as city name editor */
void ZipCountryCompleters::setCityLineEdit(Utils::QButtonLineEdit *city)
{
    m_cityEdit = city;
    // Completer
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_Model);
    completer->setCompletionColumn(ZipCountryModel::City);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
//    m_Completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->popup()->setAlternatingRowColors(true);
    city->setCompleter(completer);
    connect(m_cityEdit, SIGNAL(textChanged(QString)), this, SLOT(cityTextChanged()));
    connect(completer, SIGNAL(activated(QModelIndex)), this, SLOT(onCompleterIndexActivated(QModelIndex)));

    // button
    m_CityButton = new QToolButton(m_cityEdit);
    m_CityButton->setIcon(theme()->icon(Core::Constants::ICONOK));
    m_cityEdit->setRightButton(m_CityButton);
    m_cityEdit->installEventFilter(this);
}

/** Define the QLineEdit to use as zip code editor */
void ZipCountryCompleters::setZipLineEdit(Utils::QButtonLineEdit *zip)
{
    m_zipEdit = zip;
    // Completer
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_Model);
    completer->setCompletionColumn(ZipCountryModel::ZipCity);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->popup()->setAlternatingRowColors(true);
    zip->setCompleter(completer);
    connect(m_zipEdit, SIGNAL(textChanged(QString)), this, SLOT(zipTextChanged()));
    connect(completer, SIGNAL(activated(QModelIndex)), this, SLOT(onCompleterIndexActivated(QModelIndex)));

    // button
    m_ZipButton = new QToolButton(m_zipEdit);
    m_ZipButton->setIcon(theme()->icon(Core::Constants::ICONOK));
    m_zipEdit->setRightButton(m_ZipButton);
    m_zipEdit->installEventFilter(this);
}

/*!
 * When user selects a zip or a city this private slot is activated. It causes the line edit
 * to be populated with the selected values.
*/
void ZipCountryCompleters::onCompleterIndexActivated(const QModelIndex &index)
{
    if (!m_Model)
        return;
    const QString &zip = m_Model->index(index.row(), ZipCountryModel::Zip).data().toString();
    const QString &city = m_Model->index(index.row(), ZipCountryModel::City).data().toString();
    if (m_zipEdit) {
        m_zipEdit->clearFocus();
        m_zipEdit->setText(zip);
    }
    if (m_cityEdit) {
        m_cityEdit->clearFocus();
        m_cityEdit->setText(city);
    }
    checkData();
}

/*! Sets the country filter \e country for the zip/city edits */
void ZipCountryCompleters::setCountryFilter(const QLocale::Country country)
{
    if (!m_countryCombo)
        return;
    if (!m_Model)
        return;
    m_Model->setCountryIsoFilter(Utils::countryToIso(country));
    checkData();
}

/*! Sets a the new text new as filter for the zip completer */
void ZipCountryCompleters::zipTextChanged()
{
    if (!m_Model)
        return;
    m_Model->setZipCodeFilter(m_zipEdit->completer()->completionPrefix());
}

/*! Sets a the new text new as filter for the city completer */
void ZipCountryCompleters::cityTextChanged()
{
    if (!m_Model)
        return;
    m_Model->setCityFilter(m_cityEdit->completer()->completionPrefix());
}

/*! Checks validity of country/city/zipcode associationand sets icons according to the current status */
void ZipCountryCompleters::checkData()
{
    if (!m_Model)
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
    if (!m_Model->countryAvailable(m_countryCombo->currentCountry())) {
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
        if (m_Model->coupleExists(m_zipEdit->text(), m_cityEdit->text())) {
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

/** Refresh database if a Zip code data pack is installed/removed */
void ZipCountryCompleters::packChanged(const DataPack::Pack &pack)
{
    if (pack.dataType() == DataPack::Pack::ZipCodes) {
        QSqlDatabase::removeDatabase("ZIPS");
        createModel();
        m_cityEdit->completer()->setModel(m_Model);
        m_zipEdit->completer()->setModel(m_Model);
        m_Model->setCountryIsoFilter(m_countryCombo->currentIsoCountry());
        checkData();
    }
}

