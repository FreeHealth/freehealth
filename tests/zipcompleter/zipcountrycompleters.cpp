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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "zipcountrycompleters.h"

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

namespace {
static const unsigned char two_letter_country_code_list[] =
"  " // Default
"AF" // Afghanistan
"AL" // Albania
"DZ" // Algeria
"AS" // AmericanSamoa
"AD" // Andorra
"AO" // Angola
"AI" // Anguilla
"AQ" // Antarctica
"AG" // AntiguaAndBarbuda
"AR" // Argentina
"AM" // Armenia
"AW" // Aruba
"AU" // Australia
"AT" // Austria
"AZ" // Azerbaijan
"BS" // Bahamas
"BH" // Bahrain
"BD" // Bangladesh
"BB" // Barbados
"BY" // Belarus
"BE" // Belgium
"BZ" // Belize
"BJ" // Benin
"BM" // Bermuda
"BT" // Bhutan
"BO" // Bolivia
"BA" // BosniaAndHerzegowina
"BW" // Botswana
"BV" // BouvetIsland
"BR" // Brazil
"IO" // BritishIndianOceanTerritory
"BN" // BruneiDarussalam
"BG" // Bulgaria
"BF" // BurkinaFaso
"BI" // Burundi
"KH" // Cambodia
"CM" // Cameroon
"CA" // Canada
"CV" // CapeVerde
"KY" // CaymanIslands
"CF" // CentralAfricanRepublic
"TD" // Chad
"CL" // Chile
"CN" // China
"CX" // ChristmasIsland
"CC" // CocosIslands
"CO" // Colombia
"KM" // Comoros
"CD" // DemocraticRepublicOfCongo
"CG" // PeoplesRepublicOfCongo
"CK" // CookIslands
"CR" // CostaRica
"CI" // IvoryCoast
"HR" // Croatia
"CU" // Cuba
"CY" // Cyprus
"CZ" // CzechRepublic
"DK" // Denmark
"DJ" // Djibouti
"DM" // Dominica
"DO" // DominicanRepublic
"TL" // EastTimor
"EC" // Ecuador
"EG" // Egypt
"SV" // ElSalvador
"GQ" // EquatorialGuinea
"ER" // Eritrea
"EE" // Estonia
"ET" // Ethiopia
"FK" // FalklandIslands
"FO" // FaroeIslands
"FJ" // Fiji
"FI" // Finland
"FR" // France
"FX" // MetropolitanFrance
"GF" // FrenchGuiana
"PF" // FrenchPolynesia
"TF" // FrenchSouthernTerritories
"GA" // Gabon
"GM" // Gambia
"GE" // Georgia
"DE" // Germany
"GH" // Ghana
"GI" // Gibraltar
"GR" // Greece
"GL" // Greenland
"GD" // Grenada
"GP" // Guadeloupe
"GU" // Guam
"GT" // Guatemala
"GN" // Guinea
"GW" // GuineaBissau
"GY" // Guyana
"HT" // Haiti
"HM" // HeardAndMcDonaldIslands
"HN" // Honduras
"HK" // HongKong
"HU" // Hungary
"IS" // Iceland
"IN" // India
"ID" // Indonesia
"IR" // Iran
"IQ" // Iraq
"IE" // Ireland
"IL" // Israel
"IT" // Italy
"JM" // Jamaica
"JP" // Japan
"JO" // Jordan
"KZ" // Kazakhstan
"KE" // Kenya
"KI" // Kiribati
"KP" // DemocraticRepublicOfKorea
"KR" // RepublicOfKorea
"KW" // Kuwait
"KG" // Kyrgyzstan
"LA" // Lao
"LV" // Latvia
"LB" // Lebanon
"LS" // Lesotho
"LR" // Liberia
"LY" // LibyanArabJamahiriya
"LI" // Liechtenstein
"LT" // Lithuania
"LU" // Luxembourg
"MO" // Macau
"MK" // Macedonia
"MG" // Madagascar
"MW" // Malawi
"MY" // Malaysia
"MV" // Maldives
"ML" // Mali
"MT" // Malta
"MH" // MarshallIslands
"MQ" // Martinique
"MR" // Mauritania
"MU" // Mauritius
"YT" // Mayotte
"MX" // Mexico
"FM" // Micronesia
"MD" // Moldova
"MC" // Monaco
"MN" // Mongolia
"MS" // Montserrat
"MA" // Morocco
"MZ" // Mozambique
"MM" // Myanmar
"NA" // Namibia
"NR" // Nauru
"NP" // Nepal
"NL" // Netherlands
"AN" // NetherlandsAntilles
"NC" // NewCaledonia
"NZ" // NewZealand
"NI" // Nicaragua
"NE" // Niger
"NG" // Nigeria
"NU" // Niue
"NF" // NorfolkIsland
"MP" // NorthernMarianaIslands
"NO" // Norway
"OM" // Oman
"PK" // Pakistan
"PW" // Palau
"PS" // PalestinianTerritory
"PA" // Panama
"PG" // PapuaNewGuinea
"PY" // Paraguay
"PE" // Peru
"PH" // Philippines
"PN" // Pitcairn
"PL" // Poland
"PT" // Portugal
"PR" // PuertoRico
"QA" // Qatar
"RE" // Reunion
"RO" // Romania
"RU" // RussianFederation
"RW" // Rwanda
"KN" // SaintKittsAndNevis
"LC" // StLucia
"VC" // StVincentAndTheGrenadines
"WS" // Samoa
"SM" // SanMarino
"ST" // SaoTomeAndPrincipe
"SA" // SaudiArabia
"SN" // Senegal
"SC" // Seychelles
"SL" // SierraLeone
"SG" // Singapore
"SK" // Slovakia
"SI" // Slovenia
"SB" // SolomonIslands
"SO" // Somalia
"ZA" // SouthAfrica
"GS" // SouthGeorgiaAndTheSouthSandwichIslands
"ES" // Spain
"LK" // SriLanka
"SH" // StHelena
"PM" // StPierreAndMiquelon
"SD" // Sudan
"SR" // Suriname
"SJ" // SvalbardAndJanMayenIslands
"SZ" // Swaziland
"SE" // Sweden
"CH" // Switzerland
"SY" // SyrianArabRepublic
"TW" // Taiwan
"TJ" // Tajikistan
"TZ" // Tanzania
"TH" // Thailand
"TG" // Togo
"TK" // Tokelau
"TO" // Tonga
"TT" // TrinidadAndTobago
"TN" // Tunisia
"TR" // Turkey
"TM" // Turkmenistan
"TC" // TurksAndCaicosIslands
"TV" // Tuvalu
"UG" // Uganda
"UA" // Ukraine
"AE" // UnitedArabEmirates
"GB" // UnitedKingdom
"US" // UnitedStates
"UM" // UnitedStatesMinorOutlyingIslands
"UY" // Uruguay
"UZ" // Uzbekistan
"VU" // Vanuatu
"VA" // VaticanCityState
"VE" // Venezuela
"VN" // VietNam
"VG" // BritishVirginIslands
"VI" // USVirginIslands
"WF" // WallisAndFutunaIslands
"EH" // WesternSahara
"YE" // Yemen
"YU" // Yugoslavia
"ZM" // Zambia
"ZW" // Zimbabwe
"CS" // SerbiaAndMontenegro
;

static QString countryToIso(QLocale::Country country)
{
    if (country == QLocale::AnyCountry)
        return QString();

    QString code;
    code.resize(2);
    const unsigned char *c = two_letter_country_code_list + 2 * (uint(country));
    code[0] = ushort(c[0]);
    code[1] = ushort(c[1]);
    return code;
}

}  // End anoymous namespace

