/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef IUSERWIZARDPAGE_H
#define IUSERWIZARDPAGE_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <coreplugin/igenericpage.h>

#include <QWizardPage>

/**
 * \file iuserwizardpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 30 Jul 2011
 */

namespace UserPlugin {
class UserModel;

class USER_EXPORT IUserWizardPage : public Core::IGenericPage
{
    Q_OBJECT
public:
    /** IUserWizardPage is an extension of the user creator QWizard. \sa UserPlugin::UserWizard. */
    explicit IUserWizardPage(QObject *parent = 0) : Core::IGenericPage(parent) {}
    virtual ~IUserWizardPage() {}

    /** Create the QWizardPage to include in the user creator QWizard. */
    virtual QWizardPage *createWizardPage(QWidget *parent) = 0;

    /** When the user creator QWizard is validated, this member is called to allow extra page
     * to submit their data to their own database. */
    virtual void submit(const QString &userUid) = 0;
};

}  // End namespace UserPlugin


#endif // IUSERWIZARDPAGE_H
