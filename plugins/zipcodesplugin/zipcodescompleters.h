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
 * \version 0.6.4
 * \date 14 Feb 2012
*/

namespace DataPack {
class Pack;
}

namespace ZipCodes {
namespace Internal {
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
        ZipCity
    };

    ZipCountryModel(QObject *parent, QSqlDatabase db, bool dbAvailable = true);

    int columnCount(const QModelIndex &) const {return ZipCity+1;}

    QVariant data(const QModelIndex &index, int role) const;

    bool countryAvailable(const QLocale::Country country) const;
    bool coupleExists(const QString &zip, const QString &city) const;

public Q_SLOTS:
    void setCityFilter(const QString &city);
    void setZipCodeFilter(const QString &zipCode);
    void setCountryIsoFilter(const QString &country);

private:
    QSqlDatabase db;
    QString m_Zip,m_City, m_countryIso;
    bool m_DbAvailable;
};
}  // End namespace Internal

class ZIPCODES_EXPORT ZipCountryCompleters : public QObject
{
    Q_OBJECT
public:
    explicit ZipCountryCompleters(QObject *parent = 0);
    ~ZipCountryCompleters();

    void setCountryComboBox(Utils::CountryComboBox *box);
    void setCityLineEdit(Utils::QButtonLineEdit *country);
    void setZipLineEdit(Utils::QButtonLineEdit *zip);
    void checkData();
    QAbstractItemModel *completionModel() const {return m_Model;}

private Q_SLOTS:
    void indexActivated(const QModelIndex &index);
    void setCountryFilter(const int index);
    void zipTextChanged();
    void cityTextChanged();
    void packChanged(const DataPack::Pack &pack);

private:
    void createModel();
    bool eventFilter(QObject *, QEvent *);

private:
    Utils::QButtonLineEdit *m_cityEdit, *m_zipEdit;
    Utils::CountryComboBox *m_countryCombo;
    Internal::ZipCountryModel *m_Model;
    QAbstractItemView *m_View;
    QToolButton *m_ZipButton, *m_CityButton;
    bool m_DbAvailable;
};

}  // End namespace ZipCodes

#endif // ZIPCODESCOMPLETERS_H