/**
  \class ZipCountryModel
  Private model used by the ZipCountryCompleters
*/
ZipCountryModel::ZipCountryModel(QObject *parent, QSqlDatabase _db): QSqlQueryModel(parent)
{
    db = _db;
    m_Country = ::countryToIso(QLocale().country()).toLower();
    setQuery("SELECT ZIP, CITY FROM ZIPS LIMIT 0, 1", _db);
}

QVariant ZipCountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
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
            return zip + ", " + city;
        }
        }

    }
    return QVariant();
}

bool ZipCountryModel::countryAvailable(const int country) const
{
    QString req = QString("SELECT DISTINCT COUNT(COUNTRY) FROM ZIPS WHERE `COUNTRY`='%1'")
            .arg(::countryToIso(QLocale::Country(country)).toLower());
    QSqlQuery query(db);
    if (query.exec(req)) {
        if (query.next())
            return query.value(0).toInt();
    }
    return false;
}

bool ZipCountryModel::coupleExists(const QString &zip, const QString &city) const
{
    QString req = QString("SELECT COUNT(ZIP) FROM ZIPS WHERE `COUNTRY`='%1' AND `CITY`='%2' AND ZIP='%3'")
            .arg(m_Country).arg(city).arg(zip);
    QSqlQuery query(db);
    if (query.exec(req)) {
        if (query.next())
            return query.value(0).toInt();
    }
    return false;
}

