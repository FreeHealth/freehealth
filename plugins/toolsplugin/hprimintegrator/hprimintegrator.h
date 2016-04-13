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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_HPRIMINTEGRATOR_H
#define TOOLS_INTERNAL_HPRIMINTEGRATOR_H

#include <coreplugin/modemanager/imode.h>
#include <QObject>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file hprimintegrator.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 24 May 2013
*/

namespace Tools {
namespace Internal {
class HprimIntegratorWidgetPrivate;

class HprimIntegratorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HprimIntegratorWidget(QWidget *parent = 0);
    ~HprimIntegratorWidget();
    
    bool initialize();
    void refreshSettings();

private Q_SLOTS:
    void onFileSelected(const QModelIndex &current, const QModelIndex &);
    void onPatientSelected(const QString &fullName, const QString &uid);
    void onDataIntegrationRequested();

private:
    HprimIntegratorWidgetPrivate *d;
};

class HprimIntegratorMode : public Core::IMode
{
    Q_OBJECT
public:
    HprimIntegratorMode(QObject *parent = 0);
    ~HprimIntegratorMode();

    void refreshSettings();

private:
    HprimIntegratorWidget *_widget;
};
} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_HPRIMINTEGRATOR_H

