/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
#include "mainwindow.h"
#include "dialogs/plugindialog.h"

#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/log.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>

#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>



// include freemedforms headers
//#include <mfPrinter.h>
//#include <mfAbstractWidget.h>
//#include <mfDialogInterpret.h>
//#include <mfPluginsManager.h>
//#include <mfSettings.h>
//#include <mfAboutDialog.h>

// include toolkit headers
//#include <tkDebugDialog.h>
//#include <tkTheme.h>
//#include <tkActionManager.h>
//#include <tkContextManager.h>
//#include <tkConstantTranslations.h>

// include usertoolkit headers
//#include <tkUserManager.h>

// include Qt headers
#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QStatusBar>

// #include "form_editor_window.h"


using namespace Core;
using namespace Core::Internal;

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent )
          : MainWindowActionHandler(parent)//, fileToolBar(0), editToolBar(0),
//          m_pRootObject(0), m_MainWidget(0), m_AutomaticSaveInterval(0), m_TimerId(0),
//          m_UserManager(0)
{
    setObjectName( "Core::MainWindow" );
    recentFiles.clear();
    setWindowTitle( qApp->applicationName() );
}

bool MainWindow::initialize()
{
    qWarning() << "mainWin initialize";
    // Manage window
    createMenusAndActions();
    createToolBars();
//    createMenus();
    createStatusBar();
    retranslateUi();

//    readSettings();
//    restoreState( windowState );
////    refreshToolbarMenu();
//
//    if (tkGlobal::isDebugCompilation())
//        setCentralWidget( Core::ICore::settings()->getTreeWidget( this ) );
//
//    // Some things need to be set just after the form show
//    QTimer::singleShot( 0, this, SLOT( afterTheShow() ) );
//
//    // Make connections with menus/actions
//    QMetaObject::connectSlotsByName(this);
    return true;
}

bool MainWindow::extensionInitialized()
{
    show();
//    aboutPlugins();
    return true;
}

MainWindow::~MainWindow()
{
}

void MainWindow::afterTheShow()
{
//    // Open the last form
//    if ( ( m_OpenLastOpenedForm ) && ( recentFiles.count() ) ) {
//        QFile file( recentFiles[0] );
//        if ( file.exists() )
//            loadFile( file.fileName() );
//    }
}

QStatusBar *MainWindow::statusBar()
{
    return QMainWindow::statusBar();
}

