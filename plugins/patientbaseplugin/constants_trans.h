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
#ifndef PATIENTS_TRANS_CONSTANTS_H
#define PATIENTS_TRANS_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

namespace Patients {
namespace Constants {

    const char* const   TRANS_CONTEXT             = "Patients";

    const char* const   SEARCHBYNAME_TEXT         = QT_TRANSLATE_NOOP("Patients", "Search by Name");
    const char* const   SEARCHBYFIRSTNAME_TEXT      = QT_TRANSLATE_NOOP("Patients", "Search by Firstname");
    const char* const   SEARCHBYNAMEFIRSTNAME_TEXT  = QT_TRANSLATE_NOOP("Patients", "Search by Name and Firstname");
    const char* const   SEARCHBYDOB_TEXT          = QT_TRANSLATE_NOOP("Patients", "Search by Date Of Birth");

    const char* const   SEARCHBYNAME_TOOLTIP         = QT_TRANSLATE_NOOP("Patients", "Type the name or second name of the patient");
    const char* const   SEARCHBYFIRSTNAME_TOOLTIP      = QT_TRANSLATE_NOOP("Patients", "Type the firstname of the patient");
    const char* const   SEARCHBYNAMEFIRSTNAME_TOOLTIP  = QT_TRANSLATE_NOOP("Patients", "Type the name or second name followed by ; and the firstname");
    const char* const   SEARCHBYDOB_TOOLTIP          = QT_TRANSLATE_NOOP("Patients", "Type the date of birth (yyyy/MM/dd)");

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // PATIENTS_TRANS_CONSTANTS_H
