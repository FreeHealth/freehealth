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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKABOUTDIALOG_H
#define TKABOUTDIALOG_H

class tkAboutDialogPrivate;

#include <QDialog>
#include <tkExporter.h>

Q_TK_BEGIN_CONSTANTS
const char* const TEAM_CREATOR_MAINDEV     = QT_TRANSLATE_NOOP( "tkAboutDialog", "Creator & Main Developer" );
const char* const TEAM_DEV_CONTRIBUTORS    = QT_TRANSLATE_NOOP( "tkAboutDialog", "Developer" );
const char* const TEAM_DEV_DEBUGGER        = QT_TRANSLATE_NOOP( "tkAboutDialog", "Developer, Debugger" );
const char* const TEAM_HELPER              = QT_TRANSLATE_NOOP( "tkAboutDialog", "Helper" );
const char* const TEAM_HELPER_DEBUGGER     = QT_TRANSLATE_NOOP( "tkAboutDialog", "Helper, Debugger" );
const char* const TEAM_TESTER              = QT_TRANSLATE_NOOP( "tkAboutDialog", "Tester" );
Q_TK_END_CONSTANTS

struct Q_TK_EXPORT tkAboutDatas
{
    tkAboutDatas( const QString& n, const QString& p, const QString& e, const QString& c )
                : Name( n ),Country( p ), Email( e ), UnTranslatedComment( c ) {}
    QString Name;
    QString Country;
    QString Email;
    QString UnTranslatedComment;
};

class Q_TK_EXPORT tkAboutDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(tkAboutDialog)

public:
    enum AvailableLicense {
        LGPL,
        GPLv3,
        GPLv2,
        GPLv1,
        BSD,
        BSDModified
    };

    explicit tkAboutDialog(QWidget *parent = 0);
    virtual ~tkAboutDialog() {}

    void setApplicationName(const QString &appName);
    void setAboutText( const QString &htmlOrPlainText );
    void addTeamList( const QString &title, const QList<tkAboutDatas> &list );
    void setLicense( AvailableLicense license );
    void setLicense( const QString &html );
    void setCopyright( const QString &years, const QString &names );

    static QString getLicenseText(AvailableLicense license);

protected:
    virtual void done(int result);

private:
    tkAboutDialogPrivate *d;
};

#endif // TKABOUTDIALOG_H
