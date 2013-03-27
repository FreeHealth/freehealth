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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "commonaboutpages.h"

#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFont>
#include <QBrush>
#include <QDesktopServices>

using namespace Core;
using namespace Core::Internal;


///// BUILD ABOUT PAGE
BuildAboutPage::BuildAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("BuildAboutPage");
}

BuildAboutPage::~BuildAboutPage()
{
}

QWidget *BuildAboutPage::createPage(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(w);
    QTreeWidget *tree = new QTreeWidget(w);
    tree->header()->hide();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(tree);

    // populate tree
    tree->clear();
    QFont f;
    f.setBold(true);
    QTreeWidgetItem *compileItem = new QTreeWidgetItem( tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_INFORMATION) );
    compileItem->setFont(0,f);
    new QTreeWidgetItem( compileItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (Utils::isDebugWithoutInstallCompilation()) {
        new QTreeWidgetItem( compileItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG) + " - no install");
    } else {
        new QTreeWidgetItem( compileItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_RELEASE) );
    }

    QTreeWidgetItem *versionItem = new QTreeWidgetItem( tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_INFORMATION) );
    versionItem->setFont(0,f);
    new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_1).arg(qApp->applicationVersion()));
    if (Utils::isFullApplication()) {
        new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_FULLAPP));
    } else {
        new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_GITAPP));
    }
    new QTreeWidgetItem( versionItem, QStringList() << tr("Current Qt version: %1").arg( qVersion() ));
    tree->expandAll();

    return w;
}



///// LICENSE ABOUT PAGE

LicenseAboutPage::LicenseAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("LicenseAboutPage");
}

LicenseAboutPage::~LicenseAboutPage()
{
}

QWidget *LicenseAboutPage::createPage(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTextBrowser *browser = new QTextBrowser(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(browser);
    browser->clear();
    // add a generic message
    browser->setHtml(Utils::LicenseTerms::getTranslatedLicenseTerms(Utils::LicenseTerms::GPLv3));
    return w;
}


///// TEAM ABOUT PAGE

struct Team
{
    Team( const QString &cat, const QString& n, const QString& p, const QString& e, const QString& c )
                : Categorie(cat), Name( n ),Country( p ), Email( e ), UnTranslatedComment( c ) {}
    QString Categorie;
    QString Name;
    QString Country;
    QString Email;
    QString UnTranslatedComment;
};

static const QList<Team> m_Team = QList<Team>()
        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_CREATOR),
                 "Eric MAEKER, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_CREATOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS),
                 "Pierre-Marie Desombre, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "pm.desombre@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPER_AND_DEBUGGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS),
                 "Guillaume Denry",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "guillaume.denry@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPER_AND_DEBUGGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS),
                 "Christian A. Reiter, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_AUSTRIA),
                 "christian.a.reiter@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPER_AND_DEBUGGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS),
                 "Filipe Azevedo",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "pasnox@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEBUGGER_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS),
                 "David Geldreich",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "david.geldreich@free.fr",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEBUGGER_ADVISOR)
               )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS),
                 "Jourdan Romain",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "No@Mail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPERS_CONTRIBUTORS)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER),
                 "Eric MAEKER, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER),
                 "Jim Busser, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_CANADA),
                 "jbusser@interchange.ubc.ca",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Pierre-Marie Desombre, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "pm.desombre@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Eric MAEKER, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 QString::fromUtf8("Sébastien Belin, MD"),
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "belin.s@wanadoo.fr",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Laurent Massont, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "<No@Mail.com>",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Christian A. Reiter, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_AUSTRIA),
                 "christian.a.reiter@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPER_AND_DEBUGGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_WEBMASTER),
                 "Vincent Perez, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "vinperez@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_WEBMASTER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Vincent Perez, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "vinperez@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Karsten Hilbert, MD (GNUmed)",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_GERMANY),
                 "Karsten.Hilbert@gmx.net",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Jim Busser, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_CANADA),
                 "jbusser@interchange.ubc.ca",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_WEBMASTER),
                 "Eric MAEKER, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_WEBMASTER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER),
                 "Sascha Manns",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_GERMANY),
                 "Sascha.Manns@directbox.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER_FOR_1).arg("OpenSUSE")
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER),
                 "Eric Maeker, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER_FOR_1).arg("Debian and Ubuntu")
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER),
                 "Debian Med Team",
                 "",
                 "debian-med@lists.debian.org",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER_FOR_1).arg("Debian")
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_TRANSLATORS),
		 "Sascha Manns",
		 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_GERMANY),
		 "Sascha.Manns@directbox.com",
		 "German translation"
		)

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_TRANSLATORS),
                 "Jim Busser, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_CANADA),
                 "",
                 "English"
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_TRANSLATORS),
		 "Eric Maeker, MD",
		 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
		 "eric.maeker@gmail.com",
		 "French / English"
		)

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_TRANSLATORS),
                 "Christian A. Reiter, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_AUSTRIA),
                 "christian.a.reiter@gmail.com",
                 "English / Deutsch"
                )

	;



TeamAboutPage::TeamAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("TeamAboutPage");
}

TeamAboutPage::~TeamAboutPage()
{
}

QWidget *TeamAboutPage::createPage(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setMargin(0);
    layout->setSpacing(0);
    QTreeWidget *tree = new QTreeWidget(w);
    tree->header()->hide();
    layout->addWidget(tree);
    tree->clear();

    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;
    CategoryItemMap categories;

    foreach( const Team &t, m_Team) {
        const QString categoryName = t.Categorie;
        // ensure category root
        CategoryItemMap::iterator cit = categories.find(categoryName);
        if (cit == categories.end()) {
            QTreeWidgetItem *categoryItem = new QTreeWidgetItem(tree);
            QFont f = categoryItem->font(0);
            f.setBold(true);
            categoryItem->setFont(0,f);
            categoryItem->setText(0, t.Categorie);
            cit = categories.insert(categoryName, categoryItem);
            categoryItem->setExpanded(true);
        }
        // add item
        QTreeWidgetItem *nameItem = new QTreeWidgetItem(cit.value(), QStringList() << t.Name);
        QTreeWidgetItem *m = new QTreeWidgetItem(nameItem, QStringList() << t.Email);
        m->setForeground(0,QBrush(Qt::blue));
        QFont f = m->font(0);
        f.setUnderline(true);
        m->setFont(0,f);
        new QTreeWidgetItem(nameItem, QStringList() << t.Country);
        new QTreeWidgetItem(nameItem, QStringList() << t.UnTranslatedComment);
    }
    connect(tree,SIGNAL(itemActivated(QTreeWidgetItem*,int)),this, SLOT(mailTo(QTreeWidgetItem*)));
    return w;
}

void TeamAboutPage::mailTo(QTreeWidgetItem *item)
{
    if (!item)
        return;
    if (item->text(0).contains("@")) {
        QDesktopServices::openUrl(QUrl("mailto:"+item->text(0)));
    }

}
