/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "appaboutpage.h"

#include <utils/updatechecker.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_spashandupdate.h>

// TODO: improve this, rename the fmfcoreplugin -> coreplugin and move it into the freemedforms/plugins subdir
#include <fmfcoreplugin/coreimpl.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QApplication>
#include <QDate>

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

AppAboutPage::AppAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("AppAboutPage");
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
    Utils::UpdateChecker *up = Core::ICore::instance()->updateChecker();
    QString tmp = tkTr(Trans::Constants::APPLICATION_ABOUT_YEAR_1_WEB_2)
                   .arg(QDate::currentDate().year())
                   .arg(qApp->organizationDomain());
    if (up->hasUpdate()) {
        tmp.append("<br /><br />" + tkTr(Trans::Constants::UPDATE_AVAILABLE));
    } else {
        tmp.append("<br /><br />" + tkTr(Trans::Constants::VERSION_UPTODATE));
    }
    label->setText(tmp);
    return w;
}