bool MainWindow::aboutQt()
{
    aboutPlugins();
    return true;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

//void MainWindow::showWidget()
//{
//    if ( m_pRootObject == 0 )
//        return;
//    m_MainWidget = new mfMainWidget(m_pRootObject, this);
//    setCentralWidget( m_MainWidget );
//    // each time a widget is added to this main window restoreState should be called
//    restoreState( windowState );
//}
//
//void MainWindow::on_actionInterpretor_triggered()
//{
//#ifdef DEBUG
//    if ( !m_pRootObject )
//    {
//        QMessageBox::warning( this, qApp->applicationName(),
//                              tr( "No existing form to interpret." ) );
//        return;
//    }
//    mfDialogInterpret * interpret = new mfDialogInterpret( m_pRootObject , this );
//    interpret->exec();
//#else
//    QMessageBox::information( this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED) );
//#endif
//}
//
//void MainWindow::userManagerRequested()
//{
//    if (!m_UserManager) {
//        m_UserManager = new tkUserManager(this);
//        m_UserManager->initialize();
//        m_UserManager->show();
//    } else {
//        qApp->setActiveWindow(m_UserManager);
//        m_UserManager->activateWindow();
//    }
//}
//
////--------------------------------------------------------------------------------------------------------
////------------------------------------------ MANAGING EVENTS ---------------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::closeEvent( QCloseEvent *event )
//{
//    if ( maybeSave() ) {
//        thread.quit();
//        this->killTimer( m_TimerId );
//        writeSettings();
//        thread.wait();
//        event->accept();
//    }
//    else
//        event->ignore();
//}
//
//
//void MainWindow::timerEvent( QTimerEvent *event )
//{
//    if ( event->timerId() == m_TimerId )
//        thread.saveRecoveringFile( m_pRootObject );
//}
//
//
////--------------------------------------------------------------------------------------------------------
////------------------------------------------- File Menu Slots --------------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::documentWasModified()
//{
//    // Function connected with m_pRootObject valueChanged signal
//    if ( isWindowModified() ) return;
//    // Start a timer for automatic save
//    statusBar()->showMessage( tr( "Starting crash recoverer every %1 seconds." )
//                              .arg( m_AutomaticSaveInterval ), 1000 );
//    m_TimerId = this->startTimer( m_AutomaticSaveInterval * 1000 );
//    thread.saveRecoveringFile( m_pRootObject );
//    setWindowModified( true );
//}
//
//bool MainWindow::newFile()
//{
//    return false;
//}
//
//bool MainWindow::openFile()
//{
//    // TODO: here */
//    if ( maybeSave() ) {
//        QString fileName = QFileDialog::getOpenFileName( this , "", mfCore::settings()->formPath() );
//        if ( !fileName.isEmpty() )
//            loadFile(fileName);
//    }
//    return true;
//}
//
//bool MainWindow::saveFile()
//{
//    if ( !m_pRootObject )
//        return false;
//    if ( m_CurrentFile.isEmpty() )
//        return saveAsFile();
//    else
//        return saveFile( m_CurrentFile );
//}
//
//bool MainWindow::saveAsFile()
//{
//    QString fileName = QFileDialog::getSaveFileName( this );
//    if ( fileName.isEmpty() )
//        return false;
//    return saveFile( fileName );
//}
//
//bool MainWindow::saveFile( const QString &fileName )
//{
//#ifdef DEBUG
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if ( !ioPlugin )
//        return false;
//
//    if ( ioPlugin->saveForm( mfIOPlace::fromLocalFile( fileName ), *m_pRootObject ) ) {
//        thread.quit();
//        // Manage currentFile, status bar and modification state
//        setCurrentFile( fileName );
//        statusBar()->showMessage( tr( "File saved" ), 2000 );
//        setWindowModified( false );
//        // Manage Timer and Thread (that needs to be stopped if running)
//        this->killTimer( m_TimerId );
//        m_TimerId = 0;
//        thread.wait();
//        return true;
//    }
//    return false;
//#else
//    QMessageBox::information( this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED) );
//    return true;
//#endif
//}
//
//bool MainWindow::print()
//{
//    if ( !m_pRootObject ) return false;
//    mfPrinter printer( m_pRootObject );
//    printer.print();
//    return true;
//}
//
//bool MainWindow::maybeSave()
//{
//    // Test if form was modified --> ask user what to do
//    if ( m_pRootObject )
//        if ( m_pRootObject->isModified() ) {
//        int ret = QMessageBox::warning( this, qApp->applicationName(),
//                                        tr( "The document has been modified.\n"
//                                            "Do you want to save your changes?" ),
//                                        QMessageBox::Yes | QMessageBox::Default,
//                                        QMessageBox::No,
//                                        QMessageBox::Cancel | QMessageBox::Escape );
//        if ( ret == QMessageBox::Yes )
//            return saveFile();
//        else
//            if ( ret == QMessageBox::Cancel )
//                return false;
//    }
//    return true;
//}
//
//void MainWindow::openRecentFile()
//{
//    QAction *action = qobject_cast<QAction *>( sender() );
//    if ( action )
//        loadFile( action->data().toString() );
//}
//
//void MainWindow::refreshWholeUi()
//{
//    // Delete all object that need to be deleted
//    if (m_MainWidget)
//        delete m_MainWidget;
//    m_MainWidget = 0;
//    showWidget();
//    connect( m_pRootObject, SIGNAL( formUiMustBeUpdated() ),
//             this, SLOT( refreshWholeUi() ) );
//}
//
//void MainWindow::loadFile( const QString &fileName )
//{
//    if (m_pRootObject)
//        delete m_pRootObject;
//    m_pRootObject = 0;
//
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if (!ioPlugin) {
//        tkLog::addError( this, tr( "Main Window ERROR : Laoding form : no IO plugin defined." ) );
//        return;
//    }
//
//    // Read fileName and refresh gui
//    m_pRootObject = ioPlugin->loadForm( mfIOPlace::fromLocalFile( fileName ) );
//    if (m_pRootObject) {
//        setCurrentFile( fileName );
//        statusBar()->showMessage( tr( "File opened" ), 2000 );
//        refreshWholeUi();
//        tkLog::addMessage( this, tr ( "Main Window INFO : Form %1 correctly loaded" ).arg( fileName ) );
//    } else {
//        tkGlobal::warningMessageBox(tr("An error occured while reading file : %1. No elements were read.").arg(fileName) ,"","",qApp->applicationName());
//        return;
//    }
//
//    // Connect mfObject with documentWasModified
//    connect( m_pRootObject, SIGNAL( valueChanged() ), this, SLOT( documentWasModified() ) );
//}
//
//void MainWindow::setCurrentFile( const QString &fileName )
//{
//    m_CurrentFile = fileName;
//    setWindowModified( false );
//
//    QString shownName;
//    if ( m_CurrentFile.isEmpty() )
//        shownName = tr( "untitled" ) + ".fmf";
//    else {
//        shownName = strippedName( m_CurrentFile );
//        recentFiles.removeAll( m_CurrentFile );
//        recentFiles.prepend( m_CurrentFile );
//        updateRecentFileActions();
//    }
//    setWindowTitle( tr( "%1 [*] - %2" ).arg( shownName ).arg( qApp->applicationName() ) );
//}
//
//void MainWindow::updateRecentFileActions()
//{
////    QMutableStringListIterator it( recentFiles );
////    // Show only existing files
////    while ( it.hasNext() )
////        if ( !QFile::exists( it.next() ) )
////            it.remove();
////
////    int numRecentFiles = qMin( recentFiles.size(), (int)MaxRecentFiles );
////
////    for ( int i = 0; i < numRecentFiles; ++i ) {
////        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( recentFiles[i] ) );
////        recentFilesAct[i]->setText( text );
////        recentFilesAct[i]->setData( recentFiles[i] );
////        recentFilesAct[i]->setVisible( true );
////    }
////    for ( int j = numRecentFiles; j < MaxRecentFiles; ++j )
////        recentFilesAct[j]->setVisible( false );
////
////    separatorAct->setVisible( numRecentFiles > 0 );
//}
//
////--------------------------------------------------------------------------------------------------------
////------------------------------------------- Help Menu Slots --------------------------------------------
////--------------------------------------------------------------------------------------------------------
//bool MainWindow::aboutThisForm()
//{
//    if (!m_pRootObject)
//        return false;
//    QString msg = "<p align=center><b>";
//    QString tmp;
//    msg.append( tr( "Extra-information of the Form <br />%1" )
//                .arg( m_pRootObject->label() ) );
//    msg.append( "</b></p>" );
//    msg.append( "<ul>" );
//
//    tmp = m_pRootObject->param( mfObject::Param_Authors ).toStringList().join( "<br />" );
//    if ( !tmp.isEmpty() ) {
//        msg.append( "<li>" );
//        msg.append ( tr( "Author(s) :<br />%1" ).arg( tmp ) );
//        msg.append( "</li>" );
//    }
//    tmp = m_pRootObject->param( mfObject::Param_CreationDate ).toString();
//    if ( !tmp.isEmpty() ) {
//        msg.append( "<li>" );
//        msg.append ( tr( "Date : %1" ).arg( tmp ) );
//        msg.append( "</li>" );
//    }
//    tmp = m_pRootObject->param( mfObject::Param_Version ).toString();
//    if ( !tmp.isEmpty() ) {
//        msg.append( "<li>" );
//        msg.append ( tr( "Version : %1" ).arg( tmp ) );
//        msg.append( "</li>" );
//    }
//
//    tmp = m_pRootObject->param( mfObject::Param_License ).toString();
//    if ( !tmp.isEmpty() ) {
//        msg.append( "<li>" );
//        msg.append ( tr( "License : %1" ).arg( tmp ) );
//        msg.append( "</li>" );
//    }
//
//    tmp = m_pRootObject->param( mfObject::Param_Bibliography ).toStringList().join( "<br />" );
//    if ( !tmp.isEmpty() ) {
//        msg.append( "<li>" );
//        msg.append ( tr( "Bibliography : <br />%1" ).arg( tmp ) );
//        msg.append( "</li>" );
//    }
//    msg.append( "</ul>" );
//
//    QMessageBox::information( this, qApp->applicationName(), msg );
//
//    return true;
//}
//
//bool MainWindow::aboutApplication()
//{
//    mfAboutDialog dialog(this);
//    dialog.exec();
//    return true;
//}
//
//bool MainWindow::debugDialog()
//{
//    tkDebugDialog d( this, mfCore::settings() );
//    d.exec();
//    return true;
//}
//
//void MainWindow::on_actionHelpTextToggler_triggered()
//{
//    m_HelpTextShow = !m_HelpTextShow;
//
//    foreach ( mfAbstractWidget *widget, m_MainWidget->findChildren<mfAbstractWidget*>() )
//    {
//        if ( widget->getObject()->type() == mfObject::Type_HelpText )
//            widget->setVisible( m_HelpTextShow );
//    }
//}
//
//
////--------------------------------------------------------------------------------------------------------
////--------------------------------------- Creating Actions and Menus -------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::refreshToolbarMenu()
//{
//    // clear toolbar menu then re-populate it
////    QMenu *m = mfCore::actionManager()->menu(M_TOOLBARS);
////    m->clear();
////    QList<QToolBar*> toolbars = this->findChildren<QToolBar*>();
////    foreach( QToolBar* tb, toolbars ) {
////        if ( tb->windowTitle().isEmpty() )
////            continue;
////        QAction * a = m->addAction( tb->windowTitle() );
////        a->setObjectName( tb->objectName() );
////        a->setCheckable( true );
////        a->setChecked( tb->isVisible() );
////        connect( a, SIGNAL( toggled( bool ) ), this, SLOT( toolbarToggled( bool ) ) );
////    }
//}
//
//void MainWindow::formEditor()
//{
//    //      FormEditorWindow *window = new FormEditorWindow( this );
//    //      window->show();
//}
//
void MainWindow::createMenusAndActions()
{
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createPluginsMenu();
    createConfigurationMenu();
    createHelpMenu();

    createFileActions(A_FileNew | A_FileOpen | A_FileSave | A_FileSaveAs | A_FilePrint | A_FileQuit);
    createEditActions();
    createConfigurationActions(A_AppPreferences | A_PluginsPreferences | A_LangageChange);
    createHelpActions(A_AppAbout | A_AppHelp | A_DebugDialog | A_FormsAbout | A_QtAbout);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    QAction *a = 0;
    Command *cmd = 0;
    ActionManager *am = Core::ICore::instance()->actionManager();
    ITheme *theme = Core::ICore::instance()->theme();
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    ActionContainer *menu = am->actionContainer(Constants::M_PLUGINS);
    Q_ASSERT(menu);
    if (!menu)
        return;

    a = aUserManager = new QAction(this);
    a->setIcon(theme->icon(Constants::ICONUSERMANAGER));
    cmd = am->registerAction(a, Constants::A_USERMANAGER, ctx);
    cmd->setTranslations(Trans::Constants::USERMANAGER_TEXT);
    menu->addAction(cmd, Constants::G_PLUGINS_USERMANAGER);
    connect(aUserManager, SIGNAL(triggered()), this, SLOT(userManagerRequested()));

//    for ( int i = 0; i < MaxRecentFiles; ++i )
//    {
//        recentFilesAct[i] = new QAction( this );
//        recentFilesAct[i]->setVisible( false );
//        connect( recentFilesAct[i], SIGNAL( triggered() ), this, SLOT( openRecentFile() ) );
//        am->addAction( recentFilesAct[i], QString("RECENT%1").arg(i), G_FILE_RECENTS );
//    }

//    // Menu interpretor
//    am->createMenu(M_INTERPRETOR, MENUBAR, M_INTERPRETOR_TEXT , "", this); //interpretMenu = menuBar()->addMenu( " " );
//    am->appendGroup(G_INTERPRETOR_GENERAL, M_INTERPRETOR);
//    am->createAction( A_INTERPRETOR_GENERAL, G_INTERPRETOR_GENERAL, this );
//    am->setActionDatas( A_INTERPRETOR_GENERAL, INTERPRETOR_GENERAL_TEXT, INTERPRETOR_GENERAL_TEXT, ""); // interpretMenu->addAction( interpretationAct );
//
//    // Menu tools
//    am->createMenu(M_TOOLS, MENUBAR, M_TOOLS_TEXT , "", this); //interpretMenu = menuBar()->addMenu( " " );
//    am->appendGroup(G_TOOLS_GENERAL, M_TOOLS);
//    am->createAction( A_USERMANAGER, G_TOOLS_GENERAL, this );
//    am->setActionDatas( A_USERMANAGER, USERMANAGER_TEXT, USERMANAGER_TEXT, ICONUSERMANAGER ); // interpretMenu->addAction( interpretationAct );
//
//    // Menu configuration
//    am->createDefaultConfigurationMenu(this);
//    am->createDefaultConfigurationMenuActions(this);
//
//    // Menu languages
//    languageMenu = am->menu(M_LANGUAGES);
//    languageActionGroup = new QActionGroup( this );
//    connect( languageActionGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( switchLanguage( QAction* ) ) );
//    QMap<QString, QString> loc_lang = mfCore::translators()->availableLocalesAndLanguages();
//    int i = 0;
//    foreach( const QString &loc, loc_lang.keys() ) {
//        i++;
//        QAction * action = new QAction( QString( "&%1 %2" ).arg( QString::number( i ) , loc_lang.value( loc ) ), this );
//        action->setCheckable( true );
//        action->setData( loc );
//        languageMenu->addAction( action );
//        languageActionGroup->addAction( action );
//        if ( loc == QLocale().name().left( 2 ) )
//            action->setChecked( true );
//    }

    Core::ICore::instance()->contextManager()->updateContext();
    Core::ICore::instance()->actionManager()->retranslateMenusAndActions();
}


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- ToolBars and StatusBar ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void MainWindow::createToolBars()
{
//    tkActionManager *am = mfCore::actionManager();
//    fileToolBar = addToolBar( "" );
//    fileToolBar->setObjectName( "fileToolBar" ); // objectName() is used for state save/restoration
//    fileToolBar->addAction( am->action( A_FILE_NEW ) );   //newAct );
//    fileToolBar->addAction( am->action( A_FILE_OPEN ) );  //openAct );
//    fileToolBar->addAction( am->action( A_FILE_SAVE ) );  //saveAct );
//    fileToolBar->addAction( am->action( A_FILE_PRINT ) );  //printAct );
//    fileToolBar->addAction( am->action( A_INTERPRETOR_GENERAL ) );  //interpretationAct );
//    fileToolBar->addAction( am->action( A_HELPTEXT_TOGGLER ) );  //helpTextAct );
//    fileToolBar->addAction( am->action( A_ABOUTFORM ) );  //aboutThisFormAct );
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ) );
}

