/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "commandlineparser.h"
#include "guard_constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QDir>

#include <QDebug>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Core {
namespace Internal {
class CommandLinePrivate
{
public:
    CommandLinePrivate()
    {
        params.insert(Core::Constants::CL_Test,          "--test");
        params.insert(CommandLine::Chrono,               "--chrono");
        params.insert(CommandLine::ConfigFile,           "--config");
        params.insert(CommandLine::ClearUserDatabases,   "--clear-user-databases");
        params.insert(CommandLine::CreateVirtuals,       "--create-virtuals");
        params.insert(CommandLine::ResetUserPreferences, "--reset-user-preferences");
        params.insert(CommandLine::UserClearLogin,       "--user-clear-log");
        params.insert(CommandLine::UserClearPassword,    "--user-clear-password");

        // insert default values
        value.insert(CommandLine::UserClearLogin, QVariant());
        value.insert(CommandLine::UserClearPassword, QVariant());
        value.insert(Core::Constants::CL_Test, false);
    }

    void parseCommandLine()
    {
        LOG_FOR("CommandLine", "Parsing command line");

        QStringList args = qApp->arguments();
        foreach(const QString &a, args) {
            LOG_FOR("CommandLine", a);
            QString k = a;
            if (k.contains(" "))
                k = k.left(k.indexOf(" "));
            if (a.contains("="))
                k = k.left(k.indexOf("="));
            switch (params.key(k,-1))
            {
            case Core::Constants::CL_Test :       value.insert(Core::Constants::CL_Test, true); break;
            case CommandLine::Chrono :            value.insert(CommandLine::Chrono, true); break;
            case CommandLine::ConfigFile :        value.insert(CommandLine::ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::RunningUnderWine:   value.insert(CommandLine::RunningUnderWine, true); break;
            case CommandLine::ClearUserDatabases: value.insert(CommandLine::ClearUserDatabases, true); break;
            case CommandLine::UserClearLogin:     value.insert(CommandLine::UserClearLogin, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::UserClearPassword:  value.insert(CommandLine::UserClearPassword, a.mid(a.indexOf("=")+1).remove("\"")); break;
            default : break;
        }
        }

        if (Utils::isReleaseCompilation()) {
            value.insert(CommandLine::ClearUserDatabases, false);
        }        
    }

public:
    QHash<int, QVariant> value;
    QHash<int, QString> params;
};
}
}

CommandLine::CommandLine(QObject *parent) :
    Core::ICommandLine(parent),
    d(new CommandLinePrivate)
{
    d->parseCommandLine();
}

CommandLine::~CommandLine()
{
    if (d)
        delete d;
    d=0;
}

QVariant CommandLine::value(int param, const QVariant &def) const
{
    return d->value.value(param,def);
}

QString CommandLine::paramName(int param) const
{
    if (d->params.keys().contains(param)) {
        return d->params.value(param);
    } else {
        // If a value is recorded return stringyfied param
        if (d->value.value(param).isValid())
            return QString::number(param);
    }
    return QString::null;
}
