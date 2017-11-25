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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsdatabaseaboutpage.h"

#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

namespace {
static const char* const DRUGS_DATABASE_DATE         = "14/12/2009";
static const char* const INTERACTIONS_DATABASE_DATE  = "30/06/2009";
}

DrugsDatabaseAboutPage::DrugsDatabaseAboutPage(QObject *parent) :
        Core::IAboutPage(parent)
{
    setObjectName("DrugsDatabaseAboutPage");
}

DrugsDatabaseAboutPage::~DrugsDatabaseAboutPage()
{
}

QString DrugsDatabaseAboutPage::name() const
{
    return tkTr(Trans::Constants::VERSION);
}

QString DrugsDatabaseAboutPage::category() const
{
    return tkTr(Trans::Constants::DRUGS_DATABASE);
}

QWidget *DrugsDatabaseAboutPage::widget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(w);
    tree->setColumnCount(2);
    layout->addWidget(tree);
    tree->header()->hide();

    new QTreeWidgetItem(tree, QStringList()
                        << tkTr(Trans::Constants::DRUGS_DATABASE)
                        << ::DRUGS_DATABASE_DATE
                        );
    new QTreeWidgetItem(tree, QStringList()
                        << tkTr(Trans::Constants::INTERACTIONS_DATABASE)
                        << ::INTERACTIONS_DATABASE_DATE
                        );

    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
    return w;
}
