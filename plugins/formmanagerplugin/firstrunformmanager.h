/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FIRSTRUNFORMMANAGER_H
#define FIRSTRUNFORMMANAGER_H

#include <coreplugin/ifirstconfigurationpage.h>

#include <QWizardPage>

/**
 * \file firstrunformmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 16 May 2011
*/

namespace Form {
class FormFilesSelectorWidget;

namespace Internal {

class FirstRunFormManagerWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    FirstRunFormManagerWizardPage(QWidget *parent);

    void initializePage();
    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    FormFilesSelectorWidget *selector;
};

class FirstRunFormManagerConfigPage : public Core::IFirstConfigurationPage
{
public:
    FirstRunFormManagerConfigPage(QObject *parent) : IFirstConfigurationPage(parent) {}
    int id() const {return Core::IFirstConfigurationPage::PatientForm;}
    QWizardPage *createPage(QWidget *parent) {return new FirstRunFormManagerWizardPage(parent);}
};


}  // End namespace Internal
}  // End namespace Form

#endif // FIRSTRUNFORMMANAGER_H
