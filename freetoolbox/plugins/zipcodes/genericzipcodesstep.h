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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *  Contributors:                                                          *
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

namespace Internal {
class GenericZipCodesStepPrivate;
}  // namespace Internal

class GenericZipCodesStep : public Core::IFullReleaseStep
{
    Q_OBJECT
    friend class ZipCodes::Internal::GenericZipCodesStepPrivate;

public:
    GenericZipCodesStep(QObject *parent = 0);
    ~GenericZipCodesStep();

    QString id() const {return "GenericZipCodesStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::ZipCodes;}
    bool createTemporaryStorage();
    bool cleanTemporaryStorage();
    bool startProcessing(ProcessTiming timing, SubProcess subProcess);

    bool startDownload();

    bool process();
    QString processMessage() const;
    bool postDownloadProcessing();

    QStandardItemModel* availableCountriesModel();
    bool populateCountryModel() const;

    QStandardItemModel* selectedCountriesModel();

    bool createDatabaseScheme();
    bool populateDatabase();
    bool registerDataPack();

    bool createDatapackForCountries();

    QStringList errors() const;

Q_SIGNALS:
    void countryListDownloaded();

private Q_SLOTS:
    void onSubProcessFinished();

private:
    Internal::GenericZipCodesStepPrivate *d;
};

} // end ZipCodes

#endif // GENERICZIPCODESSTEP_H