void MainWindow::aboutPlugins()
{
    PluginDialog dialog(this);
    dialog.exec();
}


////--------------------------------------------------------------------------------------------------------
////-------------------------------------- Settings --------------------------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::readSettings()
//{
//    statusBar()->showMessage( tr( "Reading settings" ) );
//
//    // Main Application settings
//    mfCore::settings()->restoreState( this );
//    recentFiles = mfCore::settings()->value( SETTING_RECENTS ).toStringList();
//    m_AutomaticSaveInterval = mfCore::settings()->value( SETTING_SAVEINTERVAL , 600 ).toUInt(); // Default = 10 minutes
//    m_OpenLastOpenedForm = mfCore::settings()->value( SETTING_OPENLAST , true ).toBool();
//
//    // Main Widget settings
//    m_HelpTextShow = mfCore::settings()->value( SETTING_SHOWHELPTEXT, true ).toBool();
//    updateRecentFileActions();
//
//    // Notify
//    statusBar()->showMessage( tkTr(SETTINGS_RECOVERED), 2000 );
//}
//
//void MainWindow::writeSettings()
//{
//    mfCore::settings()->saveState( this );
//    // Main Application settings
//    mfCore::settings()->setValue( SETTING_RECENTS, recentFiles );
//    mfCore::settings()->setValue( SETTING_SAVEINTERVAL, m_AutomaticSaveInterval );
//    mfCore::settings()->setValue( SETTING_OPENLAST, m_OpenLastOpenedForm );
//    // Main Widget settings
//    mfCore::settings()->setValue( SETTING_SHOWHELPTEXT, m_HelpTextShow );
//}
//
//QString MainWindow::strippedName( const QString &fullFileName )
//{
//    return QFileInfo(fullFileName).fileName();
//}
//
//void MainWindow::changeEvent( QEvent * event )
//{
//    if (event->type() == QEvent::LanguageChange) {
//        retranslateUi();
//        event->accept();
//    }
//    else
//        QWidget::changeEvent( event );
//}
//

