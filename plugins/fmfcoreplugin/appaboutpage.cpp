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
#include "appaboutpage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QApplication>
#include <QDate>

using namespace Core;
using namespace Core::Internal;

static const char *ABOUT_TEXT = QT_TRANSLATE_NOOP("AboutDialog",
        "<p align=center><b>Welcome to FreeMedForms</b><br />"
        "(C) 2008-%1 by Eric MAEKER, MD</p>"
        "<p align=left>"
//        "This application is an early alpha release. This means that some "
//        "features are not yet implemented or may not work perfectly and that you may experience some bugs.<br/>"
        "This software is released without any warranty and only for testing purposal.<br/>"
        "Please refer to our <a href=\"%2\">web site</a> for more information.<br />"
        "</p>"
        );

AppAboutPage::AppAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("AppAboutPage");
    setId(objectName());
}

AppAboutPage::~AppAboutPage()
{
}

QWidget *AppAboutPage::createPage(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QLabel *label = new QLabel(w);
    label->setWordWrap(true);
    label->setOpenExternalLinks(true);
    layout->addWidget(label);
    layout->addSpacerItem(new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    label->clear();
    label->setText(tr(ABOUT_TEXT).arg(QDate::currentDate().year()).arg(qApp->organizationDomain()));
    return w;
}
