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
#include "commonaboutpages.h"

#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <coreplugin/constants.h>

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

QWidget *BuildAboutPage::widget()
{
    QWidget *w = new QWidget();
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
    QTreeWidgetItem *compilItem = new QTreeWidgetItem( tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_INFORMATIONS) );
    compilItem->setFont(0,f);
    new QTreeWidgetItem( compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (Utils::isDebugCompilation()) {
        new QTreeWidgetItem( compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG) );
    } else {
        new QTreeWidgetItem( compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_RELEASE) );
    }

    QTreeWidgetItem *versionItem = new QTreeWidgetItem( tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_INFORMATIONS) );
    versionItem->setFont(0,f);
    new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_1).arg(qApp->applicationVersion()));
    if (Utils::isFullApplication()) {
        new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_FULLAPP));
    } else {
        new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_SVNAPP));
    }
    new QTreeWidgetItem( versionItem, QStringList() << tr("Actual Qt version : %1").arg( qVersion() ));
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

QWidget *LicenseAboutPage::widget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTextBrowser *browser = new QTextBrowser(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(browser);
    browser->clear();
    // add a generic message
    browser->setHtml(Utils::LicenseTerms::getTranslatedLicenseTerms(Utils::LicenseTerms::BSDModified));
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
                 "eric.maeker@free.fr",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_CREATOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPPERS_CONTRIBUTORS),
                 "Guillaume Denry",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "guillaume.denry@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPPER_AND_DEBUGGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPPERS_CONTRIBUTORS),
                 "Filipe Azevedo",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "pasnox@gmail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEBUGGER_ADVISOR)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPPERS_CONTRIBUTORS),
                 "David Geldreich",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "david.geldreich@free.fr",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEBUGGER_ADVISOR)
               )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPPERS_CONTRIBUTORS),
                 "Jourdan Romain",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "No@Mail.com",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DEVELOPPERS_CONTRIBUTORS)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER),
                 "Eric MAEKER, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@free.fr",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER),
                 "Jim Busser, MD",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_CANADA),
                 "jbusser@interchange.ubc.ca",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_DRUG_DATABASE_MANAGER)
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_MEDICAL_ADVISORS),
                 "Eric MAEKER, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@free.fr",
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

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_WEBMASTER),
                 "Eric MAEKER, MD",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::COUNTRY_FRANCE),
                 "eric.maeker@free.fr",
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
                 "eric.maeker@free.fr",
                 Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER_FOR_1).arg("Debian and Ubuntu")
                )

        << Team( Trans::ConstantTranslations::tkTr(Trans::Constants::TEAM_PACKAGER),
                 "Debian Med Team",
                 "jbusser@interchange.ubc.ca",
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
		 "eric.maeker@free.fr",
		 "French / English"
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

QWidget *TeamAboutPage::widget()
{
    QWidget *w = new QWidget();
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
