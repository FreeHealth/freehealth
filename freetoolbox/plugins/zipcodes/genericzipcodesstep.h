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
#include <QStringList>
#include <utils/widgets/countrycombobox.h>

QT_BEGIN_NAMESPACE
class QNetworkReply;
class QProgressBar;
QT_END_NAMESPACE


namespace ZipCodes {

class GenericZipCodesStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    GenericZipCodesStep(QObject *parent = 0);
    ~GenericZipCodesStep();

    QString id() const {return "GenericZipCodesStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::ZipCodes;}

    bool createDir();
    bool cleanFiles();
    void setCountryCombo(Utils::CountryComboBox* combo);
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const { return tr("Generic zip codes database creation"); }

    bool unzipFiles();
    bool prepareData();
    bool createDatabase();
    bool populateDatabase();

Q_SIGNALS:
    void countryListDownloaded(bool);

protected Q_SLOTS:
    void slotSetProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotRequestFinished(QNetworkReply *reply);

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
    bool m_WithProgress;
    Utils::CountryComboBox* m_countryCombo;
};
} // end ZipCodes
#endif // GENERICZIPCODESSTEP_H
