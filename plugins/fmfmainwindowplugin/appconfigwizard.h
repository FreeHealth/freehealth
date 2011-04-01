/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef APPCONFIGWIZARD_H
#define APPCONFIGWIZARD_H

#include <fmfmainwindowplugin/mainwindow_exporter.h>

#include <QObject>
#include <QWidget>
#include <QWizardPage>
#include <QWizard>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

/**
 * \file appconfigwizard.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 01 Apr 2011
*/

namespace Form {
class FormFilesSelectorWidget;
}

namespace Core {
class IOptionsPage;
}

namespace MainWin {
namespace Internal {
class VirtualDatabasePreferences;
}  // End namespace Internal

class MAINWIN_EXPORT AppConfigWizard : public QWizard
{
    Q_OBJECT
public:
    AppConfigWizard(QWidget *parent = 0);

protected Q_SLOTS:
    void done(int r);

private:
};

class BeginConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    BeginConfigPage(QWidget *parent = 0);
    bool validatePage();

protected:
    void retranslate();

private:
    QLabel *intro;
    QLabel *langLabel;
};

class CreateNewUserPage: public QWizardPage
{
    Q_OBJECT
public:
    CreateNewUserPage(QWidget *parent = 0);
    bool validatePage();

private Q_SLOTS:
    void createNewUser();
//    void redefineAdminPassword();

private:
    QLabel *newUserName;
    bool passredefined;
};

class DatabaseConfigurationPage: public QWizardPage
{
    Q_OBJECT
public:
    DatabaseConfigurationPage(QWidget *parent = 0);
    ~DatabaseConfigurationPage();
    bool validatePage();

private:
    QList<Core::IOptionsPage *> pages;
};

class PatientFilePage: public QWizardPage
{
    Q_OBJECT
public:
    PatientFilePage(QWidget *parent = 0);
    bool validatePage();

private:
    Form::FormFilesSelectorWidget *selector;
};

class VirtualDatabasePage: public QWizardPage
{
    Q_OBJECT
public:
    VirtualDatabasePage(QWidget *parent = 0);
    void initializePage();

private:
    Internal::VirtualDatabasePreferences *vd;
};

class EndConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    EndConfigPage(QWidget *parent = 0);
};


}  // End namespace MainWin


#endif // APPCONFIGWIZARD_H
