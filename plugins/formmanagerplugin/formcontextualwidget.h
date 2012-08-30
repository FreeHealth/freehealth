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
#ifndef FORMCONTEXTUALWIDGET_H
#define FORMCONTEXTUALWIDGET_H

#include <QWidget>

/**
 * \file formcontextualwidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 23 Mar 2011
 * \internal
*/

namespace Form {
namespace Internal {
class FormContext;
}

class FormContextualWidget : public QWidget
{
    Q_OBJECT
public:
    FormContextualWidget(QWidget *parent = 0);
    virtual ~FormContextualWidget();

    void addContexts(const QList<int> &contexts);
    QList<int> contexts() const;

private:
    Internal::FormContext *m_Context;
};

}  // End namespace Form

#endif // FORMCONTEXTUALWIDGET_H
