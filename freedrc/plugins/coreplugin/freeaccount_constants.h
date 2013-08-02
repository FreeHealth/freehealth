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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEACCOUNT_CONSTANTS_H
#define FREEACCOUNT_CONSTANTS_H

/**
 * \file accountwidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.5
 * \date 25 Jun 2012
 * \internal
*/

namespace Core {
namespace Constants {

// Enum for the command line params that can be used with
// Core::ICore::instance()->commandLine()
// Anywhere in the freeaccount code
enum FreeAccountCommandLine {
    CL_MedinTux = 0,
    CL_Independant,
    CL_Movements,
    CL_PreferedReceipts,
    CL_Test,
    CL_ReceiptsCreator,
    CL_EMR_Name,
    CL_EMR_Uid,
    CL_SelectionOnly,
    CL_ExchangeOutFile,
    CL_ExchangeOutFileFormat,
    CL_ExchangeInFile,
    CL_PatientName,
    CL_PatientFirstname,
    CL_PatientUid,
    CL_PatientGender,
    CL_DateOfBirth,
    CL_BlockPatientDatas,
    CL_DbHost,
    CL_DbPort,
    CL_MaxParam
};
}
}

#endif // FREEACCOUNT_CONSTANTS_H
