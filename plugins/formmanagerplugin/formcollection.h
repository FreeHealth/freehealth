/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_FORMCOLLECTION_H
#define FORM_FORMCOLLECTION_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>

/**
 * \file formcollection.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 15 Sept 2012
*/

namespace Form {
class FormManager;
namespace Internal {
class FormCollectionPrivate;
}  // namespace Internal

class FORM_EXPORT FormCollection : public QObject
{
    Q_OBJECT
    friend class Form::FormManager;

protected:
    explicit FormCollection(QObject *parent = 0);

public:
    ~FormCollection();
    
    bool initialize();
    
Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    Internal::FormCollectionPrivate *d;
};

} // namespace Form

#endif  // FORM_FORMCOLLECTION_H

