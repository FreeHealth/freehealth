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
#ifndef TKUSERIDENTIFIER_H
#define TKUSERIDENTIFIER_H

// include usertoolkit headers
#include <tkUserExporter.h>
class tkUserIdentifierPrivate;

// include Qt headers
#include <QDialog>

/**
 * \file tkUserIdentifier.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 07 March 2009
*/

/**
  \brief This class is a dialog that ask user for is login/password, with a limited number of tries.
  You can show some informations on the left using the first parameter of the constructor.
  If there is not informations to show, the informations' groupBox is hidden.

  When the identification is good :
  - the dialog result() is setted to QDialog::Accepted,
  - current user is setted into user's model with the login and password of this dialog,
  - the lastLogin information is saved into database,
  - the login history is completed.

  In the other case, it is setted to QDialog::Rejected.

  You can retreive cryptedLogin and cryptedPassword using : login() and cryptedPassword().
  \ingroup usertoolkit widget_usertoolkit
  \ingroup usermanager
*/


class Q_TKUSER_EXPORT tkUserIdentifier : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(tkUserIdentifier);

    enum { MaxNumberOfTries = 4 };

public:
    explicit tkUserIdentifier( const QStringList & informations = QStringList(), QWidget *parent = 0 );

    QString  login();
    QString  cryptedPassword();

private:
    void done( int result );

private:
    tkUserIdentifierPrivate * d;
};

#endif // TKUSERIDENTIFIER_H
