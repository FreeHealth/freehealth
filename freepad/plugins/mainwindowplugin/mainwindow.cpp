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
#include "mainwindow.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/filemanager.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/dialogs/helpdialog.h>
//#include <coreplugin/idocumentprinter.h>

#include <texteditorplugin/texteditor.h>

#include <padtoolsplugin/tokenmodel.h>
#include <padtoolsplugin/padwriter.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>

#include "ui_mainwindow.h"

#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QStatusBar>
#include <QFileDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ICommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
static inline Core::IPadTools *padTools() { return Core::ICore::instance()->padTools(); }
//static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {theme()->finishSplashScreen(w); }

MainWindow::MainWindow(QWidget *parent) :
    Core::IMainWindow(parent),
    m_TokenModel(0),
    m_Writer(0)
{
    setObjectName("MainWindow");
    messageSplash(tr("Creating Main Window"));

    // Install the event filter
//    qApp->installEventFilter(this);
}

/** \brief Create the menubar and the MainWindow actions. */
bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // create menus
    createFileMenu();
    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));
    createEditMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FileNew  |
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrint |
            Core::MainWindowActions::A_FilePrintPreview |
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
            Core::MainWindowActions::A_CheckUpdate //|
//            Core::MainWindowActions::A_QtAbout
            );
    actions.createEditActions(true);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    // Creating MainWindow interface
//    m_ui = new Internal::Ui::MainWindow();
//    m_ui->setupUi(this);
//    m_ui->padTextEdit->toogleToolbar(true);
//    resize(1024, 768);
//    m_ui->splitterMain->setSizes(QList<int>() << 150);
//    m_ui->splitterErrors->setSizes(QList<int>() << 0 << 100);

    return true;
}

/**
 * \brief Before MainWindow is shown.
*/
void MainWindow::extensionsInitialized()
{
    finishSplash(this);
    readSettings();
    show();
    raise();

    // Connect post core initialization
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void MainWindow::padTextChanged()
{
	QList<Core::PadAnalyzerError> errors;
	// TODO : use a timer based on key strokes instead of realtime analysis
//**    m_ui->previewTextEdit->setPlainText(m_padTools->parse(m_ui->padTextEdit->textEdit()->toPlainText(), m_TokenModel->tokens(), errors));

//**	m_ui->listWidgetErrors->clear();
	foreach (const Core::PadAnalyzerError &error, errors) {
		switch (error.errorType()) {
		case Core::PadAnalyzerError::Error_UnexpectedChar:
//**			m_ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at line %2 and pos %3").arg(error.errorTokens()["char"].toString()).arg(error.line()).arg(error.pos()));
			break;
		case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
//**			m_ui->listWidgetErrors->addItem(tr("Expected '%1' at line %2 and pos %3").arg(error.errorTokens()["char"].toString()).arg(error.line()).arg(error.pos()));
			break;
		}
	}
}

void MainWindow::tokenChanged(const QString &token, const QString &value)
{
    refreshTokens();
}

void MainWindow::tokenItemChanged(QTreeWidgetItem *, int)
{
	refreshTokens();
}

void MainWindow::refreshTokens()
{
	padTextChanged();
}

MainWindow::~MainWindow()
{
    delete m_Writer; // Avoid a segfaulting when trying to access ICore::contextManager()
}

void MainWindow::postCoreInitialization()
{
    m_Writer = padTools()->createWriter(this);
    setCentralWidget(m_Writer);

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEPAD));

    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEPAD_UPDATE_URL);
        if (checkUpdate())
            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }
}


/**
  \brief Close the main window and the application
  \todo Add  ICoreListener
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    Utils::Log::addMessage(this, "Closing MainWindow");
    Core::ICore::instance()->requestSaveSettings();
    Core::ICore::instance()->coreIsAboutToClose();
    writeSettings();
    event->accept();
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
//**        if (m_ui)
//**            m_ui->retranslateUi(this);
        actionManager()->retranslateMenusAndActions();
    }
}

/** \brief Event filter created to catch the FileOpenEvent from MacOS X */
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fe = static_cast<QFileOpenEvent *>(event);
        if (fe) {
            readFile(fe->file());
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
}

/** \brief Populate recent files menu */
void MainWindow::aboutToShowRecentFiles()
{
    Core::ActionContainer *aci = actionManager()->actionContainer(Core::Constants::M_FILE_RECENTFILES);
    aci->menu()->clear();

    bool hasRecentFiles = false;
    foreach (const QString &fileName, fileManager()->recentFiles()) {
        hasRecentFiles = true;
        QAction *action = aci->menu()->addAction(fileName);
        action->setData(fileName);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    aci->menu()->setEnabled(hasRecentFiles);
}

/** \brief Opens a recent file. This solt must be called by a recent files' menu's action. */
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QString fileName = action->data().toString();
    if (!fileName.isEmpty()) {
        readFile(fileName);
    }
}

void MainWindow::updateCheckerEnd(bool error)
{
    Q_UNUSED(error);
    // this code avoid deletion of the resizer corner of the mainwindow
    delete statusBar();
    statusBar()->hide();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    settings()->restoreState(this, Constants::S_STATEPREFIX);
    fileManager()->getRecentFilesFromSettings();
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    settings()->saveState(this, Constants::S_STATEPREFIX);
    fileManager()->saveRecentFiles();
    settings()->sync();
}

/** \obsolete */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tkTr(Trans::Constants::READY), 2000 );
}

bool MainWindow::newFile()
{
//    if (drugModel()->drugsList().count()) {
//        bool yes = Utils::yesNoMessageBox(
//                tr("Save actual prescription ?"),
//                tr("The actual prescription is not empty. Do you want to save it before creating a new one ?"));
//        if (yes) {
//            saveFile();
//        }
//    }
//    patient()->clear();
//    refreshPatient();
//    drugModel()->clearDrugsList();
    return true;
}

/**
  \brief Open the preferences dialog
  \sa mfDrugsPreferences
*/
bool MainWindow::applicationPreferences()
{
    Core::SettingsDialog dlg(this);
    dlg.exec();
    return true;
}

/** \brief Change the font of the viewing widget */
void MainWindow::changeFontTo( const QFont &font )
{
	Q_UNUSED(font);
}

bool MainWindow::saveAsFile()
{
    // TODO: save filename */
//    return saveIcdCollection();
	return false;
}

bool MainWindow::saveFile()
{
    return true;
}

bool MainWindow::openFile()
{
    QString f;
//    = QFileDialog::getOpenFileName(this,
//                                             tkTr(Trans::Constants::OPEN_FILE),
//                                             QDir::homePath(),
//                                             QCoreApplication::translate(ICD::Constants::ICDCONSTANTS_TR_CONTEXT, ICD::Constants::FREEICD_FILEFILTER) );
    if (f.isEmpty())
        return false;
    //    QString f = "/Users/eric/prescription.di";
    readFile(f);
    fileManager()->setCurrentFile(f);
    fileManager()->addToRecentFiles(f);
    return true;
}

void MainWindow::readFile(const QString &file)
{
	Q_UNUSED(file);
//    m_ui->widget->openFile(file);
//    patient()->fromXml(datas);
//    refreshPatient();
}

