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
/***************************************************************************
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_FSPPRINTERDIALOG_H
#define TOOLS_INTERNAL_FSPPRINTERDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file fspprinterdialog.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 23 Mar 2013
*/

namespace Tools {
namespace Internal {
class FspPrinterDialogPrivate;
class Fsp;

class FspPrinterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FspPrinterDialog(QWidget *parent = 0);
    ~FspPrinterDialog();
    bool initialize(const Fsp &fsp);

    static bool isAvailable();
    static QString datapackPath();

private Q_SLOTS:
    void toggleView(bool complex);
    void expandChildren(const QModelIndex &index);
    void useTemplate(const QModelIndex &index);
    void printFsp();
    void previewFsp();
    void printCheque();
    void updatePreview();

private:
    FspPrinterDialogPrivate *d;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_FSPPRINTERDIALOG_H

