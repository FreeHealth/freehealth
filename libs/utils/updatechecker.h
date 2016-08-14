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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <utils/global_exporter.h>

#include <QObject>
#include <QUrl>
#include <QString>
#include <QProgressBar>
QT_BEGIN_NAMESPACE
class QWidget;
class QSettings;
QT_END_NAMESPACE

namespace Utils {
namespace Constants {
    const char* const  FREEHEALTH_UPDATE_URL  = "https://freehealth.io/update/update-fhio.txt";
    const char* const  FREEDIAMS_UPDATE_URL     = "https://freehealth.io/update/update-di.txt";
    const char* const  FREEACCOUNT_UPDATE_URL   = "https://freehealth.io/update/update-freeaccount.txt";
    const char* const  FREEICD_UPDATE_URL       = "https://freehealth.io/update/update-freeicd.txt";
    const char* const  FREEGIR_UPDATE_URL       = "https://freehealth.io/update/update-freegir.txt";
    const char* const  FREETOOLBOX_UPDATE_URL   = "https://freehealth.io/update/update-freetoolbox.txt";
    const char* const  FREEPAD_UPDATE_URL       = "https://freehealth.io/update/update-freepad.txt";
    const char* const  FREEDRC_UPDATE_URL       = "https://freehealth.io/update/update-freedrc.txt";
    const char* const  FREEDDIMANAGER_UPDATE_URL= "https://freehealth.io/update/update-freeddi.txt";
    const char* const  FREEGUARD_UPDATE_URL     = "https://freehealth.io/update/update-freeguard.txt";

    const char* const  S_CHECKUPDATE       = "CheckUpdate";
    const char* const  S_LAST_CHECKUPDATE  = "LastCheckUpdate";
}
}

/**
 * \file updatechecker.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 18 Mar 2010
*/

namespace Utils {
namespace Internal {
class UpdateCheckerPrivate;
}

class UTILS_EXPORT UpdateChecker : public QObject
{
    Q_OBJECT
    friend class Internal::UpdateCheckerPrivate;

public:
     UpdateChecker(QObject *parent = 0);
    ~UpdateChecker();

    bool needsUpdateChecking(QSettings *settings) const;

    bool isChecking() const;
    void check(const QString &url);
    void check(const QUrl &url);
    void cancel();

    bool fileRetrieved();
    bool hasUpdate();
    QString lastVersion();
    QString updateText();

    QProgressBar *progressBar(QWidget *parent);

public Q_SLOTS:
    void showUpdateInformation();

Q_SIGNALS:
    void updateFound(const QString &updateText);
    void updateFound();
    void done(bool);

protected:
    void emitSignals();

private:
    Internal::UpdateCheckerPrivate *d;
};
}

#endif  // UPDATECHECKER_H
