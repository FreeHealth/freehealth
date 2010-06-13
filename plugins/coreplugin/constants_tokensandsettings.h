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
 ***************************************************************************/
#ifndef CORE_CONSTANTS_TOKENS_SETTINGS_H
#define CORE_CONSTANTS_TOKENS_SETTINGS_H

namespace Core {

namespace Constants {

// TEXT COMPLETION tokens

// For Patients
const char * const  TOKEN_PATIENTAGE        = "AGE";           /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_PATIENTYEARSOLD   = "YEARSOLD";           /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_PATIENTFULLNAME   = "PATIENTFULLNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_PATIENTNAME       = "PATIENTNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_PATIENTSECONDNAME = "PATIENTSECONDNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_PATIENTSURNAME    = "PATIENTSURNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_PATIENTDATEOFBIRTH= "DATEOFBIRTH";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_WEIGHT            = "WEIGHT";        /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_HEIGHT            = "HEIGHT";          /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_CLCR              = "CLCR";          /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_DATE              = "DATE";
const char * const  TOKEN_NUMBEROFDRUGS     = "NBDRUGS";

// For Users
const char * const  TOKEN_USERAGE        = "USERAGE";           /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERYEARSOLD   = "USERYEARSOLD";           /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERFULLNAME   = "USERFULLNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERNAME       = "USERNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERSECONDNAME = "USERSECONDNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERSURNAME    = "USERSURNAME";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERSPECIALITIES    = "USERSPECIALITIES";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERIDENTIFIANTS    = "USERIDENTIFIANTS";   /*!< /brief Tokens for Patient::replaceTokens() */
const char * const  TOKEN_USERQUALIFICATIONS    = "USERQUALIFICATIONS";   /*!< /brief Tokens for Patient::replaceTokens() */

// Some path tags
const char * const  TAG_APPLICATION_RESOURCES_PATH = "%appResources%";
const char * const  TAG_APPLICATION_THEME_PATH     = "%theme%";

// Settings keys
const char * const  S_RECENTFILES_GROUP = "RecentFiles";
const char * const  S_RECENTFILES_KEY   = "File";
const char * const  S_PATIENT_UUID_HISTORY = "PatientsUuids";
const char * const  S_SAVEINTERVAL      = "Core/SaveInterval";
const char * const  S_OPENLAST          = "Core/OpenLastOpenedFile";
const char * const  S_SHOWHELPTEXT      = "Core/ShowFormHelpText";
const char * const  S_LASTLOGIN         = "Login/userLogin";
const char * const  S_LASTPASSWORD      = "Login/userPassword";
const char * const  S_DATEFORMAT        = "Dates/Format";

const char * const  S_PATIENTFORMS_FILENAME  = "Core/PatientFormsFile";

const char * const  S_USE_EXTERNAL_DATABASE  = "ExternalDatabase/UseIt";
const char * const  S_EXTERNAL_DATABASE_LOG  = "ExternalDatabase/Log";
const char * const  S_EXTERNAL_DATABASE_PASS = "ExternalDatabase/Pass";
const char * const  S_EXTERNAL_DATABASE_HOST = "ExternalDatabase/Host";
const char * const  S_EXTERNAL_DATABASE_PORT = "ExternalDatabase/Port";

const char * const  S_ALWAYS_SAVE_WITHOUT_PROMPTING = "Core/SaveInDatabaseWithoutPrompringUser";

}  // end Constants
} // end Core

#endif // CORE_CONSTANTS_TOKENS_SETTINGS_H
