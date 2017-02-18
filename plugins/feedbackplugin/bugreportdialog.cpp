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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Christian A Reiter, Eric Maeker                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Feedback::BugReportDialog
 * Simple bug report sender dialog.
 */

#include "bugreportdialog.h"
#include "ui_bugreportdialog.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/qbuttonlineedit.h>

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QPushButton>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>


using namespace Feedback;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }


namespace Feedback {
namespace Internal {
class BugReportDialogPrivate
{
public:
    BugReportDialogPrivate(BugReportDialog *parent):
        q(parent)
    {}

    ~BugReportDialogPrivate()
    {
        if (ui)
            delete ui;
    }

    void createUi()
    {
        ui = new Ui::BugReportDialog;
        ui->setupUi(q);
        _sendButton = new QPushButton(q);
        _sendButton->setDisabled(true);
        ui->buttonBox->addButton(_sendButton, QDialogButtonBox::ActionRole);
        ui->hasLookedUpDocumentationCheckBox->setCheckState(Qt::PartiallyChecked);
    }

    void connectUi()
    {
        QObject::connect(ui->emailEdit, SIGNAL(textChanged(QString)), q, SLOT(validateInputs()));
        QObject::connect(ui->categoryCombo, SIGNAL(currentIndexChanged(int)), q, SLOT(validateInputs()));
        QObject::connect(ui->severityCombo, SIGNAL(currentIndexChanged(int)), q, SLOT(validateInputs()));
        QObject::connect(ui->descrEdit, SIGNAL(textChanged()), q, SLOT(validateInputs()));
        QObject::connect(ui->hasLookedUpDocumentationCheckBox, SIGNAL(clicked()), q, SLOT(validateInputs()));
        QObject::connect(ui->docUrlPushButton, SIGNAL(clicked()), q, SLOT(openFmfDocUrl()));
        QObject::connect(_sendButton, SIGNAL(clicked()), q, SLOT(sendBugReport()));
    }

    void getReportInformation()
    {
        ui->applicationValue->setText(qApp->applicationName() + " - " + qApp->applicationVersion());
        ui->dateValue->setText(QLocale().toString(QDateTime::currentDateTime()));
        ui->osValue->setText(Utils::uname());
        ui->severityCombo->addItem(QString(Trans::Constants::HIGH).toUpper());
        ui->severityCombo->addItem(QString(Trans::Constants::MEDIUM).toUpper());
        ui->severityCombo->addItem(QString(Trans::Constants::LOW).toUpper());
        ui->severityCombo->setCurrentIndex(1);
    }

    QStringList prepareBugReport()
    {
        QStringList lines;
        typedef QPair<QString, QString> pairs;
        QList<pairs> report;
        report << pairs(ui->emailLabel->text().remove(":").remove("&"), ui->emailEdit->text());
        report << pairs(ui->dateLabel->text().remove(":").remove("&"), ui->dateValue->text());
        report << pairs(ui->applicationLabel->text().remove(":").remove("&"), ui->applicationValue->text());
        report << pairs(ui->osLabel->text().remove(":").remove("&"), Utils::lineWrapString(ui->osValue->text(), 50));
        report << pairs(ui->severityLabel->text().remove(":").remove("&"), ui->severityCombo->currentText());
        report << pairs("---", "");
        report << pairs(ui->catLabel->text().remove(":").remove("&"), ui->categoryCombo->currentText());
        report << pairs(ui->descrLabel->text().remove(":").remove("&"), Utils::lineWrapString(ui->descrEdit->toPlainText(), 50));
        if (ui->hasLookedUpDocumentationCheckBox->isChecked())
            report << pairs("has looked up documentation", "yes");
        else
            report << pairs("has looked up documentation", "no");
        // find the max length
        int max = 0;
        foreach(const pairs &pair, report) {
            max = qMax(max, pair.first.length());
        }
        // justify test && create line
        foreach(const pairs &pair, report) {
            if (pair.first == "---") {
                lines << "-------------------------";
                continue;
            }
            QString second = " " + pair.second;
            second = second.replace("\n", "\n" + QString().fill(' ', max+5));
            lines << "  " + pair.first.leftJustified(max+2, QLatin1Char('.')) + second;
        }
        return lines;
    }

public:
    Ui::BugReportDialog *ui;
    QPushButton *_sendButton;

private:
    BugReportDialog *q;
};
} // namespace Internal
} // namespace Feedback

BugReportDialog::BugReportDialog(QWidget *parent) :
    QDialog(parent),
    d(new BugReportDialogPrivate(this))
{
    d->createUi();
    d->connectUi();
    d->getReportInformation();
    d->_sendButton->setText(tr("&Send report"));
    setWindowTitle(tr("Problem reporting dialog"));
    Utils::centerWidget(this, parent);
}

BugReportDialog::~BugReportDialog()
{
    if (d)
        delete d;
    d = 0;
}

