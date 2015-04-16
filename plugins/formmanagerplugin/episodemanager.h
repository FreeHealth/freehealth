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
 *   Main developers : Eric MAEKER
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_INTERNAL_EPISODEMANAGER_H
#define FORM_INTERNAL_EPISODEMANAGER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>

/**
 * \file episodemanager.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 02 Sept 2013
*/

namespace Form {
class EpisodeModel;
class FormMain;
class FormCore;

namespace Internal {
class EpisodeManagerPrivate;
} // namespace Internal

class FORM_EXPORT EpisodeManager : public QObject
{
    Q_OBJECT
    friend class Form::FormCore;

protected:
    explicit EpisodeManager(QObject *parent = 0);
    bool initialize();

public:
    ~EpisodeManager();
    bool isInitialized() const;

    EpisodeModel *episodeModel(Form::FormMain *form);
    EpisodeModel *episodeModel(const QString &formUid);

private:
    Internal::EpisodeManagerPrivate *d;
};

} // namespace Form

#endif // FORM_INTERNAL_EPISODEMANAGER_H

