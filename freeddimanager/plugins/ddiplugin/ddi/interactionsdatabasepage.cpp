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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "interactionsdatabasepage.h"
#include "afssapsintegrator.h"
#include "interactionstep.h"
#include "drugdruginteraction.h"
#include "drugdruginteractionmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/googletranslator.h>

#include <extensionsystem/pluginmanager.h>

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QProgressDialog>
#include <QDataWidgetMapper>
#include <QToolBar>

#include "ui_interactiondatabasecreator.h"
#include "ui_interactiondatabasechecker.h"

#include <QDebug>

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()         {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/Interactions/") + QDir::separator();}

static inline QString translationsCorrectionsFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::INTERACTIONS_ENGLISHCORRECTIONS_FILENAME);}
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString afssapsNewIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}

static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}


//static QString typeToString(const QString &type)
//{
//    QStringList tmp;
//    if (type.contains("C")) {
//        tmp << tkTr(Trans::Constants::CONTRAINDICATION);
//    }
//    if (type.contains("D")) {
//        tmp << tkTr(Trans::Constants::DISCOURAGED);
//    }
//    if (type.contains("T")) {
//        tmp << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
//    }
//    if (type.contains("P")) {
//        tmp << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
//    }
//    if (type.contains("I")) {
//        tmp << tkTr(Trans::Constants::INFORMATION);
//    }

//    return tmp.join( ", " );
//}


//static QIcon typeToIcon(const QString &type)
//{
//    if (type.contains("C"))
//        return theme()->icon("critical.png");
//    if (type.contains("D"))
//        return theme()->icon("deconseille.png");
//    if (type.contains("T"))
//        return theme()->icon("warning.png");
//    if (type.contains("P"))
//        return theme()->icon("info.png");
//    if (type.contains("I"))
//        return theme()->icon("info.png");
//    return QIcon();
//}

QString InteractionsDatabaseCreatorPage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + tr("Interaction database");
}

QWidget *InteractionsDatabaseCreatorPage::createPage(QWidget *parent)
{
    return new InteractionDatabaseCreator(parent);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
namespace IAMDb {

class InteractionDatabaseCreatorPrivate
{
public:
    InteractionStep *m_Step;
};


} // namespace IAMDb


InteractionDatabaseCreator::InteractionDatabaseCreator(QWidget *parent) :
        QWidget(parent), ui(new Ui::InteractionDatabaseCreator), d(new InteractionDatabaseCreatorPrivate)
{
    ui->setupUi(this);
    d->m_Step = new InteractionStep(this);
    pluginManager()->addObject(d->m_Step);
    d->m_Step->createDir();
}

InteractionDatabaseCreator::~InteractionDatabaseCreator()
{
    pluginManager()->removeObject(d->m_Step);
    delete ui; ui=0;
    delete d->m_Step; d->m_Step=0;
    delete d; d=0;
}

void InteractionDatabaseCreator::on_recreateRoutes_clicked()
{
//    DrugsDB::Tools::recreateRoutes();
}

void InteractionDatabaseCreator::on_createAndSave_clicked()
{
    d->m_Step->process();
    if (ui->biblioCheck->isChecked())
        d->m_Step->postProcessDownload();
}

void InteractionDatabaseCreator::on_checkDatas_clicked()
{
//    // clear ui
//    ui->treeWidget->clear();
//    QTreeWidgetItem *item;

//    // check afssaps molecules linking
//    // check afssaps molecules links reviews
//    AfssapsLinkerModel *molLinkModel = AfssapsLinkerModel::instance();
//    while (molLinkModel->canFetchMore(QModelIndex()))
//        molLinkModel->fetchMore(QModelIndex());
//    item = new QTreeWidgetItem(ui->treeWidget);
//    QTreeWidgetItem *unreviewed = new QTreeWidgetItem(item, QStringList() << tr("Unreviewed"));
//    QTreeWidgetItem *unlinked = new QTreeWidgetItem(item, QStringList() << tr("Unlinked"));
//    int unr = 0;
//    int unl = 0;
//    int nb = molLinkModel->rowCount();
//    for(int i = 0; i < nb; ++i) {
//        const QString &reviewed = molLinkModel->index(i, AfssapsLinkerModel::Review).data().toString();
//        if (reviewed!="true") {
//            new QTreeWidgetItem(unreviewed, QStringList() << tr("Unreviewed: %1").arg(molLinkModel->index(i, AfssapsLinkerModel::AfssapsName).data().toString()));
//            ++unr;
//        }
//        const QString &links = molLinkModel->index(i, AfssapsLinkerModel::AtcCodes).data().toString();
//        if (links.isEmpty()) {
//            new QTreeWidgetItem(unlinked, QStringList() << tr("Unlinked: %1").arg(molLinkModel->index(i, AfssapsLinkerModel::AfssapsName).data().toString()));
//            ++unl;
//        }
//    }
//    item->setText(0, tr("AFSSAPS Molecules link (%1%) and review (%2%)")
//                  .arg((double)(nb-unl)/(double)nb*100.0)
//                  .arg((double)(nb-unr)/(double)nb*100.0));

//    // check afssaps tree reviews
//    AfssapsClassTreeModel *afssapsTreeModel = AfssapsClassTreeModel::instance();
//    while (afssapsTreeModel->canFetchMore(QModelIndex()))
//        afssapsTreeModel->fetchMore(QModelIndex());
//    item = new QTreeWidgetItem(ui->treeWidget);
//    unr = 0;
//    nb = afssapsTreeModel->rowCount();
//    for(int i = 0; i < afssapsTreeModel->rowCount(); ++i) {
//        const QString &reviewed = afssapsTreeModel->index(i, AfssapsClassTreeModel::Review).data().toString();
//        if (reviewed!="true") {
//            new QTreeWidgetItem(item, QStringList() << tr("Unreviewed: %1").arg(afssapsTreeModel->index(i, AfssapsClassTreeModel::Name).data().toString()));
//            ++unr;
//        }
//    }
//    item->setText(0, tr("AFSSAPS Tree review (%1%)")
//                  .arg((double)(nb-unr)/(double)nb*100.0));

//    // check afssaps interactions reviews
//    InteractionModel *interactionModel = InteractionModel::instance();
//    while (interactionModel->canFetchMore(QModelIndex()))
//        interactionModel->fetchMore(QModelIndex());
//    item = new QTreeWidgetItem(ui->treeWidget);
//    unr = 0;
//    nb = interactionModel->rowCount();
//    for(int i = 0; i < nb; ++i) {
//        const QString &reviewed = interactionModel->index(i, InteractionModel::Review).data().toString();
//        if (reviewed!="true") {
//            new QTreeWidgetItem(item, QStringList() << tr("Unreviewed: %1").arg(interactionModel->index(i, InteractionModel::Name).data().toString()));
//            ++unr;
//        }
//    }
//    item->setText(0, tr("AFSSAPS Interaction review (%1%)")
//                  .arg((double)(nb-unr)/(double)nb*100.0));

}


namespace IAMDb {

class InteractionDatabaseCheckerPrivate
{
public:
};

}  // End namespace IAMDb


InteractionDatabaseChecker::InteractionDatabaseChecker(QWidget *parent) :
        QWidget(parent), ui(new Ui::InteractionDatabaseChecker), d(new InteractionDatabaseCheckerPrivate)
{
    ui->setupUi(this);
}

InteractionDatabaseChecker::~InteractionDatabaseChecker()
{
    delete ui; ui=0;
    delete d; d=0;
}

void InteractionDatabaseChecker::on_check_clicked()
{
    // TODO: code here */
}




