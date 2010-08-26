/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
    const char * const  G_PATIENTS_INFORMATIONS = "Patients.groupInfos";

    const char * const  A_SEARCH_PATIENTS_BY_NAME         = "Patients.SearchByName";
    const char * const  A_SEARCH_PATIENTS_BY_FIRSTNAME    = "Patients.SearchFirstname";
    const char * const  A_SEARCH_PATIENTS_BY_NAMEFIRSTNAME  = "Patients.SearchNameFirstname";
    const char * const  A_SEARCH_PATIENTS_BY_DOB          = "Patients.SearchByDOB";

    const char * const  A_VIEWPATIENT_INFOS               = "Patient.Informations";
    const char * const  A_VIEWPATIENTDATABASE_INFOS       = "Patient.DB.Informations";


    const char * const  C_PATIENTS             = "Patients.Context";
    const char * const  C_PATIENTS_SEARCH      = "Patients.Context.Search";

    const char * const  MODE_SEARCH_UNIQUENAME = "PatientModeSearch";
    const int           P_MODE_SEARCH          = 1;

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // PATIENTS_MENUS_CONSTANTS_H
