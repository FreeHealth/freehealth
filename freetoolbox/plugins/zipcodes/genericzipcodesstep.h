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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GENERICZIPCODESSTEP_H
#define GENERICZIPCODESSTEP_H

#include <coreplugin/ifullreleasestep.h>
#include <utils/widgets/countrycombobox.h>

#include <QStringList>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
class QNetworkReply;
class QProgressBar;
class QStandardItemModel;
QT_END_NAMESPACE

/**
 * \file genericzipcodestep.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 30 Nov 2012
*/

namespace Utils {
class HttpDownloader;
}

namespace ZipCodes {

struct PostalInfo {
    PostalInfo(const QString &postalCode,
               const QString &city,
               const QString &country,
               const QString &extraCode = QString::null);

    QString postalCode;
    QString city;
    QString extraCode;
    QString country;
};

class GenericZipCodesStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    GenericZipCodesStep(QObject *parent = 0);
    ~GenericZipCodesStep();

    QString id() const {return "GenericZipCodesStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::ZipCodes;}

    bool createTemporaryStorage();
    bool cleanTemporaryStorage() { return true; }
    bool startDownload(QProgressBar *bar = 0);

    bool process();
    QString processMessage() const { return tr("Generic zip codes database creation"); }
    bool postDownloadProcessing();

    bool downloadSelectedCountryInfo();

    QStandardItemModel* availableCountriesModel() { return m_availableCountriesModel; }
    QStandardItemModel* selectedCountriesModel() { return m_selectedCountriesModel; }

    bool createDatabaseScheme();
    bool startDownloadingSelectedCountryData();
    bool populateDatabase();
    bool registerDataPack();

    QStringList errors() const {return m_Errors;}

    void selectCountry(const QModelIndex &index);
    void deselectCountry(const QModelIndex &index);

Q_SIGNALS:
    void availableCountriesListDownloaded();
    void countryListDownloaded(bool);

protected Q_SLOTS:
    void slotSetProgress(qint64 bytesReceived, qint64 bytesTotal);
    bool onAvailableCountriesDownloaded();
    bool downloadZipCodesUsingCachedIso();
    void onSelectedCountryDownloadFinished(QNetworkReply* reply);

private:
    QStringList m_Errors;
    bool m_WithProgress;
    QStandardItemModel *m_availableCountriesModel;
    QStandardItemModel *m_selectedCountriesModel;
    QLocale::Country m_selectedCountry;
    QStringList m_selectedCountryList, m_availableIsoCodes;
    int m_selectedCountriesCounter;
    QList<PostalInfo> m_postalList;
    Utils::HttpDownloader *m_downloader;
};

} // end ZipCodes

#endif // GENERICZIPCODESSTEP_H
