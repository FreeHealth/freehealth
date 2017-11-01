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
#include "commandlineparser.h"

#include <coreplugin/ipatient.h>
#include <coreplugin/icommandline.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>

#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDomElement>
#include <QDomDocument>

#include <QDebug>

using namespace Core;
using namespace Internal;

CommandLine::CommandLine() :
        Core::ICommandLine()
//    d(new CommandLinePrivate)
{
//    d->parseCommandLine();
}

CommandLine::~CommandLine()
{
//    if (d)
//        delete d;
//    d=0;
}

void CommandLine::feedPatientDatas(Core::IPatient *patient)
{
//    d->feedPatientDatas(patient);
}

QVariant CommandLine::value(int param, const QVariant &def) const
{
    // TODO: code heree */
    return QVariant();
//    return d->value.value(param,def);
}

QString CommandLine::paramName(int param) const
{
    // TODO: code heree */
//    if (d->params.keys().contains(param))
//        return d->params.value(param);
//    else
//        return QString::number(param);
    return QString::null;
}
