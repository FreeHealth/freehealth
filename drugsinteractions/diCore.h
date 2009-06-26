/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
 *   This code is inspired of the Monkey Studio project                    *
 *   http://www.monkeystudio.org/                                          *
 *   Filipe AZEVEDO aka Nox P@sNox <pasnox@gmail.com>                      *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DICORE_H
#define DICORE_H

// include drugsinteractions headers
#include "diMainWindow.h"

// include toolkit headers
class tkSettings;
class tkTranslators;
class tkUpdateChecker;

// include medintux toolkit headers
class tkMedintuxConfiguration;

// including drugswidget headers
class mfDrugsModel;

// include Qt headers
QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

/**
 * \file diCore.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.3
 * \date 08 Jun 2009
   \brief Core manager for DrugsInteractions
   \ingroup drugsinteractions
*/

class diCore
{
public:
    static bool init();

    // Application objects (widgets, toolbars, specific object like updateChecker...)
    static tkSettings*       settings();
    static diMainWindow*     mainWindow();
    static tkTranslators*    translators();
    static tkUpdateChecker * updateChecker();
    static mfDrugsModel *    drugsModel();
    static tkMedintuxConfiguration *medintuxConfiguration();

    // Patient management
    static QString & patientName();
    static void setPatientName( const QString &name);
    static QString & patientDateOfBirth();
    static QString & patientAge();
    static void setPatientDateOfBirth( const QString &date);
    static QString & patientWeight();
    static QString & patientSize();
    static QString & patientClCr();

};

#endif // DICORE_H
