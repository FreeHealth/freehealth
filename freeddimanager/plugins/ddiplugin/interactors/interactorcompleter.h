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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_INTERNAL_INTERACTORCOMPLETER_H
#define DRUGSDB_INTERNAL_INTERACTORCOMPLETER_H

#include <QCompleter>

/**
 * \file interactorcompleter.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 28 Oct 2012
*/

namespace DrugsDB {
namespace Internal {
class MoleculesInteractorCompleterPrivate;

class MoleculesInteractorCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit MoleculesInteractorCompleter(QObject *parent = 0);
    ~MoleculesInteractorCompleter();
    bool initialize();

Q_SIGNALS:

public Q_SLOTS:

private:
    Internal::MoleculesInteractorCompleterPrivate *d;
};

class ClassInteractorCompleterPrivate;

class ClassInteractorCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit ClassInteractorCompleter(QObject *parent = 0);
    ~ClassInteractorCompleter();
    bool initialize();

Q_SIGNALS:

public Q_SLOTS:

private:
    Internal::ClassInteractorCompleterPrivate *d;
};

} // namespace Internal
} // namespace DrugsDB

#endif // DRUGSDB_INTERNAL_INTERACTORCOMPLETER_H

