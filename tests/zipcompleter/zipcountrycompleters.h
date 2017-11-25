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
#ifndef ZIPCOUNTRYCOMPLETERS_H
#define ZIPCOUNTRYCOMPLETERS_H

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

    ZipCountryModel(QObject *parent, QSqlDatabase db);

    int columnCount(const QModelIndex &parent) const {return ZipCity+1;}

    QVariant data(const QModelIndex &index, int role) const;

    bool countryAvailable(const int country) const;
    bool coupleExists(const QString &zip, const QString &city) const;

public Q_SLOTS:
    void filterCity(const QString &name);
    void filterZipCode(const QString &zipCode);
    void filterCountry(const int country);

private:
    QSqlDatabase db;
    QString m_Zip,m_City, m_Country;
};

class ZipCountryCompleters : public QObject
{
    Q_OBJECT
public:
    explicit ZipCountryCompleters(QObject *parent = 0);
    ~ZipCountryCompleters();

    void setCountryComboBox(QComboBox *box);
    void setCityLineEdit(QLineEdit *country);
    void setZipLineEdit(QLineEdit *zip);
    void checkData();
    QAbstractItemModel *completionModel() const {return m_Model;}

private Q_SLOTS:
    void indexActivated(const QModelIndex &index);
    void filterCountry(const int index);
    void zipTextChanged();
    void cityTextChanged();

private:
    bool eventFilter(QObject *, QEvent *);

private:
    QLineEdit *m_City, *m_Zip;
    QComboBox *m_Country;
    ZipCountryModel *m_Model;
    QAbstractItemView *m_View;
    QToolButton *m_ZipButton, *m_CityButton;
};

#endif // ZIPCOUNTRYCOMPLETERS_H
