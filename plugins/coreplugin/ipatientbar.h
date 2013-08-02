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
#ifndef CORE_IPATIENTBAR_H
#define CORE_IPATIENTBAR_H

#include <coreplugin/core_exporter.h>
#include <QWidget>

/**
 * \file ipatientbar.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.4
 * \date 04 Mar 2013
*/

namespace Core {

class CORE_EXPORT IPatientBar : public QWidget
{
    Q_OBJECT

public:
    explicit IPatientBar(QWidget *parent = 0) : QWidget(parent) {}
    virtual ~IPatientBar() {}

    virtual void addBottomWidget(QWidget *widget) = 0;
    virtual void showMessage(const QString &message, int duration_ms = 2000, const QString &css = QString::null) = 0;
};

} // namespace Core

#endif // CORE_IPATIENTBAR_H
