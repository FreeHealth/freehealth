#include "commonaboutpages.h"

#include <coreplugin/global.h>
#include <coreplugin/constants.h>
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

using namespace Core;
using namespace Core::Internal;


///// BUILD ABOUT PAGE
BuildAboutPage::BuildAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("BuildAboutPage");
    m_Widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_Widget);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(tree);
}

BuildAboutPage::~BuildAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

void BuildAboutPage::refreshContents()
{
    tree->clear();
    QFont f;
    f.setBold(true);
    QTreeWidgetItem *compilItem = new QTreeWidgetItem( tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_INFORMATIONS) );
    compilItem->setFont(0,f);
    new QTreeWidgetItem( compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (tkGlobal::isDebugCompilation()) {
        new QTreeWidgetItem( compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG) );
    } else {
        new QTreeWidgetItem( compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_RELEASE) );
    }

    QTreeWidgetItem *versionItem = new QTreeWidgetItem( tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_INFORMATIONS) );
    versionItem->setFont(0,f);
    new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_1).arg(qApp->applicationVersion()));
    if (tkGlobal::isFullApplication()) {
        new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_FULLAPP));
    } else {
        new QTreeWidgetItem( versionItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_SVNAPP));
    }
    new QTreeWidgetItem( versionItem, QStringList() << tr("Actual Qt version : %1").arg( qVersion() ));
    tree->expandAll();
}

QWidget *BuildAboutPage::widget()
{
    return m_Widget;
}



///// LICENSE ABOUT PAGE

LicenseAboutPage::LicenseAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("LicenseAboutPage");
    m_Widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    browser = new QTextBrowser(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(browser);
}

LicenseAboutPage::~LicenseAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

void LicenseAboutPage::refreshContents()
{
    browser->clear();
    // add a generic message
    browser->setHtml(LicenseTerms::getTranslatedLicenseTerms(LicenseTerms::BSDModified));
}

QWidget *LicenseAboutPage::widget()
{
    return m_Widget;
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

        ;



TeamAboutPage::TeamAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("TeamAboutPage");
    m_Widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_Widget);
    layout->setMargin(0);
    layout->setSpacing(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    layout->addWidget(tree);
}

TeamAboutPage::~TeamAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

void TeamAboutPage::refreshContents()
{
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
        new QTreeWidgetItem(nameItem, QStringList() << t.Email);
        new QTreeWidgetItem(nameItem, QStringList() << t.Country);
        new QTreeWidgetItem(nameItem, QStringList() << t.UnTranslatedComment);
    }
}

QWidget *TeamAboutPage::widget()
{
    return m_Widget;
}
