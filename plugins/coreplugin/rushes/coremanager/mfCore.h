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
#ifndef MFCORE_H
#define MFCORE_H

#include <QObject>
#include <QHash>

// include freemedforms headers
#include "mfExporter.h"
class mfSettings;
class mfMainWindow;
class mfMainWidget;
class mfScriptor;
//class mfUpdateChecker;
class mfPluginsManager;

// include toolkit headers
class tkTranslators;
class tkActionManager;
class tkUpdateChecker;

// include usertoolkit headers
class tkUserModel;

// include Qt headers
QT_BEGIN_NAMESPACE
class QTemporaryFile;
QT_END_NAMESPACE

/**
 * \file mfCore.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.10
 * \date 09 Aug 2009
*/

class Q_FMF_EXPORT Core : public ICore
{
    Q_OBJECT
public:
    static Core *instance();

    static bool init();
    static void endOfApplication();

    // Application objects (widgets, toolbars, specific object like updateChecker...)
    static mfSettings*       settings();
    static mfMainWindow*     mainWindow();
    static mfMainWidget*     mainWidget();
    static mfScriptor*       scriptor();
    static tkUpdateChecker*  updateChecker();
    static tkTranslators*    translators();
    static mfPluginsManager* pluginsManager();
    static void              changeLanguage( const QString & lang );
    static void              renewTemporaryFile();
    static QTemporaryFile *  temporaryFile();
    static tkUserModel *     userModel();
    static tkActionManager * actionManager();

};

#endif
