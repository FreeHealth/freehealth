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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ZIPCODESCOMPLETERS_H
#define ZIPCODESCOMPLETERS_H

#include <zipcodesplugin/zipcodes_exporter.h>
#include <utils/widgets/countrycombobox.h>
#include <utils/widgets/qbuttonlineedit.h>

#include <QObject>
#include <QSqlQueryModel>
#include <QValidator>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QCompleter;
class QModelIndex;
class QAbstractItemView;
class QComboBox;
class QToolButton;
QT_END_NAMESPACE

/**
 * \file zipcodescompleters.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 08 Dec 2012
*/

namespace ZipCodes {
namespace Internal {
class ZipStateProvinceModel;

class ZipCountryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum ColumnRepresentation {
        Id = 0,
        Zip,
        City,
        ExtraCode,
        Country,
        ZipCity,
        CityZip,
        Province,
        NumberOfColumn
    };

    ZipCountryModel(QObject *parent);

    int columnCount(const QModelIndex &) const {return NumberOfColumn;}

    QVariant data(const QModelIndex &index, int role) const;

    bool isCountryAvailable(const QLocale::Country country) const;
    bool exists(const QString &countryIso, const QString &city, const QString &zip, const QString &province) const;

public Q_SLOTS:
    void setCountryFilter(const QString &iso);
    void setCityFilter(const QString &city);
    void setZipFilter(const QString &zip);

private:
    QString currentFilter() const;
    void refreshQuery();

private:
    QString _sqlQuery, _countryIso, _zip, _province, _city;
};

class ZipStateProvinceModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum ColumnRepresentation {
        Province = 0
    };

    ZipStateProvinceModel(QObject *parent);
    int columnCount(const QModelIndex &) const {return 1;}
    QVariant data(const QModelIndex &index, int role) const;

public Q_SLOTS:
    void setCountryFilter(const QString &iso);
    void setCityFilter(const QString &city);
    void setZipFilter(const QString &zip);

private:
    QString currentFilter() const;
    void refreshQuery();

private:
    QString _sqlQuery, _countryIso, _zip, _province, _city;
};
}  // End namespace Internal

class ZIPCODES_EXPORT ZipCountryCompleters : public QObject
{
    Q_OBJECT
public:
    explicit ZipCountryCompleters(QObject *parent = 0);
    ~ZipCountryCompleters();

    void setCountryComboBox(Utils::CountryComboBox *box);
    void setStateProvinceComboBox(QComboBox *box);
    void setCityLineEdit(Utils::QButtonLineEdit *country);
    void setZipLineEdit(Utils::QButtonLineEdit *zip);
    void checkData();

private Q_SLOTS:
    void onCompleterIndexActivated(const QModelIndex &index);
    void setCountryFilter(const QLocale::Country country);
    void zipTextChanged();
    void cityTextChanged();
    void setStateProvinceFilter(int);
    void onDatabaseRefreshed();

private:
    void createModel();

private:
    Utils::QButtonLineEdit *m_cityEdit, *m_zipEdit;
    Utils::CountryComboBox *m_countryCombo;
    QComboBox *m_provinceCombo;
    Internal::ZipCountryModel *m_ZipModel, *m_CityModel;
    Internal::ZipStateProvinceModel *m_ProvinceModel;
    QToolButton *m_ZipButton, *m_CityButton;
    bool m_DbAvailable;
};

}  // End namespace ZipCodes

#endif // ZIPCODESCOMPLETERS_H
