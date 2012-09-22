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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICDCONTEXTUALWIDGET_H
#define ICDCONTEXTUALWIDGET_H

#include <icdplugin/icd_exporter.h>

#include <QWidget>

/**
 * \file icdcontextualwidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 14 Oct 2010
 * \internal
*/


namespace ICD {
namespace Internal {
class IcdContext;
}

class ICD_EXPORT IcdContextualWidget : public QWidget
{
    Q_OBJECT
public:
    IcdContextualWidget(QWidget *parent = 0);
    virtual ~IcdContextualWidget();

private:
    Internal::IcdContext *m_Context;
};

}  // End namespace ICD

#endif // ICDCONTEXTUALWIDGET_H
