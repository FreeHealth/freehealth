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
#include "dynamicalertdialog.h"
#include "alertitem.h"
#include "ui_dynamicalertdialog.h"
#include "ui_dynamicalertdialogoverridingcomment.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/imainwindow.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QDialogButtonBox>
#include <QToolButton>
#include <QScrollArea>

#include <QDebug>

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

DynamicAlertDialog::DynamicAlertDialog(const QList<AlertItem> &items,
                                       const QString &themedIcon,
                                       const QList<QAbstractButton *> &buttons,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DynamicAlertDialog),
    cui(0),
    _overrideButton(0),
    _overrideCommentRequired(false)
{
    // Do we need to ask for an overriding comment
    foreach(const AlertItem &item, items) {
        if (item.isOverrideRequiresUserComment()) {
            _overrideCommentRequired = true;
            break;
        }
    }

    // Prepare the ui
    ui->setupUi(this);
    layout()->setSpacing(5);
    setWindowTitle(tkTr(Trans::Constants::DYNAMIC_ALERT));
    setWindowModality(Qt::WindowModal);

    // Manage the general icon of the dialog
    if (!themedIcon.isEmpty() && QFile(theme()->iconFullPath(themedIcon, Core::ITheme::BigIcon)).exists()) {
        ui->generalIconLabel->setPixmap(theme()->icon(themedIcon, Core::ITheme::BigIcon).pixmap(64,64));
        setWindowIcon(theme()->icon(themedIcon));
    } else {
        int maxPriority = AlertItem::Low;
        for(int i=0; i<items.count();++i) {
            maxPriority = qMax(maxPriority, int(items.at(i).priority()));
        }
        QString icon;
        switch (maxPriority) {
        case AlertItem::High: icon = Core::Constants::ICONCRITICAL; break;
        case AlertItem::Medium: icon = Core::Constants::ICONWARNING; break;
        case AlertItem::Low: icon = Core::Constants::ICONINFORMATION; break;
        }
        ui->generalIconLabel->setPixmap(theme()->icon(icon, Core::ITheme::BigIcon).pixmap(64,64));
        setWindowIcon(theme()->icon(icon));
    }

    // Include alerts
    if (items.count()==1) {
        // No tabwidget
        const AlertItem &alert = items.at(0);
        QVBoxLayout *central = new QVBoxLayout(this);

        if (!alert.category().isEmpty()) {
            QFont bold;
            bold.setBold(true);
            QLabel *label = new QLabel(this);
            label->setFont(bold);
            label->setTextFormat(Qt::RichText);
            label->setAlignment(Qt::AlignHCenter);
            label->setWordWrap(true);
            //        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            label->setText(alert.category());
            central->addWidget(label);
        }

        QLabel *label = new QLabel(this);
        label->setTextFormat(Qt::RichText);
        label->setAlignment(Qt::AlignHCenter);
        label->setWordWrap(true);
        //        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        label->setText(alert.label());
        central->addWidget(label);

        if (!alert.description().isEmpty()) {
            QLabel *label = new QLabel(this);
            label->setTextFormat(Qt::RichText);
            label->setAlignment(Qt::AlignLeft);
            label->setWordWrap(true);
            //        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            label->setText(alert.description());
            central->addWidget(label);
        }

        QWidget *container = new QWidget(this);
        QVBoxLayout *containerLayout = new QVBoxLayout(container);
        containerLayout->setMargin(0);
        container->setLayout(containerLayout);

        QScrollArea *s = new QScrollArea(this);
        s->setBackgroundRole(QPalette::Background);
        s->setWidgetResizable(true);
        s->setFrameStyle(QFrame::NoFrame);
        s->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        QWidget *alertContainer = new QWidget(this);
        alertContainer->setLayout(central);
        s->setWidget(alertContainer);
        containerLayout->addWidget(s);

        ui->centralLayout->addWidget(container);

    } else {
        // With tabwidget
        QHash<QString, QVBoxLayout *> categories;

        for(int i=0; i < items.count(); ++i) {
            const AlertItem &alert = items.at(i);
            QString cat = alert.category();
            if (cat.isEmpty()) {
                cat = tkTr(Trans::Constants::ALERT);
            }

            // Get the category layout
            QVBoxLayout *lay = categories.value(cat, 0);
            if (!lay) {
                lay = new QVBoxLayout;
                categories.insert(cat, lay);
            } else {
                QFrame *line = new QFrame(this);
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                lay->addWidget(line);
            }

            // Add the label / description to the layout
            QLabel *label = new QLabel(this);
            label->setWordWrap(true);
            label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            label->setTextFormat(Qt::RichText);
            label->setText(alert.label());
            lay->addWidget(label);

            if (!alert.description().isEmpty()) {
                QLabel *label = new QLabel(this);
                label->setTextFormat(Qt::RichText);
                label->setAlignment(Qt::AlignLeft);
                label->setWordWrap(true);
                label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
                label->setText(alert.description());
                lay->addWidget(label);
            }
        }

        QTabWidget *tab = new QTabWidget(this);
        // Create a tab for each category
        foreach(const QString &cat, categories.keys()) {
            QWidget *container = new QWidget(this);
            QVBoxLayout *containerLayout = new QVBoxLayout(container);
            containerLayout->setMargin(0);
            container->setLayout(containerLayout);

            QScrollArea *s = new QScrollArea(this);
            s->setBackgroundRole(QPalette::Background);
            s->setWidgetResizable(true);
            s->setFrameStyle(QFrame::NoFrame);
            s->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            QWidget *alertContainer = new QWidget(this);
            QVBoxLayout *central = categories.value(cat);
            alertContainer->setLayout(central);
            s->setWidget(alertContainer);
            containerLayout->addWidget(s);

            tab->addTab(container, cat);
        }
        ui->centralLayout->addWidget(tab);
    }

    // Add buttons
    QDialogButtonBox *box = new QDialogButtonBox(Qt::Horizontal, this);
    QFont bold;
    bold.setBold(true);
    QToolButton *accept = new QToolButton(this);
    accept->setMinimumHeight(22);
    accept->setText(tr("Accept alert"));
    accept->setIcon(theme()->icon(Core::Constants::ICONOK, Core::ITheme::SmallIcon));
    accept->setIconSize(QSize(16,16));
    accept->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    accept->setFont(bold);
    box->addButton(accept, QDialogButtonBox::AcceptRole);

    _overrideButton = new QToolButton(this);
    _overrideButton->setMinimumHeight(22);
    _overrideButton->setText(tr("Override alert"));
    _overrideButton->setIcon(theme()->icon(Core::Constants::ICONNEXT, Core::ITheme::SmallIcon));
    _overrideButton->setIconSize(QSize(16,16));
    _overrideButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _overrideButton->setFont(bold);
    box->addButton(_overrideButton, QDialogButtonBox::RejectRole);

    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(override()));
    ui->buttonLayout->setMargin(0);
    ui->buttonLayout->setSpacing(0);
    ui->buttonLayout->addWidget(box);

    adjustSize();
}