/** Define the bug categories for the ui combobox */
void BugReportDialog::setBugCategories(const QStringList &cat)
{
    d->ui->categoryCombo->clear();
    d->ui->categoryCombo->addItems(cat);
    d->ui->categoryCombo->setCurrentIndex(-1);
}

void BugReportDialog::validateInputs()
{
    // WARN_FUNC;
    bool enabled = true;
    QString toolTip;

    // Check that email is not empty
    if (!d->ui->emailEdit->text().isEmpty()) {
        d->ui->emailState->setPixmap(theme()->icon(Core::Constants::ICONOK, Core::ITheme::SmallIcon).pixmap(16, 16));
    } else {
        enabled = false;
        toolTip += QString("<li>%1</li>").arg(tr("Please enter your email address."));
        d->ui->emailState->setPixmap(theme()->icon(Core::Constants::ICONWARNING, Core::ITheme::SmallIcon).pixmap(16, 16));
    }

    // Check category
    if (d->ui->categoryCombo->currentIndex() == -1) {
        enabled = false;
        toolTip += QString("<li>%1</li>").arg(tr("Please choose the issue category."));
        d->ui->categoryState->setPixmap(theme()->icon(Core::Constants::ICONWARNING, Core::ITheme::SmallIcon).pixmap(16, 16));
    } else {
        d->ui->categoryState->setPixmap(theme()->icon(Core::Constants::ICONOK, Core::ITheme::SmallIcon).pixmap(16, 16));
    }

    // Check severity
    if (d->ui->severityCombo->currentIndex() == -1) {
        enabled = false;
        toolTip += QString("<li>%1</li>").arg(tr("Please select the severity of the issue."));
        d->ui->severityState->setPixmap(theme()->icon(Core::Constants::ICONWARNING, Core::ITheme::SmallIcon).pixmap(16, 16));
    } else {
        d->ui->severityState->setPixmap(theme()->icon(Core::Constants::ICONOK, Core::ITheme::SmallIcon).pixmap(16, 16));
    }

    // Check description
    if (d->ui->descrEdit->toPlainText().isEmpty()) {
        enabled = false;
        toolTip += QString("<li>%1</li>").arg(tr("Describe the problem you have in short, pregnant words.<br/>The more information you provide, the easier we can help."));
        d->ui->descriptionState->setPixmap(theme()->icon(Core::Constants::ICONWARNING, Core::ITheme::SmallIcon).pixmap(16, 16));
    } else {
        d->ui->descriptionState->setPixmap(theme()->icon(Core::Constants::ICONOK, Core::ITheme::SmallIcon).pixmap(16, 16));
    }

    // Check doc
    if (d->ui->hasLookedUpDocumentationCheckBox->checkState() == Qt::PartiallyChecked) {
        enabled = false;
        toolTip += QString("<li>%1</li>").arg(tr("Check/uncheck the documentation checkbox."));
        d->ui->docState->setPixmap(theme()->icon(Core::Constants::ICONWARNING, Core::ITheme::SmallIcon).pixmap(16, 16));
    } else {
        d->ui->docState->setPixmap(theme()->icon(Core::Constants::ICONOK, Core::ITheme::SmallIcon).pixmap(16, 16));
        d->ui->hasLookedUpDocumentationCheckBox->setTristate(false);
    }

    // Manage global tooltip
    if (!d->ui->hasLookedUpDocumentationCheckBox->isChecked()) {
        enabled = false;
        toolTip += QString("<li>%1</li>").arg(tr("Please confirm that you have looked up the documentation"));
    }
    d->_sendButton->setToolTip(enabled? "" : QString("<p>%1</p><ul>%2</ul>").arg(tr("The following problems occurred:")).arg(toolTip));
    d->_sendButton->setEnabled(enabled);
}

/**
 * \internal
 * Prepare and send the bug report.
 */
void BugReportDialog::sendBugReport()
{
    QString fullmsg = "\n\n" + d->prepareBugReport().join("\n");
    fullmsg += "\n\n" + Utils::Log::toString("");
    QDesktopServices::openUrl(QString("mailto:freemedforms-dev@googlegroups.com?subject=%1&body=%2")
            .arg("Bug Report: " + qApp->applicationName() + qApp->applicationVersion())
            .arg(fullmsg));
}

void BugReportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        if (d->_sendButton)
            d->_sendButton->setText(tr("&Send report"));
        break;
    default:
        break;
    }
}

void BugReportDialog::openFmfDocUrl()
{
    if(!QDesktopServices::openUrl(QUrl(Utils::Constants::URL_ONLINEDOCUMENTATION))) {
        Utils::warningMessageBox(tr("The application has requested the operating system to open the URL in an external browser but something went wrong."),
                                 tr("Please verify that you have a default browser enabled or go directly to %1 and check the documentation.").arg(Utils::Constants::URL_ONLINEDOCUMENTATION));
    }
}
