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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_MENUS_CONSTANTS_H
#define PATIENTS_MENUS_CONSTANTS_H

namespace Patients {
namespace Constants {

    const char * const  M_PATIENTS              = "Patients.menuDrugs";
    const char * const  M_PATIENTS_SEARCH       = "Patients.menuSearch";
    const char * const  M_PATIENTS_MODES        = "Patients.menuModes";

    const char * const  G_PATIENTS              = "Patients.groupDrugs";
    const char * const  G_PATIENTS_SEARCH       = "Patients.groupSearch";
    const char * const  G_PATIENTS_MODE         = "Patients.groupModes";
    const char * const  G_PATIENTS_NEW          = "Patients.groupNew";
    const char * const  G_PATIENTS_HISTORY      = "Patients.groupHistory";
    const char * const  G_PATIENTS_INFORMATION  = "Patients.groupInfo";

    const char * const  A_SEARCH_PATIENTS_BY_NAME         = "Patients.SearchByName";
    const char * const  A_SEARCH_PATIENTS_BY_FIRSTNAME    = "Patients.SearchFirstname";
    const char * const  A_SEARCH_PATIENTS_BY_NAMEFIRSTNAME  = "Patients.SearchNameFirstname";
    const char * const  A_SEARCH_PATIENTS_BY_DOB          = "Patients.SearchByDOB";

    const char * const  A_VIEWPATIENT_INFOS               = "Patient.Information";
    const char * const  A_VIEWPATIENTDATABASE_INFOS       = "Patient.DB.Information";
    const char * const  A_VIEWCURRENTPATIENTDATA          = "Patient.CurrentData";

    const char * const  A_EXPORTPATIENTFILE               = "Patient.ExportPatientFile";


    const char * const  C_PATIENTS             = "Patients.Context";
    const char * const  C_PATIENTS_SEARCH      = "Patients.Context.Search";

    const char * const  MODE_SEARCH_UNIQUENAME = "PatientModeSearch";
    const int           P_MODE_SEARCH          = 1;

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // PATIENTS_MENUS_CONSTANTS_H
