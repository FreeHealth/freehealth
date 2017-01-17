/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QDate>
#include <QModelIndex>

namespace Ui {
    class AssetsViewerWidget;
}

class ACCOUNT_EXPORT AssetsViewer : public QWidget
{
    Q_OBJECT
public:
    AssetsViewer(QWidget *parent = 0);
    ~AssetsViewer();
    enum itemValues {
        LABEL = 0,
        YEARLY_VALUE
        };

private Q_SLOTS:
    void recordAsset();
    void deleteAsset();
    void yearDateChanged(const QDate &year);
    void writeLabelByRow(const QModelIndex &index);
    void userIsChanged();
    void quitFreeAccount();

private:
    void fillModeComboBox();
    void fillBankComboBox();
    void refresh();
    void showAssets();
    void changeEvent(QEvent *e);
    Ui::AssetsViewerWidget *ui;
};

#endif

