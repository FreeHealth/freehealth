/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_LIB_SERVERCREATIONWIDGET_H
#define DATAPACK_LIB_SERVERCREATIONWIDGET_H

#include <QWidget>

/**
 * \file servercreationwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 29 Jun 2014
*/

namespace DataPack {
namespace Internal {
class ServerCreationWidgetPrivate;
} // namespace Internal

class ServerCreationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ServerCreationWidget(QWidget *parent = 0);
    ~ServerCreationWidget();

    void setDefaultScreeningPath(const QString &absPath);
    bool addScreeningPath(const QString &absPath);
    int numberOfCheckedPacks() const;

    void setDefaultServerOutputPath(const QString &absPath);

public Q_SLOTS:
    bool onCreateServerRequested();

private Q_SLOTS:
    bool onAddScreeningPathButtonClicked();
    void updateTotalNumberOfPacks();

private:
    void retranslate();
    void changeEvent(QEvent *e);

private:
    Internal::ServerCreationWidgetPrivate *d;
};

} // namespace DataPack

#endif // DATAPACK_LIB_SERVERCREATIONWIDGET_H

