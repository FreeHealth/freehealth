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
 *   Main Developer: Fredman, Eric Maeker <eric.maeker@gmail.com>          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "patientbase.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>

using namespace AxiSante4;
using namespace Trans::ConstantTranslations;

namespace {
const char * const DB_NAME = "AXISANTE4";
}

PatientBase::PatientBase(QObject *parent) :
    QObject(parent)
{
    setObjectName("Axisante4::PatientBase");
}

void PatientBase::setDatabasePath(const QString &absFilePath)
{
    _dbFilePath = QDir::cleanPath(absFilePath);
}

bool PatientBase::initialize()
{
    Q_ASSERT(_dbFilePath.isEmpty());
    if (!QSqlDatabase::connectionNames().contains(::DB_NAME)) {
        QSqlDatabase DB = QSqlDatabase::addDatabase("QODBC", ::DB_NAME);
        DB.setDatabaseName(_dbFilePath);
    }
    QSqlDatabase DB = QSqlDatabase::database(::DB_NAME);
    if (!DB.isOpen()){
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(::DB_NAME).arg(DB.lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(DB.connectionName()).arg(DB.driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(DB.connectionName()).arg(DB.driverName()));
    }
    return true;
}

bool PatientBase::startDataExtraction()
{
    return true;
}

bool PatientBase::extractNextPatient()
{
    // select from the table FPatient
    QSqlQuery query;
    query.prepare(QString("select * from FPatient WHERE (NCEPatient>10 AND NCEPatient<20)"));
    query.exec();
    if (query.isActive()) {
        while (query.next()) {
//            patientptr->setindex(query.value(0).toString());
//            patientptr->setnom(query.value(4).toString());
//            patientptr->setprenom(query.value(57).toString());
//            patientptr->setdatedenaissance(query.value(12).toString());
//            patientptr->setadresse(query.value(5).toString());
//            patientptr->setcodepostal(query.value(7).toString());
//            patientptr->setville(query.value(8).toString());
//            patientptr->settextelibre(query.value(29).toString());
//            patientptr->settelephone1(query.value(9).toString());
//            patientptr->settelephone2(query.value(10).toString());
//            xmlwriterblock patientblock(fileptr, patientptr);
        }
    }
    return true;
}
