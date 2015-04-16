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
#ifndef TOOLS_CHEQUEPRINTERDIALOG_H
#define TOOLS_CHEQUEPRINTERDIALOG_H

#include <toolsplugin/tools_exporter.h>
#include <QDialog>
#include <QDate>

/**
 * \file chequeprinterdialog.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 27 Mar 2013
 * \note requires Qt 4.8+
*/

namespace Tools {
namespace Internal {
class ChequePrintFormatModel;
}

namespace Ui {
class ChequePrinterDialog;
}

class TOOLS_EXPORT ChequePrinterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChequePrinterDialog(QWidget *parent = 0);
    ~ChequePrinterDialog();
    static bool isAvailable();
    static QString datapackPath();

    void initializeWithSettings();

    void setOrder(const QString &order);
    void setPlace(const QString &place);
    void setDate(const QDate &date);
    void setAmount(double amount);
    void setDefaultAmounts(const QStringList &values);

private:
    void done(int result);

private Q_SLOTS:
    bool printCheque();
    void previewCheque();

private:
    Ui::ChequePrinterDialog *ui;
    Internal::ChequePrintFormatModel *_printFormatModel;
};

} // namespace Tools

#endif // TOOLS_CHEQUEPRINTERDIALOG_H
