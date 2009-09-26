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
#ifndef TKUSERWIZARD_H
#define TKUSERWIZARD_H

#include <usermanagerplugin/usermanager_exporter.h>

namespace Utils {
class LineEditEchoSwitcher;
}

#include <QObject>
#include <QWidget>
#include <QWizardPage>
#include <QWizard>
class QLabel;
class QEvent;
class QLineEdit;
class QPushButton;


/**
 * \file userwizard.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.6
 * \date 17 Sept 2009
*/

namespace UserPlugin {

/** \todo create a new wizard manager in Core */

class USER_EXPORT UserWizard : public QWizard
{
    Q_OBJECT
public:
    UserWizard(QWidget *parent = 0);

    void setModelRow(const int row)        { m_Row = row; }
    void createUser(bool state = true)     { m_CreateUser = state; }

protected Q_SLOTS:
    void done(int r);

private:
    int m_Row;
    bool m_Saved, m_CreateUser;
};


class UserLanguageSelectorPage: public QWizardPage
{
    Q_OBJECT
public:
    UserLanguageSelectorPage(QWidget *parent = 0);
private:
    void changeEvent(QEvent *e);
    void retranslate();
    QLabel * lbl;
};

class UserIdentityPage: public QWizardPage
{
    Q_OBJECT
public:
    UserIdentityPage(QWidget *parent = 0);
    bool validatePage();
};

class UserAdressPage: public QWizardPage
{
    Q_OBJECT
public:
    UserAdressPage(QWidget *parent = 0);
};

class UserTelsAndMailPage: public QWizardPage
{
    Q_OBJECT
public:
    UserTelsAndMailPage(QWidget *parent = 0);
    bool validatePage();
};

class UserLoginPasswordPage: public QWizardPage
{
    Q_OBJECT
public:
    UserLoginPasswordPage(QWidget *parent = 0);
    bool validatePage();
private:
    QPushButton *echoConfirmPass, *echoPass;
    Utils::LineEditEchoSwitcher *leLogin, *lePassword, *lePasswordConfirm;
};

class UserSpecialiesQualificationsPage: public QWizardPage
{
    Q_OBJECT
public:
    UserSpecialiesQualificationsPage(QWidget *parent = 0);
};

class UserRightsPage: public QWizardPage
{
    Q_OBJECT
public:
    UserRightsPage(QWidget *parent = 0);
};

class UserGenericPage: public QWizardPage
{
    Q_OBJECT
public:
    UserGenericPage(QWidget *parent = 0);
};

class UserPrescriptionsPage: public QWizardPage
{
    Q_OBJECT
public:
    UserPrescriptionsPage(QWidget *parent = 0);
};

class UserAdministrativePage: public QWizardPage
{
    Q_OBJECT
public:
    UserAdministrativePage(QWidget *parent = 0);
};

}  // End UserPlugin


#endif // TKUSERWIZARD_H
