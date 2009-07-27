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
#include "mfAboutDialog.h"

#include <tkSettings.h>
#include <tkScrollingWidget.h>
#include <tkConstantTranslations.h>

#include <QApplication>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

namespace mfAboutDialogPrivateConstants {

const char* const  UPDATE_DRUGS       = QT_TRANSLATE_NOOP( "diAboutDialog", "Drugs updated on 26/06/2009");
const char* const  UPDATE_IAM         = QT_TRANSLATE_NOOP( "diAboutDialog", "Interactions updated on 30/06/2009");
const char* const  COPYRIGHT_YEARS    = "2008-2009";
const char* const  COPYRIGHT_NAMES    = QT_TRANSLATE_NOOP( "diAboutDialog", "Eric MAEKER, MD and the FreeMedForms Team" );
const tkAboutDialog::AvailableLicense  LICENCE = tkAboutDialog::BSD;

const char* const COUNTRY_FRANCE      = QT_TRANSLATE_NOOP( "diAboutDialog", "France" );
}

using namespace mfAboutDialogPrivateConstants;

const QList<tkAboutDatas> m_Creator = QList<tkAboutDatas>()
        << tkAboutDatas( "Eric MAEKER, MD", COUNTRY_FRANCE, "eric.maeker@free.fr",          TEAM_CREATOR_MAINDEV );

const QList<tkAboutDatas> m_Contributors = QList<tkAboutDatas>()
        << tkAboutDatas( "Guillaume Denry", COUNTRY_FRANCE, "guillaume.denry@gmail.com",    TEAM_DEV_DEBUGGER )
        << tkAboutDatas( "Filipe Azevedo", COUNTRY_FRANCE, "pasnox@gmail.com",              TEAM_HELPER_DEBUGGER )
        << tkAboutDatas( "David Geldreich", COUNTRY_FRANCE, "david.geldreich@free.fr",      TEAM_HELPER_DEBUGGER )
        << tkAboutDatas( QString::fromUtf8("Sébastien Belin"), COUNTRY_FRANCE, "belin.s@wanadoo.fr", TEAM_TESTER )
        << tkAboutDatas( "Jourdan Romain", COUNTRY_FRANCE, "No@Mail.com",                   TEAM_HELPER );

const QList<tkAboutDatas> m_Database = QList<tkAboutDatas>()
        << tkAboutDatas( "Eric MAEKER, MD", COUNTRY_FRANCE, "eric.maeker@free.fr", TEAM_CREATOR_MAINDEV );


mfAboutDialog::mfAboutDialog(QWidget *parent) :
    tkAboutDialog(parent)
{
    addTeamList( tkTr(CREATOR), m_Creator );
    addTeamList( tkTr(DEVELOPPERS_CONTRIBUTORS), m_Contributors );
    addTeamList( tkTr(DRUG_DATABASE_MANAGER), m_Database );

    setAboutText( tr( "%1 is an early release. All features may not work and you may experiment bugs.<br>"
                      "This software is release without any warranty and only for test purposal.<br>"
                      "Please refer to web site for more informations.<br>"
                      "<a href=\"%2\">Web site</a>")
                  .arg(qApp->applicationName() )
                  .arg(tkSettings::instance()->path(tkSettings::WebSiteUrl)));

    setLicense(LICENCE);
    setCopyright(COPYRIGHT_YEARS, COPYRIGHT_NAMES);

    // check registration
//    QString sn = tkSettings::instance()->value( MFDRUGS_SETTING_SERIAL ).toString();
//    if (tkSerialNumber::isValid(sn))
//        registerLbl->setText(tr("Registered version."));
//    else
//        registerLbl->setText(tr("Unregistered version. You can register Drugs Interactions. Please visit the web site."));
}

//void diAboutDialog::done(int result)
//{
////    if (result == QDialog::Accepted) {
////        if (tkSerialNumber::isValid(serialLineEdit->text()))
////            tkSettings::instance()->setValue( MFDRUGS_SETTING_SERIAL, serialLineEdit->text() );
////    }
//    QDialog::done(result);
//}

