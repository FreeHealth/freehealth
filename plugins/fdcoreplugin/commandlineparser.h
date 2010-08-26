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
#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <coreplugin/icommandline.h>

#include <QString>
#include <QVariant>



namespace Core {
namespace Internal {
class CommandLinePrivate;
}
class Patient;

class CommandLine  : public Core::ICommandLine
{
public:
    enum Param {
        CL_MedinTux = 0,
        CL_EMR_Name,
        CL_EMR_Uid,
        CL_SelectionOnly,
        CL_DrugsDatabaseUid,
        CL_ExchangeFile,
        CL_ExchangeFileFormat,
        CL_ExchangeInFile,
        CL_PatientName,
        CL_PatientFirstname,
        CL_PatientUid,
        CL_PatientGender,
        CL_DateOfBirth,
        CL_Weight,
        CL_Weight_Unit,
        CL_Height,
        CL_Height_Unit,
        CL_CrCl,
        CL_CrCl_Unit,
        CL_Creatinine,
        CL_Creatinine_Unit,
        CL_DrugsAllergies,
        CL_InnAllergies,
        CL_AtcAllergies,
        CL_DrugsIntolerances,
        CL_InnIntolerances,
        CL_AtcIntolerances,
        CL_ICD10Diseases,
        CL_Chrono,
        CL_TransmitDosage,
        CL_ConfigFile,
        CL_RunningUnderWine,
        CL_BlockPatientDatas,
        CL_MaxParam
    };

    CommandLine();
    ~ CommandLine();

    QVariant value(int param, const QVariant &def = QVariant()) const;
    QString paramName(int param) const;

    void feedPatientDatas(Core::Patient *patient);

private:
    Internal::CommandLinePrivate *d;
};

}

#endif // COMMANDLINEPARSER_H
