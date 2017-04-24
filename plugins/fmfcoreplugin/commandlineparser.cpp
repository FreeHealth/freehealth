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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "commandlineparser.h"

#include <utils/global.h>
#include <utils/log.h>

#include <QApplication>
#include <QDebug>

using namespace Core;

CommandLine::CommandLine(QObject *parent) :
    ICommandLine(parent)
{
    // known command line params
    ref.insert(Chrono, "--chrono");
    ref.insert(CL_TransmitDosage, "--transmit-dosage");
    ref.insert(ConfigFile, "--config");
    ref.insert(ClearUserDatabases, "--clear-user-databases");
    ref.insert(CreateVirtuals, "--create-virtuals");
    ref.insert(ResetUserPreferences, "--reset-users-preferences");
    ref.insert(UserClearLogin,       "--user-clear-log");
    ref.insert(UserClearPassword,    "--user-clear-password");
    ref.insert(CheckFormUpdates,     "--dont-check-form-update");
    ref.insert(AlwaysUpdateForms,    "--always-update-forms");
    ref.insert(NoCheckUpdate, "--no-check-update");

    // set default values
    params.insert(Chrono, false);
    params.insert(CL_TransmitDosage, false);
    params.insert(ClearUserDatabases, false);
    params.insert(CreateVirtuals, false);
    params.insert(ResetUserPreferences, false);
    params.insert(UserClearLogin, QVariant());
    params.insert(UserClearPassword, QVariant());
    params.insert(CheckFormUpdates, true);
    params.insert(AlwaysUpdateForms, false);
    params.insert(NoCheckUpdate, false);

    // read command line params
    QStringList args = qApp->arguments();
    foreach(const QString &a, args) {
        QString k = a;
        if (k.contains(" "))
            k = k.left(k.indexOf(" "));
        if (a.contains("="))
            k = k.left(k.indexOf("="));
        switch (ref.key(k,-1))
        {
        case Chrono : params.insert(Chrono, true); break;
        case CL_TransmitDosage : params.insert(CL_TransmitDosage, true); break;
        case ConfigFile : params.insert(ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
        case ClearUserDatabases : params.insert(ClearUserDatabases, true); break;
        case CreateVirtuals : params.insert(CreateVirtuals, true); break;
        case ResetUserPreferences : params.insert(ResetUserPreferences, true); break;
        case UserClearLogin: params.insert(CommandLine::UserClearLogin, a.mid(a.indexOf("=")+1).remove("\"")); break;
        case UserClearPassword: params.insert(CommandLine::UserClearPassword, a.mid(a.indexOf("=")+1).remove("\"")); break;
        case CheckFormUpdates: params.insert(CommandLine::CheckFormUpdates, false); break;
        case AlwaysUpdateForms: params.insert(CommandLine::AlwaysUpdateForms, true); break;
        case NoCheckUpdate: params.insert(CommandLine::NoCheckUpdate, true); break;
        default : break;
        }
    }

    if (Utils::isReleaseCompilation()) {
        params.insert(ClearUserDatabases, false);
    }
}

CommandLine::~CommandLine()
{}

QVariant CommandLine::value(int param, const QVariant &def) const
{
    return params.value(param,def);
}

void CommandLine::setValue(int ref, const QVariant &value)
{
    params.insert(ref, value);
}

QString CommandLine::paramName(int param) const
{
    if (ref.keys().contains(param))
        return ref.value(param);
    return QString::number(param);
}
