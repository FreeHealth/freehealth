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

#include <utils/widgets/detailswidget.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include "ui_atcpage.h"

#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>

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
    connect(ui->atcView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onAtcCodeSelectionChanged(QModelIndex,QModelIndex)));

    // Create the ATC details widget & content
    _details = new Utils::DetailsWidget(this);
    QLabel *codeLabel = new QLabel(tr("Code"), _details);
    QLabel *englishLabel = new QLabel(tr("English"), _details);
    QLabel *frenchLabel = new QLabel(tr("French"), _details);
    QLabel *deutschLabel = new QLabel(tr("Deutsch"), _details);
    QLabel *spanishLabel = new QLabel(tr("Spanish"), _details);
    _code = new QLineEdit(_details);
    _english = new QLineEdit(_details);
    _french = new QLineEdit(_details);
    _deutsch = new QLineEdit(_details);
    _spanish = new QLineEdit(_details);
    _code->setReadOnly(true);
    _english->setReadOnly(true);
    _french->setReadOnly(true);
    _deutsch->setReadOnly(true);
    _spanish->setReadOnly(true);
    QWidget *container = new QWidget(_details);
    QFormLayout *form = new QFormLayout(container);
    form->setMargin(0);
    form->addRow(codeLabel, _code);
    form->addRow(englishLabel, _english);
    form->addRow(frenchLabel, _french);
    form->addRow(deutschLabel, _deutsch);
    form->addRow(spanishLabel, _spanish);
    _details->setWidget(container);
    _details->setSummaryText(tr("No ATC selected"));

    layout()->addWidget(_details);
}

AtcWidget::~AtcWidget()
{
    delete ui;
}

void AtcWidget::onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    AtcModel *model = dbCore()->atcModel();
    QModelIndex code = model->index(current.row(), AtcModel::ATC_Code, current.parent());
    QModelIndex en = model->index(current.row(), AtcModel::ATC_EnglishLabel, current.parent());
    QModelIndex fr = model->index(current.row(), AtcModel::ATC_FrenchLabel, current.parent());
    QModelIndex de = model->index(current.row(), AtcModel::ATC_DeutschLabel, current.parent());
    _details->setSummaryText(code.data().toString() + " - " + en.data().toString());
//    QModelIndex es = model->index(current.row(), AtcModel::ATC_SpanishLabel, current.parent());
    _code->setText(code.data().toString());
    _english->setText(en.data().toString());
    _french->setText(fr.data().toString());
    _deutsch->setText(de.data().toString());
//    _spanish->setText(es.data().toString());
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
