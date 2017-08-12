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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftb_constants.h"
#include "fullreleasepage.h"

#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/itoolpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/ifullreleasestep.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
//#include <utils/iconbadgealert.h>

#include <QTreeWidgetItem>
#include <QMap>
#include <QList>
#include <QDir>
#include <QDockWidget>
#include <QCloseEvent>
#include <QLabel>
#include <QProgressDialog>

using namespace Core;
using namespace Trans::ConstantTranslations;

#ifdef DEBUG
enum FullRelease { FullReleaseDownload=false, FullReleaseRunProcess=true };
#else
enum FullRelease { FullReleaseDownload=true, FullReleaseRunProcess=true };
#endif

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {theme()->finishSplashScreen(w); }


namespace {
    struct PageData {
        int index;
        QString category;
        QString id;
    };
}

Q_DECLARE_METATYPE(::PageData);

MainWindow::MainWindow(QWidget *parent) :
        Core::IMainWindow(parent),
        ui(0),
        m_FullReleasePage(0),
        m_applied(false)
{
    setObjectName("MainWindow");
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    messageSplash(tr("Creating Main Window"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/** Initialize the main window (create actions and menus) */
bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // create the help dialog
    createFileMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
                Core::MainWindowActions::A_FileQuit
                );
    actions.setConfigurationActions(
                Core::MainWindowActions::A_AppPreferences |
                Core::MainWindowActions::A_LanguageChange
                );
    actions.setHelpActions(
                Core::MainWindowActions::A_AppAbout |
                Core::MainWindowActions::A_PluginsAbout |
                Core::MainWindowActions::A_AppHelp |
                Core::MainWindowActions::A_DebugDialog |
                Core::MainWindowActions::A_CheckUpdate
                );
    actions.createEditActions(false);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    // Create General pages
    m_FullReleasePage = new FullReleasePage(this);

    ui = new Ui::MainWindow;
    ui->setupUi(this);
    ui->centralWidget->layout()->setMargin(0);

    ui->splitter->setCollapsible(1, false);
    ui->pageTree->header()->hide();

    connect(ui->pageTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        this, SLOT(pageSelected()));

    return true;
}

/**
 * When all dependent plugins are initialized, start the second initialization:
 * - setup ui
 * - start update checker
 */
void MainWindow::extensionsInitialized()
{
#ifndef WITH_TESTS
    // Start the update checker
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEDDIMANAGER_UPDATE_URL);
        if (checkUpdate())
            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }
#endif

    setWindowTitle(QString("%1 %2 - (c) %3").arg(qApp->applicationName()).arg(qApp->applicationVersion()).arg(tkTr(Trans::Constants::EHR_COMMUNITY)));
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDDIMANAGER));
}

/**
 * Processing after the Core is opened.
 * Update actions translations, check path preferences, show ui
 */
void MainWindow::postCoreInitialization()
{
    actionManager()->retranslateMenusAndActions();
    contextManager()->updateContext();
    raise();
    show();
    readSettings();

    preparePages();

    const QString &path = settings()->value(Constants::S_GITFILES_PATH).toString();
    if (path.isEmpty() || !QDir(settings()->value(Constants::S_GITFILES_PATH).toString()).exists()) {
        Utils::warningMessageBox(tr("Wrong path settings."), tr("You will be redirected to the preferences pages. "
                                                                "Please set the correct path, then restart the application."));
        applicationPreferences();
    }

    setWindowIcon(theme()->icon(Constants::ICONFREETOOLBOX));
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
    setStatusBar(0);
}

/** Processing before closing mainwindow */
void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

