/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers :                                                     *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_FORMCORE_H
#define FORM_FORMCORE_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>

/**
 * \file formcore.h
 * \author Eric MAEKER
 * \version 0.9.0
 * \date 02 Sept 2013
*/

namespace Form {
class FormManager;
class EpisodeManager;
class PatientFormItemDataWrapper;

namespace Internal {
class FormManagerPlugin;
class FormCorePrivate;
}  // namespace Internal

class FORM_EXPORT FormCore : public QObject
{
    Q_OBJECT
    friend class Form::Internal::FormManagerPlugin;

protected:
    explicit FormCore(QObject *parent = 0);
    bool initialize();

public:
    static FormCore &instance();
    ~FormCore();
    bool isInitialized() const;

    Form::FormManager &formManager() const;
    Form::EpisodeManager &episodeManager() const;
    Form::PatientFormItemDataWrapper &patientFormItemDataWrapper() const;

Q_SIGNALS:

public Q_SLOTS:
    void activatePatientFileCentralMode();

private:
    Internal::FormCorePrivate *d;
    static FormCore *_instance;
};

} // namespace Form

#endif  // FORM_FORMCORE_H

