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
#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <coreplugin/core_exporter.h>

#include <QDialog>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

/**
 * \file plugindialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 08 June 2010
*/


namespace ExtensionSystem {
class PluginSpec;
class PluginView;
}

namespace Core {

class CORE_EXPORT PluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginDialog(QWidget *parent);

private Q_SLOTS:
    void updateButtons();
    void openDetails();
    void openDetails(ExtensionSystem::PluginSpec *spec);
    void openErrorDetails();

private:
    ExtensionSystem::PluginView *m_view;

    QPushButton *m_detailsButton;
    QPushButton *m_errorDetailsButton;
    QPushButton *m_closeButton;
};

} // namespace Core

#endif // PLUGINDIALOG_H
