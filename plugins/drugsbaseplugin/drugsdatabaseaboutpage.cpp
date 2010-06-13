/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
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
