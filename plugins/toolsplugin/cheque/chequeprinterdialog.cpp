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
 *   Main Developer: Eric Maeker <eric.maeker@gmail.com>                  *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "chequeprinterdialog.h"
#include "chequeprinter.h"
#include "chequeconstants.h"
#include "chequeprintformat.h"
#include "chequeprintformatmodel.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/imageviewer.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>

#include "ui_chequeprinterdialog.h"

#include <QPushButton>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Tools {

ChequePrinterDialog::ChequePrinterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChequePrinterDialog),
    _printFormatModel(0)
{
    ui->setupUi(this);
    ui->valueLineEdit->setFocus();
    _printFormatModel = new Internal::ChequePrintFormatModel(this);
    _printFormatModel->initialize();
    ui->chequeFormat->setModel(_printFormatModel);
    ui->chequeFormat->selectionModel()->setCurrentIndex(_printFormatModel->index(0,0), QItemSelectionModel::SelectCurrent);
    setWindowTitle(tr("Cheque printing assistant"));
    setWindowIcon(theme()->icon(Core::Constants::ICONCHEQUE));

    QPushButton *button = ui->buttonBox->addButton("Prévisualiser", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(previewCheque()));
    button = ui->buttonBox->addButton("Imprimer le chèque", QDialogButtonBox::AcceptRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printCheque()));
}

ChequePrinterDialog::~ChequePrinterDialog()
{
    delete ui;
}

void ChequePrinterDialog::initializeWithSettings()
{
    setPlace(settings()->value(Constants::S_PLACE).toString());
    setDate(QDate::currentDate());
    setOrder(settings()->value(Constants::S_ORDER).toString());
    setDefaultAmounts(settings()->value(Constants::S_VALUES).toStringList());
}

void ChequePrinterDialog::done(int result)
{
    if (result == QDialog::Rejected) {
        QDialog::done(result);
        return;
    }
    if (printCheque())
        QDialog::done(QDialog::Accepted);
}

bool ChequePrinterDialog::printCheque()
{
    // Print the cheque
    ChequePrinter print;
    print.setDrawRects(true);
    print.setOrder(ui->order->text());
    print.setPlace(ui->place->text());
    print.setDate(ui->date->date());
    if (!ui->valueLineEdit->text().simplified().isEmpty()) {
        print.setAmount(ui->valueLineEdit->text().toDouble());
    } else if (ui->valueListWidget->selectionModel()->hasSelection()) {
        print.setAmount(ui->valueListWidget->selectionModel()->currentIndex().data().toDouble());
    } else {
        Utils::warningMessageBox(tr("No amount"), tr("Please specify an amount for the cheque."));
        return false;
    }
    if (!print.print(_printFormatModel->chequePrintFormat(ui->chequeFormat->selectionModel()->currentIndex()))) {
        LOG_ERROR("Unable to print cheque");
        return false;
    }
    return true;
}

void ChequePrinterDialog::setOrder(const QString &order)
{
    ui->order->setText(order);
}

void ChequePrinterDialog::setPlace(const QString &place)
{
    ui->place->setText(place);
}

void ChequePrinterDialog::setDate(const QDate &date)
{
    ui->date->setDate(date);
}

void ChequePrinterDialog::setAmount(double amount)
{
    ui->valueLineEdit->setText(QString::number(amount, 'f', 2));
}

void ChequePrinterDialog::setDefaultAmounts(const QStringList &values)
{
    ui->valueListWidget->clear();
    foreach(const QString &val, values) {
        ui->valueListWidget->addItem(val);
    }
}

void ChequePrinterDialog::previewCheque()
{
    Utils::ImageViewer viewer(this);
    ChequePrinter printer;
    printer.setDrawRects(true);
    printer.setOrder(ui->order->text());
    printer.setPlace(ui->place->text());
    printer.setDate(ui->date->date());
    if (!ui->valueLineEdit->text().simplified().isEmpty()) {
        printer.setAmount(ui->valueLineEdit->text().toDouble());
    } else if (ui->valueListWidget->selectionModel()->hasSelection()) {
        printer.setAmount(ui->valueListWidget->selectionModel()->currentIndex().data().toDouble());
    }
    Internal::ChequePrintFormat format = _printFormatModel->chequePrintFormat(ui->chequeFormat->selectionModel()->currentIndex());
    QPixmap pix = printer.preview(format).scaledToWidth(700, Qt::SmoothTransformation);
    viewer.setPixmap(pix);
    viewer.exec();
}

} // namespace Tools