void MainWindow::retranslateUi()
{
//    fileToolBar->setWindowTitle( tr( "&File" ) );
}


//void MainWindow::toolbarToggled( bool state )
//{
//    QAction * a = qobject_cast<QAction*>( sender() );
//    // find toolbar and hide it
//    QToolBar* tb = findChild<QToolBar*>( a->objectName() );
//    if ( tb ) tb->setVisible( state );
//}
//
//void MainWindow::saveFormData()
//{
//#ifdef DEBUG
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if ( !ioPlugin )
//        return;
//
//    if ( ioPlugin->saveFormData( mfIOPlace::fromLocalFile( m_CurrentFile + ".dat" ), *m_pRootObject ) )
//        tkLog::addMessage( this,  tr( "Form data saved" ) );
//    else
//        tkLog::addError( this, tr( "Failure in form data saving operation." ) );
//#else
//    QMessageBox::information( this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED) );
//#endif
//}
//
//void MainWindow::loadFormData()
//{
//#ifdef DEBUG
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if ( !ioPlugin )
//        return;
//
//    QString fileName = QFileDialog::getOpenFileName(this, tr("choose a file data to load"), mfCore::settings()->formPath());
//    if (fileName == "")
//        return;
//
//    if ( ioPlugin->loadFormData( mfIOPlace::fromLocalFile( fileName ), *m_pRootObject ) )
//    {
//        m_pRootObject->setModified(false);
//        setWindowModified( false );
//        tkLog::addMessage( this, tr( "Form data loaded" ) );
//    }
//    else
//        tkLog::addError( this, tr( "Failure in form data loading operation." ) );
//#else
//    QMessageBox::information( this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED) );
//#endif
//}
