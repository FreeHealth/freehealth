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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DrugsDB::AtcPage
 * ATC classification manipulation page.
 * \internal
 */

/**
 * \class DrugsDB::AtcWidget
 * Show the ATC tree.
 * \internal
 */

#include "atcpage.h"
#include "atcmodel.h"
#include "drugsdbcore.h"

#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include "ui_atcpage.h"

using namespace DrugsDB;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsDBCore *dbCore() {return DrugsDB::DrugsDBCore::instance();}

AtcPage::AtcPage(QObject *parent) :
        IToolPage(parent)
{}

QString AtcPage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_ATC_HELPERS;
}

QWidget *AtcPage::createPage(QWidget *parent)
{
    return new AtcWidget(parent);
}

AtcWidget::AtcWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AtcPage)
{
    ui->setupUi(this);
    ui->atcView->setModel(dbCore()->atcModel());
    ui->atcView->setIndentation(7);
    ui->atcView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

AtcWidget::~AtcWidget()
{
    delete ui;
}

void AtcWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        for(int i = 0; i < dbCore()->atcModel()->columnCount(); ++i)
            ui->atcView->setColumnHidden(i, true);
        ui->atcView->setColumnHidden(AtcModel::ATC_Code, false);
        // TODO: find the current language and set the correct column not hidden
        ui->atcView->setColumnHidden(AtcModel::ATC_EnglishLabel, false);
        ui->atcView->header()->setResizeMode(AtcModel::ATC_Code, QHeaderView::ResizeToContents);
        ui->atcView->header()->setResizeMode(AtcModel::ATC_EnglishLabel, QHeaderView::Stretch);
    }
}