void ZipCountryModel::filterCity(const QString &name)
{
    if (m_City==name)
        return;
    m_City=name;
    QString req = QString("SELECT ZIP, CITY FROM ZIPS WHERE `COUNTRY`='%1' AND `CITY` like '%2%' ORDER BY CITY ASC LIMIT 0, 20")
            .arg(m_Country).arg(name);
    setQuery(req, db);
}

void ZipCountryModel::filterZipCode(const QString &zipCode)
{
    if (m_Zip==zipCode)
        return;
    m_Zip=zipCode;
    QString req = QString("SELECT ZIP, CITY FROM ZIPS WHERE `COUNTRY`='%1' AND `ZIP` like '%2%' ORDER BY ZIP LIMIT 0, 20")
            .arg(m_Country).arg(zipCode);
    setQuery(req, db);
}

void ZipCountryModel::filterCountry(const int country)
{
    m_Country = ::countryToIso(QLocale::Country(country)).toLower();
}


/**
  \class ZipCountryCompleters
  The ZipCountryCompleters is an object that eases the input of zipcodes and city names. It
  works together with a zipcode database. You just have to set the zipcode and city QLineEdit with
  setZipLineEdit() and setCityLineEdit(). Users will get a completer for the zipcode and the city.
  When the user will select a zipcode or a city name, both line edit will be updated with the selected
  data.
*/
ZipCountryCompleters::ZipCountryCompleters(QObject *parent) :
    QObject(parent), m_Zip(0), m_City(0), m_Country(0), m_Model(0),
    m_ZipButton(0), m_CityButton(0)
{
    // Create Db
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ZIPS");
    db.setDatabaseName(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/databases/zipcodes/zipcodes.db"));
    db.open();
    // Create SQL model && completer
    m_Model = new ZipCountryModel(this, db);
}

ZipCountryCompleters::~ZipCountryCompleters()
{
}

/** Define the QComboBox to use as country selector. The combo will be automatically populated and its current index will be set to the current QLocale::Country */
void ZipCountryCompleters::setCountryComboBox(QComboBox *box)
{
    m_Country = box;
    box->clear();
    int i=1;
    QMap<QString, int> countries;
    while (i < 246) {
        const QString &c = QLocale::countryToString(QLocale::Country(i));
        if (c.isEmpty())
            return;
        countries.insert(c, i);
        ++i;
    }
    QMapIterator<QString, int> it(countries);
    while (it.hasNext()) {
        it.next();
        QString flag = ::countryToIso(QLocale::Country(it.value()));
        flag = QDir::cleanPath(qApp->applicationDirPath() + QString("/../../../../../global_resources/pixmap/16x16/flags/%1.png").arg(flag));
        box->addItem(QIcon(flag), it.key(), it.value());
    }

    connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(filterCountry(int)));
    // select current country
    box->setCurrentIndex(QLocale().country()-1);
}

/** Define the QLineEdit to use as city name editor */
void ZipCountryCompleters::setCityLineEdit(QLineEdit *city)
{
    m_City = city;
    // Completer
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_Model);
    completer->setCompletionColumn(ZipCountryModel::City);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
//    m_Completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->popup()->setAlternatingRowColors(true);
    city->setCompleter(completer);
    connect(m_City, SIGNAL(textChanged(QString)), this, SLOT(cityTextChanged()));
    connect(completer, SIGNAL(activated(QModelIndex)), this, SLOT(indexActivated(QModelIndex)));

    // button
    m_CityButton = new QToolButton(m_City);
    m_CityButton->setToolTip("City button");
    m_CityButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_CityButton->setIconSize(QSize(16,16));
    m_CityButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/ok.png")));
    m_CityButton->setMinimumSize(20,20);
    m_CityButton->setMaximumSize(20,20);
    m_CityButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    m_CityButton->show();

    int frameWidth = m_City->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QSize msz = m_City->minimumSizeHint();
    m_City->setMinimumSize(qMax(msz.width(), m_CityButton->maximumHeight() + frameWidth * 2 + 2),
                          qMax(msz.height(), m_CityButton->maximumHeight() + frameWidth * 2 + 2));
    m_City->setStyleSheet(QString("padding-left: %1px;").arg(m_CityButton->sizeHint().width() + frameWidth));
    m_City->installEventFilter(this);
}