/** Get freetoolbox pages and prepare the treeview */
void MainWindow::preparePages()
{
    // TODO: Use the Core::PageWidget
    QList<IToolPage*> pages = ExtensionSystem::PluginManager::instance()->getObjects<IToolPage>();
    pages.prepend(m_FullReleasePage);

    QMap<QString, QTreeWidgetItem *> categories;
    QFont title;
    title.setBold(true);
    if (Utils::isRunningOnMac())
        title.setPointSize(title.pointSize() + 1);
    title.setCapitalization(QFont::AllUppercase);

    int index = 0;
    foreach (IToolPage *page, pages) {
        PageData pageData;
        pageData.index = index;
        pageData.category = page->category();
        pageData.id = page->id();

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, page->name());
        item->setData(0, Qt::UserRole, qVariantFromValue(pageData));

        QStringList categoriesId = page->category().split(QLatin1Char('|'));
        QStringList trCategories = page->category().split(QLatin1Char('|'));
        QString currentCategory = categoriesId.at(0);

        QTreeWidgetItem *treeitem;
        if (!categories.contains(currentCategory)) {
            treeitem = new QTreeWidgetItem(ui->pageTree);
            treeitem->setText(0, trCategories.at(0));
            treeitem->setForeground(0, QBrush(QColor("#728194")));
            treeitem->setData(0, Qt::UserRole, qVariantFromValue(pageData));
            treeitem->setFont(0, title);
            QSize s = treeitem->sizeHint(0);
            if (Utils::isRunningOnMac())
                s.setHeight(30);
            else
                s.setHeight(20);
            treeitem->setSizeHint(0, s);
            categories.insert(currentCategory, treeitem);
        }

        int catCount = 1;
        while (catCount < categoriesId.count()) {
            if (!categories.contains(currentCategory + QLatin1Char('|') + categoriesId.at(catCount))) {
                treeitem = new QTreeWidgetItem(categories.value(currentCategory));
                currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
                treeitem->setText(0, trCategories.at(catCount));
                treeitem->setData(0, Qt::UserRole, qVariantFromValue(pageData));
                categories.insert(currentCategory, treeitem);
            } else {
                currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
            }
            ++catCount;
        }

        categories.value(currentCategory)->addChild(item);

//        pages.append(page);
        ui->stackedPages->addWidget(page->createPage(ui->stackedPages));

        if (page->id() == m_currentPage && currentCategory == m_currentCategory) {
            ui->stackedPages->setCurrentIndex(ui->stackedPages->count());
            ui->pageTree->setCurrentItem(item);
        }

        index++;
    }

    ui->pageTree->expandAll();
    ui->splitter->setStretchFactor(ui->splitter->indexOf(ui->pageTree), 1);
    ui->splitter->setStretchFactor(ui->splitter->indexOf(ui->layoutWidget), 2);
}

/** When a page is selected shows the corresponding widget */
void MainWindow::pageSelected()
{
    QTreeWidgetItem *item = ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();
    int index = data.index;
    m_currentCategory = data.category;
    m_currentPage = data.id;
    ui->stackedPages->setCurrentIndex(index);
}

/** Show help of the current page (not coded) */
void MainWindow::showHelp()
{
//    QTreeWidgetItem *item = ui->pageTree->currentItem();
//    const PageData &data = item->data(0, Qt::UserRole).value<PageData>();
//    int index = data.index;
//    Core::HelpDialog::showPage(m_pages.at(index)->helpPage());
}

/** Save mainwindow settings */
void MainWindow::saveSettings()
{
    settings()->setValue("LastPreferenceCategory", m_currentCategory);
    settings()->setValue("LastPreferencePage", m_currentPage);
    // Save geometry
    settings()->setValue(Constants::S_WINGEOMETRY, saveGeometry());
    settings()->setValue(Constants::S_WINSTATE, saveState());
    settings()->setValue(Constants::S_WINSPLITTER, ui->splitter->saveState());
    settings()->sync();
}

/** Read mainwindow settings */
void MainWindow::readSettings()
{
    m_currentCategory = settings()->value("LastPreferenceCategory").toString();
    m_currentPage = settings()->value("LastPreferencePage").toString();
    // Restore geometry
    restoreGeometry(settings()->value(Constants::S_WINGEOMETRY).toByteArray());
    restoreState(settings()->value(Constants::S_WINSTATE).toByteArray());
    ui->splitter->restoreState(settings()->value(Constants::S_WINSPLITTER).toByteArray());
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
        break;
    default:
        break;
    }
}