DynamicAlertDialog::~DynamicAlertDialog()
{
    delete ui;
    if (cui) delete cui; cui=0;
}

void DynamicAlertDialog::override()
{
    if (!_overrideCommentRequired) {
        reject();
        return;
    }

    // Append the comment
    cui = new Ui::DynamicAlertDialogOverridingComment;
    QWidget *w = new QWidget(this);
    cui->setupUi(w);
    ui->centralLayout->addWidget(w);
    connect(cui->validateComment, SIGNAL(clicked()), this, SLOT(validateUserOverridingComment()));

    _overrideButton->hide();
}

void DynamicAlertDialog::validateUserOverridingComment()
{
    if (!cui->overridingComment->toPlainText().isEmpty())
        reject();
}

void DynamicAlertDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
}

DynamicAlertResult DynamicAlertDialog::executeDynamicAlert(const AlertItem &item, const QString &themedIcon, QWidget *parent)
{
    return executeDynamicAlert(QList<AlertItem>() << item, themedIcon, parent);
}

DynamicAlertResult DynamicAlertDialog::executeDynamicAlert(const QList<AlertItem> &items, const QString &themedIcon, QWidget *parent)
{
    DynamicAlertResult result;
    DynamicAlertDialog dlg(items, themedIcon, QList<QAbstractButton*>(), parent);
    if (dlg.exec()==QDialog::Accepted) {

    } else {

    }
    return result;
}
