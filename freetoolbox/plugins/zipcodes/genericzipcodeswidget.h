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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GENERICZIPCODESWIDGET_H
#define GENERICZIPCODESWIDGET_H

#include <QWidget>
#include "genericzipcodesstep.h"

QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

namespace ZipCodes {
namespace Ui {
class GenericZipCodesWidget;
}

class GenericZipCodesWidget : public QWidget
{
    Q_OBJECT
public:
    GenericZipCodesWidget(QWidget *parent = 0);
    ~GenericZipCodesWidget();

protected Q_SLOTS:
    void on_downloadButton_clicked();
    void on_populateDbButton_clicked();
    void on_readCountries_clicked();
    void onDownloadFinished();
    void onProcessFinished();
    void onPostDownloadProcessFinished();

    void selectCountry(const QModelIndex &index);
    void selectCurrentCountry();
    void deselectCountry(const QModelIndex &index);
    void deselectCurrentCountry();
    void updateActions();
    void on_createPackageButton_clicked();

private:
    Ui::GenericZipCodesWidget *ui;
    GenericZipCodesStep *m_Step;
    QStandardItemModel *m_availableCountriesModel;
    QStandardItemModel *m_selectedCountriesModel;
};

} // end ZipCodes
#endif // GENERICZIPCODESWIDGET_H