/** Define the QLineEdit to use as zip code editor */
void ZipCountryCompleters::setZipLineEdit(QLineEdit *zip)
{
    m_Zip = zip;
    // Completer
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_Model);
    completer->setCompletionColumn(ZipCountryModel::ZipCity);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->popup()->setAlternatingRowColors(true);
    zip->setCompleter(completer);
    connect(m_Zip, SIGNAL(textChanged(QString)), this, SLOT(zipTextChanged()));
    connect(completer, SIGNAL(activated(QModelIndex)), this, SLOT(indexActivated(QModelIndex)));

    // button
    m_ZipButton = new QToolButton(m_Zip);
    m_ZipButton->setToolTip("Zip button");
    m_ZipButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_ZipButton->setIconSize(QSize(16,16));
    m_ZipButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/ok.png")));
    m_ZipButton->setMinimumSize(20,20);
    m_ZipButton->setMaximumSize(20,20);
    m_ZipButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    m_ZipButton->show();

    int frameWidth = m_Zip->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QSize msz = m_Zip->minimumSizeHint();
    m_Zip->setMinimumSize(qMax(msz.width(), m_ZipButton->maximumHeight() + frameWidth * 2 + 2),
                          qMax(msz.height(), m_ZipButton->maximumHeight() + frameWidth * 2 + 2));
    m_Zip->setStyleSheet(QString("padding-left: %1px;").arg(m_ZipButton->sizeHint().width() + frameWidth));
    m_Zip->installEventFilter(this);
}

/** When user select a zip or a city this private slot s activated. It causes the line edit
    to be populated with the selected values.
*/
void ZipCountryCompleters::indexActivated(const QModelIndex &index)
{
    QString zip = m_Model->index(index.row(), ZipCountryModel::Zip).data().toString();
    QString city = m_Model->index(index.row(), ZipCountryModel::City).data().toString();
    if (m_Zip) {
        m_Zip->clearFocus();
        m_Zip->setText(zip);
    }
    if (m_City) {
        m_City->clearFocus();
        m_City->setText(city);
    }
    checkData();
}

void ZipCountryCompleters::filterCountry(const int index)
{
    if (!m_Country)
        return;
    if (!m_Model)
        return;
    m_Model->filterCountry(m_Country->itemData(index).toInt());
    checkData();
}

void ZipCountryCompleters::zipTextChanged()
{
    m_Model->filterZipCode(m_Zip->completer()->completionPrefix());
}

void ZipCountryCompleters::cityTextChanged()
{
    m_Model->filterCity(m_City->completer()->completionPrefix());
}

/** Private slot used to check the validity of the country/city/zipcode association. */
void ZipCountryCompleters::checkData()
{
    if (!m_Zip || !m_City) {
        return;
    }
    // country should never be empty
    if (!m_Country) {
        m_ZipButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/critical.png")));
        m_CityButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/critical.png")));
        m_ZipButton->setToolTip("No country selected");
        m_CityButton->setToolTip("No country selected");
        return;
    }
    // country zipcodes available
    if (!m_Model->countryAvailable(m_Country->currentIndex()+1)) {
        m_ZipButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/help.png")));
        m_CityButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/help.png")));
        return;
    }

    // zip/city empty -> can be calculated ?
    // zip && city not empty -> check the couple with the model
    if (!m_Zip->text().isEmpty() && !m_City->text().isEmpty()) {
        if (m_Model->coupleExists(m_Zip->text(), m_City->text())) {
            m_ZipButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/ok.png")));
            m_CityButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/ok.png")));
            m_ZipButton->setToolTip("Zip/city/country association checked");
            m_CityButton->setToolTip("Zip/city/country association checked");
        } else {
            m_ZipButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/warning.png")));
            m_CityButton->setIcon(QIcon(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/pixmap/16x16/warning.png")));
            m_ZipButton->setToolTip("Wrong zip/city/country association");
            m_CityButton->setToolTip("Wrong zip/city/country association");
        }
    }
}

/** Event Filter is used to draw toolButtons inside the QLineEdit. */
bool ZipCountryCompleters::eventFilter(QObject *o, QEvent *e)
{
    if (o==m_Zip) {
        if (e->type()==QEvent::Resize) {
            m_Zip->event(e);
            QSize sz = m_ZipButton->sizeHint();
            int frameWidth = m_Zip->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
            m_ZipButton->move(m_Zip->rect().left() + frameWidth ,
                              (m_Zip->rect().bottom() + 1 - sz.height()) / 2);
        }
    } else if (o==m_City) {
        if (e->type()==QEvent::Resize) {
            m_City->event(e);
            QSize sz = m_CityButton->sizeHint();
            int frameWidth = m_City->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
            m_CityButton->move(m_City->rect().left() + frameWidth ,
                              (m_City->rect().bottom() + 1 - sz.height()) / 2);
        }
    }
    return false;
}
