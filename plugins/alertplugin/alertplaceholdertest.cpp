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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertplaceholdertest.h"
#include "alertitem.h"

#include "ui_alertplaceholdertest.h"

#include <QDebug>

using namespace Alert;

AlertPlaceHolderTest::AlertPlaceHolderTest(QWidget *parent) :
    IAlertPlaceHolder(parent),
    ui(new Ui::AlertPlaceHolderTest)
{
    ui->setupUi(this);
}

AlertPlaceHolderTest::~AlertPlaceHolderTest()
{
    delete ui;
}

QString AlertPlaceHolderTest::uuid() const
{
    return "UID_TEST";
}

// for UI presentation of the place holder
QString AlertPlaceHolderTest::name(const QString &lang) const
{
    return "name";
}

QString AlertPlaceHolderTest::category(const QString &lang) const
{
    return "category";
}

QString AlertPlaceHolderTest::description(const QString &lang) const
{
    return "description";
}

void AlertPlaceHolderTest::clear()
{
    qWarning() << "clear";
}

bool AlertPlaceHolderTest::addAlert(const AlertItem &alert)
{
    qWarning() << "addAlert" << alert.label();
    return true;
}

bool AlertPlaceHolderTest::removeAlert(const AlertItem &alert)
{
    qWarning() << "removeAlert" << alert.label();
    return true;
}

bool AlertPlaceHolderTest::highlightAlert(const AlertItem &alert)
{
    qWarning() << "highlighAlert" << alert.label();
    return true;
}
