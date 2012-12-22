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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "printerpreferences.h"
#include "constants.h"

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_menu.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

#include <QFileDialog>
#include <QDir>

#if QT_VERSION < 0x050000
#include <QPrinterInfo>
#else
#include <QtPrintSupport/QPrinterInfo>
#endif

using namespace Print::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

PrinterPreferencesPage::PrinterPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("PrinterPreferencesPage");
}

PrinterPreferencesPage::~PrinterPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PrinterPreferencesPage::id() const { return objectName(); }
QString PrinterPreferencesPage::displayName() const { return tkTr(Trans::Constants::DEFAULTS); }
QString PrinterPreferencesPage::category() const { return tkTr(Trans::Constants::PRINTING); }
QString PrinterPreferencesPage::title() const {return tr("Printer preferences");}
int PrinterPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_PRINT;
}

void PrinterPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void PrinterPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PrinterPreferencesPage::finish() { delete m_Widget; }

void PrinterPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Print::Constants::S_DEFAULT_PRINTER, QString("System"));
    defaultvalues.insert(Print::Constants::S_COLOR_PRINT, QPrinter::GrayScale);
    defaultvalues.insert(Print::Constants::S_RESOLUTION, QPrinter::ScreenResolution);
    defaultvalues.insert(Print::Constants::S_TWONUP, false);
    defaultvalues.insert(Print::Constants::S_KEEP_PDF, false);
    defaultvalues.insert(Print::Constants::S_PDF_FOLDER, QVariant());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

QWidget *PrinterPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PrinterPreferencesWidget(parent);
    return m_Widget;
}



PrinterPreferencesWidget::PrinterPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDataToUi();
}

void PrinterPreferencesWidget::setDataToUi()
{
    printerList->clear();
    if (!QPrinterInfo::availablePrinters().count()) {
        QListWidgetItem *item = new QListWidgetItem(printerList);
        QFont bold;
        bold.setBold(true);
        item->setBackgroundColor(QColor("yellow"));
        item->setForeground(QColor("red"));
        item->setFont(bold);
        item->setText(tr("WARNING !\n"
                         "No configurated printer found on your system.\n"
                         "Printing and print preview should not work at all.\n"
                         "Please configure a printer.\n"
                         "Refer to your operating system documentation."));
        return;
    }
    QString select = settings()->value(Print::Constants::S_DEFAULT_PRINTER).toString();
    foreach(const QPrinterInfo &info, QPrinterInfo::availablePrinters()) {
        QListWidgetItem *item = new QListWidgetItem(printerList);
        QString name = info.printerName();
        if (name == select) {
            QColor c = QColor("lightred");
            c.setAlpha(122);
            item->setData(Qt::BackgroundRole, c);
            name.append(" [Selected]");
            item->setSelected(true);
        }
        if (info.isDefault()) {
            QColor c = QColor("lightblue");
            c.setAlpha(122);
            item->setData(Qt::BackgroundRole, c);
            name.append(" [Default]");
        }
        item->setData(Qt::DisplayRole, name);
    }
    // Add settings name if printer is not connected !

    QListWidgetItem *system = new QListWidgetItem(tr("Always use system default printer"), printerList);
    system->setData(Qt::UserRole, "System");
    if (select == "System")
        system->setSelected(true);
    QListWidgetItem *user = new QListWidgetItem(tr("Always ask user for the printer"), printerList);
    user->setData(Qt::UserRole, "User");
    if (select == "User")
        user->setSelected(true);

    // Color print
    if (settings()->value(Print::Constants::S_COLOR_PRINT).toInt() == QPrinter::Color) {
        colorBox->setChecked(true);
    } else {
        grayBox->setChecked(true);
    }

    // Resolution / 2Nup
    resolutionCombo->setCurrentIndex(settings()->value(Print::Constants::S_RESOLUTION).toInt());
    nupBox->setChecked(settings()->value(Print::Constants::S_TWONUP).toBool());

    // Pdf
    keepPdfBox->setChecked(settings()->value(Print::Constants::S_KEEP_PDF).toBool());
    folderName->setText(settings()->value(Print::Constants::S_PDF_FOLDER).toString());
}

void PrinterPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    // Default printer
    if (printerList->selectedItems().count()) {
        QListWidgetItem *sel = printerList->selectedItems().at(0);
        if (sel) {
            if (!sel->data(Qt::UserRole).toString().isEmpty()) {
                s->setValue(Print::Constants::S_DEFAULT_PRINTER, sel->data(Qt::UserRole));
            } else {
                s->setValue(Print::Constants::S_DEFAULT_PRINTER, sel->text());
            }
        }
    } else {
        s->setValue(Print::Constants::S_DEFAULT_PRINTER, "system");
    }
    // Color Print
    if (colorBox->isChecked())
        s->setValue(Print::Constants::S_COLOR_PRINT, QPrinter::Color);
    else
        s->setValue(Print::Constants::S_COLOR_PRINT, QPrinter::GrayScale);

    // Resolution - 2NUp
    s->setValue(Print::Constants::S_RESOLUTION, resolutionCombo->currentIndex());
    s->setValue(Print::Constants::S_TWONUP, nupBox->isChecked());

    // Pdf
    s->setValue(Print::Constants::S_KEEP_PDF, keepPdfBox->isChecked());
    s->setValue(Print::Constants::S_PDF_FOLDER, folderName->text());
}

void PrinterPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Utils::Log::addMessage("PrinterPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("Printer"));
    s->setValue(Print::Constants::S_DEFAULT_PRINTER, QString("System"));
    s->setValue(Print::Constants::S_COLOR_PRINT, QPrinter::GrayScale);
    s->setValue(Print::Constants::S_RESOLUTION, QPrinter::ScreenResolution);
    s->setValue(Print::Constants::S_TWONUP, false);
    s->setValue(Print::Constants::S_KEEP_PDF, false);
    s->setValue(Print::Constants::S_PDF_FOLDER, QVariant());
    s->sync();
}

void PrinterPreferencesWidget::on_selectFolderButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a directory"),
                                                    QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     );
    folderName->setText(dir);
}

void PrinterPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
