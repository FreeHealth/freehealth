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
/**
  \class tkActionManager
  \brief tkActionManager is the place to hold your actions, menus and menubars.
  * It manages automatic menu/menubar creation and automatic translation as well.
  * You can easily create menuBars using createMenuBar(). MENUBARS ONLY CONTAINS MENUS.\n
  * You can easily create menus using createMenu(). MENUS ONLY CONTAINS GROUPS.\n
  * You can set groups into menus using appendGroup(). GROUP CAN CONTAIN ACTIONS AND MENUS.\n
  * You can easily create actions using createAction() and then setActionDatas(), setActionText(), setActionToolTip(),
  * setActionThemedIcon(), setActionUnthemedIcon(), setActionEnabled(). Themed icon are retreive using the tkTheme manager.\n
  * Use QString as identifiants. Identifiants MUST BE UNIQUE inside the same structure (menuBars, menus, groups and action).
  *
  * If you want to use the automatic translation of menus / actions, use the setContext() before defining menus and actions.\n
  * The context correspond to the translation context (please refer Qt doc QCoreApplication::translate() ). By default, this
  * manager uses the tkConstants context to ask for translations. retranslate() is automatically connected to the tkTranslators
  * signal languageChanged(). To take full advantage of automatic translation, pass a QT_TRANSLATE_NOOP( "context", "What to translate" )
  * or a QT_TR_NOOP("Text to tr") to the functions. Please refer to Qt doc.
  *
  * !!!!  IF YOU DON'T SPECIFY PARENT QOBJECT AT MENUBARS, MENUS AND ACTIONS CREATION TIME,
  * YOU MUST MANAGE DELETION OF MENUBARS, MENUS AND ACTIONS POINTERS.
  *
  * \code
  *    setTrContext("context");
  *    setActionText( id, QT_TRANSLATE_NOOP( "context", "What to translate" ) );  // OK for automatic translations using "context"
  * \endcode
  *
  * \code
  *    setTrContext(className());
  *    setActionText( id, QT_TR_NOOP( "What to translate" ) );   // OK for automatic translation using current class context
  * \endcode
  *
  * \code
  *    setActionText( id, tr( "What to translate" ) );      // No automatic translation can be done if language changes during execution
  * \endcode
  *
  * If you are not using this class inside FreeMedForms, you can create basic menuBar using createDefaultMenusAndGroups(),
  * or createDefaultFileMenu(), createDefaultEditMenu(), createDefaultFormatMenu(), createDefaultAboutMenu()
  * and populate these menus with classicals actions using createDefaultActions() or createDefaultFileMenuActions(),
  * createDefaultEditMenuActions(), createDefaultFormatMenuActions(), createDefaultAboutMenuActions(). \n
  * If you are using this manager in your plugins please DO NOT USE these functions.
  *
  * When you have setted all your menubars/menus/groups/actions or when you add something you must call refreshAll() or
  * refreshMenu() or refreshMenuBar() in order to create/refresh the item. Unless that, you will not have the correct item.
  *
  * You can automatically connect menuBars/menus/actions to slots like this :
  * \code
  *    // According your class is a QObject/QWidget with macro Q_OBJECT setted.
  *    m->createMenu( "myMenu", "parentMenu", "Text", "Icon.png", this);  // DON'T FORGET THE OWNERSHIP (parent) TO CONNECT SIGNALS
  *    m->createAction( "action", G_EDIT_SELECT, this);             // DON'T FORGET THE OWNERSHIP (parent) TO CONNECT SIGNALS
  *    m->setActionDatas( "action", "Automatic slot connection" );
  *    QMetaObject::connectSlotsByName(this);
  *    // --> Q_SLOT on_action_triggered() { is automacally connect to action SIGNAL(triggered()) }
  *    // --> Q_SLOT on_myMenu_aboutToShow() { is automacally connect to myMenu SIGNAL(aboutToShow()) }
  * \endcode
  \ingroup toolkit
  \ingroup object_toolkit
  \sa constants_actionmanager
*/

#include "tkActionManager.h"

#include <tkLog.h>
#include <tkTheme.h>
#include <tkTranslators.h>
#include <tkConstantTranslations.h>

#include <QAction>
#include <QApplication>
//#include <QObject>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

// Some constants to mark the Ids of Objects
const char * const MENUBARPREFIX = "B_";
const char * const MENUPREFIX = "M_";
const char * const ACTIONPREFIX = "A_";
const char * const GROUPPREFIX = "G_";
const char * const MARK = "tkActionManager::Mark";

// Some contants to setProperties of Objects
const char * const TR_CONTEXTID   = "tkAMPrivate::context";
const char * const UNTR_TEXT      = "tkAMPrivate::text";
const char * const UNTR_TOOLTIP   = "tkAMPrivate::tooltip";
const char * const THEMED_ICON    = "tkAMPrivate::themedicon";
const char * const UNTHEMED_ICON  = "tkAMPrivate::unthemedicon";

class tkActionManagerPrivate //: public QObject
{    
public:    
    tkActionManagerPrivate() :
            m_ActualContext(0),
            m_isDirty(false)
    {
        m_TrContextList << "tkConstants" ;  // add default translation context
    }
    
    
    ~tkActionManagerPrivate()
    {
//        qDeleteAll(m_MenuBars.values());
//        qDeleteAll(m_Menus.values());
//        qDeleteAll(m_Actions.values());
        m_MenuBars.clear();
        m_Menus.clear();
        m_Groups.clear();
        m_Actions.clear();
    }
    
    void setTextProperties( QObject * obj, const QString &unTrText, const QString &unTrToolTip=QString::null, const int trContextIndex = -1 )
    {
        if (trContextIndex != -1)
            obj->setProperty( TR_CONTEXTID, unTrText );
        if (!unTrText.isEmpty())
            obj->setProperty( UNTR_TEXT, unTrText );
        if (!unTrToolTip.isEmpty())
            obj->setProperty( UNTR_TOOLTIP, unTrToolTip );
    }
    void setIconProperties( QObject * obj, const QString &thIcon=QString::null, const QString &unThIcon=QString::null )
    {
        if (!thIcon.isEmpty()) {
            obj->setProperty( THEMED_ICON, thIcon );
            obj->setProperty( "icon", tkTheme::icon( thIcon ) );
        }
        if (!unThIcon.isEmpty()) {
            obj->setProperty( UNTHEMED_ICON, unThIcon );
            obj->setProperty( "icon", QIcon( unThIcon ) );
        }
    }

    void mark( QAction *obj, const QString &id )  { obj->setProperty(MARK, QString(ACTIONPREFIX+id) ); }
    void mark( QMenu *obj, const QString &id )    { obj->setProperty(MARK, QString(MENUPREFIX+id) ); }
    void mark( QMenuBar *obj, const QString &id ) { obj->setProperty(MARK, QString(MENUBARPREFIX+id) ); }
    QString getFullId( QObject *obj ) { return obj->property(MARK).toString(); }

    QMenuBar *menuBar( const QString &id)
    {
        Q_ASSERT_X(menuBarExists(id), "tkActionManagerPrivate::menuBar","id does not exists");
        if (id.startsWith(MENUBARPREFIX))
            return m_MenuBars[id];
        else
            return m_MenuBars[QString(MENUBARPREFIX+id)];
    }
    QMenu *menu( const QString &id )
    {
        Q_ASSERT_X(menuExists(id), "tkActionManagerPrivate::menu","menuId does not exists");
        if (id.startsWith(MENUPREFIX))
            return m_Menus[id];
        else
            return m_Menus[QString(MENUPREFIX+id)];
    }
    QAction *action( const QString &actionId )
    {
        Q_ASSERT_X(actionExists(actionId), "tkActionManagerPrivate::action","actionId does not exists");
        if (actionId.startsWith(ACTIONPREFIX))
            return m_Actions[actionId];
        else
            return m_Actions[QString(ACTIONPREFIX+actionId)];
    }

    void refreshMenuBar( const QString &menuId )
    {
        Q_ASSERT_X(menuBarExists(menuId), "tkActionManagerPrivate::refreshMenuBar","menuId does not exists");
        QString id = menuId;
        if (!id.startsWith(MENUBARPREFIX))
            id.prepend(MENUBARPREFIX);
        QMenuBar *mb = m_MenuBars[id];
        foreach( const QString &childMenu, menuBarChildren(menuId)) {
            QMenu *m = populateMenu(childMenu);
            mb->addMenu(m);
        }
    }
    void refreshMenu( const QString &menuId)
    {
        Q_ASSERT_X(menuExists(menuId), "tkActionManagerPrivate::refreshMenu","menuId does not exists");
        QString id = menuId;
        if (!id.startsWith(MENUPREFIX))
            id.prepend(MENUPREFIX);
        populateMenu(id);
    }
    void refreshAll()
    {
        foreach( const QString &id, m_Menus.keys() )
            refreshMenu(id);
        foreach( const QString &id, m_MenuBars.keys() )
            refreshMenuBar(id);
    }

    QMenu *populateMenu( const QString &menuId )
    {
        Q_ASSERT_X( menuExists(menuId),"tkActionManagerPrivate::populateMenu","menuId does not exists");
        QMenu * m = 0;
        m = menu(menuId);
        m->clear();
        foreach(const QString &child, menuChildren(menuId)) {//m_Menu_Contents.value(menuId)) {
            if (child.startsWith(MENUPREFIX)) {
                QMenu *childMenu = populateMenu(child);
                m->addMenu(childMenu);
                m->addSeparator();
            }
            else if (child.startsWith(GROUPPREFIX))
                addGroupToMenu( m, child );
            else if (child.startsWith(ACTIONPREFIX)) {
                m->addAction(action(child));
            }
        }
        return m;
    }
    
    void addGroupToMenu( QMenu *m, const QString &groupId )
    {
        Q_ASSERT_X(groupExists(groupId), "tkActionManagerPrivate::addGroupToMenu", "group must exists");
        int i = 0;
        QString tmp = "group: " + groupId;
        foreach( const QString &child, m_Groups.value(groupId) ) {
            if (child.startsWith(GROUPPREFIX)) {
                addGroupToMenu( m, child );
                ++i;
                tmp += " + group: " + child;
            } else if (child.startsWith(MENUPREFIX)) {
                m->addMenu( menu(child) );
                ++i;
                tmp += " + menu: " + child;
            } else if (child.startsWith(ACTIONPREFIX)) {
                m->addAction( action(child) );
                ++i;
                tmp += " + action: " + child;
            }
        }
        if (i>0)
            m->addSeparator();
    }
        
    bool menuBarExists(const QString &id)   { if (m_MenuBars.contains(QString(MENUBARPREFIX+id))) return true; else return m_MenuBars.contains(QString(id));}
    bool menuExists(const QString &id)      { if (m_Menus.contains(QString(MENUPREFIX+id))) return true; else return m_Menus.contains(id);}
    bool groupExists(const QString &id)     { if (m_Groups.contains(QString(GROUPPREFIX+id))) return true; else return m_Groups.contains(id);}
    bool actionExists(const QString &id)    { if (m_Actions.contains(QString(ACTIONPREFIX+id))) return true; else return m_Actions.contains(id);}
    bool itemExists(const QString &id)      { return menuBarExists(id) || menuExists(id) || groupExists(id) || actionExists(id); }
    
    void insertMenuBar( QMenuBar *mb, const QString &id )
    {
        Q_ASSERT_X(!id.isEmpty(), "tkActionManagerPrivate::insertActionToGroup", "group must exists");
        m_MenuBars.insert( QString(MENUBARPREFIX+id), mb);
    }
    void insertGroupToMenu( const QString &groupId, const QString &menuId )
    {
        Q_ASSERT_X(menuExists(menuId), "tkActionManagerPrivate::insertGroupToMenu", "group must exists");
        m_Menu_Contents[QString(MENUPREFIX+menuId)] << QString(GROUPPREFIX+groupId);
        m_Groups[QString(GROUPPREFIX+groupId)] = QStringList();
    }
    void insertActionToGroup( QAction *a, const QString &id, const QString &group )
    {
        Q_ASSERT_X(groupExists(group), "tkActionManagerPrivate::insertActionToGroup", "group must exists");
        m_Actions.insert( QString(ACTIONPREFIX+id), a);
        m_Groups[QString(GROUPPREFIX+group)] << QString(ACTIONPREFIX+id);
    }
    void insertActionToMenu( QAction *a, const QString &id, const QString &menu )
    {
        Q_ASSERT_X(menuExists(menu), "tkActionManagerPrivate::insertActionToMenu", "group must exists");
        m_Actions.insert( QString(ACTIONPREFIX+id), a);
        m_Menu_Contents[QString(MENUPREFIX+menu)] << QString(ACTIONPREFIX+id);
    }
    void insertMenuToGroup( QMenu *m, const QString &id, const QString &group )
    {
        Q_ASSERT_X(groupExists(group), "tkActionManagerPrivate::insertMenuToGroup", "group must exists");
        m_Menus.insert( QString(MENUPREFIX+id), m);
        m_Groups[QString(GROUPPREFIX+group)] << QString(MENUPREFIX+id);
    }
    void insertMenuToMenu( QMenu *m, const QString &id, const QString &toMenu )
    {
        Q_ASSERT_X(menuExists(toMenu), "tkActionManagerPrivate::insertMenuToMenu", "menu must exists");
        m_Menus.insert( QString(MENUPREFIX+id), m);
        m_Menu_Contents[QString(MENUPREFIX+toMenu)] << QString(MENUPREFIX+id);
    }
    void insertMenuToMenuBar( QMenu *m, const QString &id, const QString &toMenuBar )
    {
        Q_ASSERT_X(menuBarExists(toMenuBar), "tkActionManagerPrivate::insertMenuToMenu", "menubar must exists");
        m_Menus.insert( QString(MENUPREFIX+id), m);
        m_Menu_Contents[QString(MENUBARPREFIX+toMenuBar)] << QString(MENUPREFIX+id);
    }
    void insertPopUpMenu( QMenu *m, const QString &id )
    {
        m_Menus.insert( QString(MENUPREFIX+id), m);
    }

    void removeAction( const QString &actionId )
    {
        Q_ASSERT_X(!actionId.isEmpty(), "removeAction", "actionId is empty");
        Q_ASSERT_X(actionExists(actionId), "tkActionManagerPrivate::removeAction","actionId does not exists");
        QString id;
        if (id.startsWith(ACTIONPREFIX))
            id = QString(ACTIONPREFIX+actionId);
        else
            id = actionId;
        // remove action from list
        m_Actions.remove(id);
        // remove all references to this action into trees
        foreach( const QString &contents, m_Menu_Contents.keys() )
            m_Menu_Contents[contents].removeAll(id);
        foreach( const QString &contents, m_Groups.keys() )
            m_Groups[contents].removeAll(id);
    }
    void removeMenu( const QString &menuId )
    {
        Q_ASSERT_X(!menuId.isEmpty(), "removeMenu", "menuId is empty");
        Q_ASSERT_X(menuExists(menuId), "tkActionManagerPrivate::removeMenu","menuId does not exists");
        QString id;
        if (id.startsWith(MENUPREFIX))
            id = QString(MENUPREFIX+menuId);
        else
            id = menuId;
        // remove action from list
        m_Menus.remove(id);
        removeReferencesOf(id);
        // TODO remove childs ?
    }
    void removeMenuBar( const QString &menuId )
    {
        Q_ASSERT_X(!menuId.isEmpty(), "removeMenuBar", "menuId is empty");
        Q_ASSERT_X(menuBarExists(menuId), "tkActionManagerPrivate::removeMenu","menuId does not exists");
        QString id;
        if (id.startsWith(MENUBARPREFIX))
            id = QString(MENUBARPREFIX+menuId);
        else
            id = menuId;
        // remove action from list
        m_MenuBars.remove(id);
        removeReferencesOf(id);
        // TODO remove childs ?
    }
    void removeMenuBar( QMenuBar *mb )
    {
        Q_ASSERT_X(m_MenuBars.values().contains(mb), "removeMenuBar", "No pointer in hash");
        if (m_MenuBars.values().contains(mb))
            removeMenuBar( m_MenuBars.key(mb) );
    }
    void removeMenu( QMenu *m )
    {
        Q_ASSERT_X(m_Menus.values().contains(m), "removeMenu", "No pointer in hash");
        if (m_Menus.values().contains(m))
            removeMenu( m_Menus.key(m) );
    }
    void removeAction( QAction *a )
    {
        Q_ASSERT_X(m_Actions.values().contains(a), "removeAction", "No pointer in hash");
        if (m_Actions.values().contains(a))
            removeAction( m_Actions.key(a) );
    }

    QStringList menuBarChildren( const QString &id)
    {
        Q_ASSERT_X( menuBarExists(id), "tkActionManagerPrivate::menuBarChildren","id does not exists");
        if (id.startsWith(MENUBARPREFIX))
            return m_Menu_Contents.value(id);
        else { // this should never be the case
            return m_Menu_Contents.value(QString(MENUBARPREFIX+id));
        }
    }
    QStringList menuChildren( const QString &id)
    {
        Q_ASSERT_X(menuExists(id), "tkActionManagerPrivate::menuBar","id does not exists");
        if (id.startsWith(MENUPREFIX))
            return m_Menu_Contents.value(id);
        else
            return m_Menu_Contents.value(QString(MENUBARPREFIX+id));
    }
    QStringList groupChildren( const QString &id )
    {
        Q_ASSERT_X(groupExists(id), "tkActionManagerPrivate::menuBar","id does not exists");
        if (id.startsWith(GROUPPREFIX))
            return m_Groups.value(id);
        else
            return m_Groups.value(QString(GROUPPREFIX+id));
    }

    void retranslate()
    {
        foreach( QMenu *m, m_Menus )
            retranslateObject(m);
        foreach( QAction *a, m_Actions )
            retranslateObject(a);
    }
    void retranslateObject( QAction *obj )
    {
        if (!obj->property(TR_CONTEXTID).isValid())
            return;
        QString context = m_TrContextList[obj->property(TR_CONTEXTID).toInt()];
        if (obj->property(UNTR_TEXT).isValid())
            obj->setText( QCoreApplication::translate( context.toAscii(), obj->property(UNTR_TEXT).toString().toAscii() ));
        if (obj->property(UNTR_TOOLTIP).isValid())
            obj->setToolTip( QCoreApplication::translate( context.toAscii(), obj->property(UNTR_TOOLTIP).toString().toAscii() ));
    }
    void retranslateObject( QMenu *obj )
    {
        if (!obj->property(TR_CONTEXTID).isValid())
            return;
        QString context = m_TrContextList[obj->property(TR_CONTEXTID).toInt()];
        if (obj->property(UNTR_TEXT).isValid())
            obj->setTitle( QCoreApplication::translate( context.toAscii(), obj->property(UNTR_TEXT).toString().toAscii() ));
        if (obj->property(UNTR_TOOLTIP).isValid())
            obj->setTitle( QCoreApplication::translate( context.toAscii(), obj->property(UNTR_TOOLTIP).toString().toAscii() ));
    }

private:
    void removeReferencesOf( const QString &id )
    {
        foreach( const QString &contents, m_Menu_Contents.keys() )
            m_Menu_Contents[contents].removeAll(id);
        foreach( const QString &contents, m_Groups.keys() )
            m_Groups[contents].removeAll(id);
    }

public:
    QStringList  m_TrContextList;
    int          m_ActualContext;
    bool         m_isDirty;

private:
    QHash< QString, QStringList > m_Menu_Contents; //  <-- menuBar contents, menuContents (menu only contains groups)
    QHash< QString, QStringList > m_Groups;        //  <-- groups can contain actions and menus
    
    QHash< QString, QAction *>  m_Actions;
    QHash< QString, QMenu *>    m_Menus;
    QHash< QString, QMenuBar *> m_MenuBars;

    // TODO
    QHash< QString, QString >   m_Parent;
    // END TODO

    // TODO
//    QHash<int, tkAction*> m_Context_Actions;
    // END TODO

    QStringList m_Dirty;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////   PUBLIC INTERFACE   //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tkActionManager *tkActionManager::m_Instance = 0;

/*! \brief Return the unique instance of tkActionManager. If instance does not exists it is automacally created. */
tkActionManager *tkActionManager::instance()
{
    if (!m_Instance)
        m_Instance = new tkActionManager(qApp);
    return m_Instance;
}

tkActionManager::tkActionManager( QObject *parent ) :
        QObject(parent), d(0)
{
    setObjectName( "tkActionManager" );
    d = new tkActionManagerPrivate();
    connect( tkTranslators::instance(), SIGNAL(languageChanged()), this, SLOT( retranslate()));
}

/** \brief Destructor */
tkActionManager::~tkActionManager()
{
    if (d)
        delete d;
    d=0;
}

/** \brief Create a default menubar with : menu/groups (File, Edit, About) */
void tkActionManager::createDefaultMenusAndGroups(QWidget * parent)
{
    createMenuBar( MENUBAR, qApp->activeWindow() );
    createDefaultFileMenu(parent);
    createDefaultEditMenu(parent);
    createDefaultFormatMenu(parent);
    createDefaultAboutMenu(parent);
}

/** \brief Create a default menu's structure for File menu. No actions are inserted. */
void tkActionManager::createDefaultFileMenu(QWidget * parent)
{
    if (!d->menuBarExists( MENUBAR ))
        createMenuBar( MENUBAR, qApp->activeWindow() );
    if (d->menuExists( M_FILE ) )
        return;
    tkActionManager *m = this;
    m->createMenu( M_FILE  , MENUBAR, M_FILE_TEXT, "", parent );
    // add groups to menus
    m->appendGroup( G_FILE_NEW,        M_FILE );
    m->appendGroup( G_FILE_OPEN,       M_FILE );
    m->appendGroup( G_FILE_PROJECT,    M_FILE );
    m->appendGroup( G_FILE_SAVE,       M_FILE );
    m->appendGroup( G_FILE_PRINT,      M_FILE );
    m->appendGroup( G_FILE_RECENTS,    M_FILE );
    m->appendGroup( G_FILE_OTHER,      M_FILE );
    m->appendGroup( G_FILE_EXIT,       M_FILE );
}

/** \brief Create a default menu's structure for Edit menu. No actions are inserted. */
void tkActionManager::createDefaultEditMenu(QWidget * parent, const QString &toMenuOrMenuBar)
{
    if (d->menuExists( M_EDIT ) )
        return;
    QString menuParent = toMenuOrMenuBar;
    if (toMenuOrMenuBar.isEmpty()) {
        if (!d->menuBarExists( MENUBAR ))
            createMenuBar( MENUBAR, qApp->activeWindow() );
        menuParent = MENUBAR;
    } else {
        if ((!d->menuExists(toMenuOrMenuBar)) && (!d->menuBarExists(toMenuOrMenuBar))) {
            tkLog::addError( this, tr("%1 default menu can not be created into %2, because %2 does not exists.")
                             .arg("Edit").arg(toMenuOrMenuBar) );
            return;
        }
    }
    tkActionManager *m = this;
    m->createMenu( M_EDIT  , menuParent, M_EDIT_TEXT, "", parent );
    m->appendGroup( G_EDIT_UNDOREDO,   M_EDIT );
    m->appendGroup( G_EDIT_COPYPASTE,  M_EDIT );
    m->appendGroup( G_EDIT_SELECT,     M_EDIT );
    m->createMenu( M_EDIT_LIST  , M_EDIT, M_EDIT_LIST_TEXT, "", parent );
    m->appendGroup( G_EDIT_LIST,       M_EDIT_LIST );
    m->appendGroup( G_EDIT_EDITOR,     M_EDIT );
    m->appendGroup( G_EDIT_ADVANCED,   M_EDIT );
    m->appendGroup( G_EDIT_FIND,       M_EDIT );
    m->appendGroup( G_EDIT_OTHER,      M_EDIT );
}

/** \brief Create a default menu's structure for Format menu. No actions are inserted. */
void tkActionManager::createDefaultFormatMenu(QWidget *parent, const QString &toMenuOrMenuBar)
{
    if (d->menuExists( M_FORMAT ) )
        return;
    if (!d->menuBarExists( MENUBAR ))
        createMenuBar( MENUBAR, qApp->activeWindow() );
    QString menuParent = toMenuOrMenuBar;
    if (toMenuOrMenuBar.isEmpty()) {
        if (!d->menuBarExists( MENUBAR ))
            createMenuBar( MENUBAR, qApp->activeWindow() );
        menuParent = MENUBAR;
    } else {
        if ((!d->menuExists(toMenuOrMenuBar)) && (!d->menuBarExists(toMenuOrMenuBar))) {
            tkLog::addError( this, tr("%1 default menu can not be created into %2, because %2 does not exists.")
                             .arg("Edit").arg(toMenuOrMenuBar) );
            return;
        }
    }
    tkActionManager *m = this;
    m->createMenu( M_FORMAT  , menuParent, M_FORMAT_TEXT, "", parent );
    m->appendGroup( G_FORMAT_FONT,      M_FORMAT );
    m->appendGroup( G_FORMAT_PARAGRAPH, M_FORMAT );
    m->appendGroup( G_FORMAT_TABLE,     M_FORMAT );
    m->appendGroup( G_FORMAT_IMAGE,     M_FORMAT );
    m->appendGroup( G_FORMAT_OTHER,     M_FORMAT );
}

/** \brief Create a default menu's structure for Configuration menu. No actions are inserted. */
void tkActionManager::createDefaultConfigurationMenu(QWidget * parent)
{
    if (!d->menuBarExists( MENUBAR ))
        createMenuBar( MENUBAR , qApp->activeWindow() );
    if (d->menuExists( M_CONFIGURATION ) )
        return;
    tkActionManager *m = this;
    m->createMenu( M_CONFIGURATION  ,  MENUBAR, M_CONFIGURATION_TEXT, "", parent );
    m->appendGroup( G_CONFIGURATION,   M_CONFIGURATION );
    m->createMenu( M_TOOLBARS,         M_CONFIGURATION, M_TOOLBARS_TEXT, "", parent );
    m->createMenu( M_LANGUAGES,        M_CONFIGURATION, M_LANGUAGES_TEXT, "", parent );
    m->appendGroup( G_TOOLBARS,        M_TOOLBARS );
    m->appendGroup( G_LANGUAGES,       M_LANGUAGES );
    m->appendGroup( G_PREFERENCES,     M_CONFIGURATION );
}

/** \brief Create a default menu's structure for About menu. No actions are inserted. */
void tkActionManager::createDefaultAboutMenu(QWidget * parent)
{
    if (!d->menuBarExists( MENUBAR ))
        createMenuBar( MENUBAR , qApp->activeWindow() );
    if (d->menuExists( M_HELP ) )
        return;
    tkActionManager *m = this;
    m->createMenu( M_HELP  , MENUBAR, M_ABOUT_TEXT, "", parent );
    m->appendGroup( G_HELP_HELP,       M_HELP );
    m->appendGroup( G_HELP_ABOUT,      M_HELP );
    m->appendGroup( G_HELP_DEBUG,      M_HELP );
}

/** \brief Create the default actions for menus/groups : File, Edit, Format, About */
void tkActionManager::createDefaultActions( QObject *parent )
{
    createDefaultFileMenuActions(parent);
    createDefaultEditMenuActions(parent);
    createDefaultFormatMenuActions(parent);
    createDefaultAboutMenuActions(parent);
}

/** \brief Create the default actions for menu File */
void tkActionManager::createDefaultFileMenuActions( QObject *parent )
{
    Q_ASSERT_X(d->menuExists( M_FILE ), "tkActionManager::createDefaultFileMenuActions","menu file does not exists");
    tkActionManager *m = this;
    QAction *a = 0;
    a = m->createAction(   A_FILE_NEW,   G_FILE_NEW , parent);
    m->setActionDatas( A_FILE_NEW,   FILENEW_TEXT, FILENEW_TEXT, ICONFILENEW );
    a = m->createAction(   A_FILE_OPEN,   G_FILE_OPEN , parent);
    m->setActionDatas( A_FILE_OPEN,   FILEOPEN_TEXT, FILEOPEN_TEXT, ICONOPEN );
    a->setShortcut( QKeySequence::Open );
    a = m->createAction(   A_FILE_SAVE,   G_FILE_SAVE, parent );
    m->setActionDatas( A_FILE_SAVE,   FILESAVE_TEXT, FILESAVE_TEXT, ICONSAVE );
    a->setShortcut( QKeySequence::Save );
    a = m->createAction(   A_FILE_SAVEAS, G_FILE_SAVE, parent );
    m->setActionDatas( A_FILE_SAVEAS, FILESAVEAS_TEXT, FILESAVEAS_TEXT, ICONSAVEAS );
    a->setShortcut( QKeySequence::SaveAs );
    a = m->createAction(   A_FILE_PRINT,  G_FILE_PRINT, parent );
    m->setActionDatas( A_FILE_PRINT,  FILEPRINT_TEXT, FILEPRINT_TEXT, ICONPRINT );
    a->setShortcut( QKeySequence::Print );
    a = m->createAction(   A_FILE_EXIT,   G_FILE_EXIT, parent );
    m->setActionDatas( A_FILE_EXIT,   FILEEXIT_TEXT, FILEEXIT_TEXT, ICONEXIT );
    a->setShortcut( QKeySequence::Close );
    a->setMenuRole( QAction::QuitRole );
}

/** \brief Create the default actions for menu Edit */
void tkActionManager::createDefaultEditMenuActions( QObject *parent )
{
    Q_ASSERT_X(d->menuExists( M_EDIT ), "tkActionManager::createDefaultEditMenuActions","menu edit does not exists");
    tkActionManager *m = this;
    QAction *a = 0;
    a = m->createAction( A_EDIT_COPY,    G_EDIT_COPYPASTE, parent );
    m->setActionDatas( A_EDIT_COPY,  EDITCOPY_TEXT, EDITCOPY_TEXT, ICONCOPY );
    a->setShortcut( QKeySequence::Copy );
    a = m->createAction( A_EDIT_CUT,     G_EDIT_COPYPASTE, parent );
    m->setActionDatas( A_EDIT_CUT,   EDITCUT_TEXT, EDITCUT_TEXT, ICONCUT );
    a->setShortcut( QKeySequence::Cut );
    a = m->createAction( A_EDIT_PASTE,   G_EDIT_COPYPASTE, parent );
    m->setActionDatas( A_EDIT_PASTE, EDITPASTE_TEXT, EDITPASTE_TEXT, ICONPASTE );
    a->setShortcut( QKeySequence::Paste );

    a = m->createAction( A_EDIT_UNDO,    G_EDIT_UNDOREDO, parent );
    m->setActionDatas( A_EDIT_UNDO,  EDITUNDO_TEXT, EDITUNDO_TEXT, ICONUNDO );
    a->setShortcut( QKeySequence::Undo );
    a = m->createAction( A_EDIT_REDO,    G_EDIT_UNDOREDO, parent );
    m->setActionDatas( A_EDIT_REDO,  EDITREDO_TEXT, EDITREDO_TEXT, ICONREDO );
    a->setShortcut( QKeySequence::Redo );

    a = m->createAction( A_EDIT_SELECTALL,    G_EDIT_SELECT, parent );
    m->setActionDatas( A_EDIT_SELECTALL,  EDITSELECTALL_TEXT, EDITSELECTALL_TEXT, ICONSELECTALL );
    a->setShortcut( QKeySequence::SelectAll );
    a = m->createAction( A_EDIT_DESELECT,     G_EDIT_SELECT, parent );
    m->setActionDatas( A_EDIT_DESELECT,   EDITDESESLECT_TEXT, EDITDESESLECT_TEXT, ICONDESELECT );

    a = m->createAction( A_LIST_ADD,         G_EDIT_LIST, parent );
    m->setActionDatas( A_LIST_ADD,       LISTADD_TEXT, LISTADD_TEXT, ICONADD );
    a = m->createAction( A_LIST_REMOVE,      G_EDIT_LIST, parent );
    m->setActionDatas( A_LIST_REMOVE,    LISTREMOVE_TEXT, LISTREMOVE_TEXT, ICONREMOVE );
    a = m->createAction( A_LIST_MOVEUP,      G_EDIT_LIST, parent );
    m->setActionDatas( A_LIST_MOVEUP,    LISTMOVEUP_TEXT, LISTMOVEUP_TEXT, ICONMOVEUP );
    a = m->createAction( A_LIST_MOVEDOWN,    G_EDIT_LIST, parent );
    m->setActionDatas( A_LIST_MOVEDOWN,  LISTMOVEDOWN_TEXT, LISTMOVEDOWN_TEXT, ICONMOVEDOWN );
}

/** \brief Create the default actions for menu Format */
void tkActionManager::createDefaultFormatMenuActions( QObject *parent )
{
    Q_ASSERT_X(d->menuExists( M_FORMAT ), "tkActionManager::createDefaultFormatMenuActions","menu format does not exists");
    tkActionManager *m = this;
    QAction *a = 0;
    a = m->createAction( A_FORMAT_BOLD,        G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_BOLD,      FORMATBOLD_TEXT, FORMATBOLD_TEXT, ICONBOLD );
    a->setShortcut( QKeySequence::Bold );
    QFont bold;
    bold.setBold( true );
    a->setFont( bold );
    a->setCheckable( true );

    a = m->createAction( A_FORMAT_ITALIC,      G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_ITALIC,    FORMATITALIC_TEXT, FORMATITALIC_TEXT, ICONITALIC );
    a->setShortcut( QKeySequence::Italic );
    QFont italic;
    italic.setItalic( true );
    a->setFont( italic );
    a->setCheckable( true );

    a = m->createAction( A_FORMAT_UNDERLINE,   G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_UNDERLINE, FORMATUNDERLINE_TEXT, FORMATUNDERLINE_TEXT, ICONUNDERLINE );
    a->setShortcut( QKeySequence::Underline );
    QFont underline;
    underline.setUnderline( true );
    a->setFont( underline );
    a->setCheckable( true );

    a = m->createAction( A_FORMAT_STRIKE,      G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_STRIKE,    FORMATSTRIKE_TEXT, FORMATSTRIKE_TEXT, ICONSTRIKE );
    QFont strike;
    strike.setStrikeOut( true );
    a->setFont( strike );
    a->setCheckable( true );

    a = m->createAction( A_FORMAT_BIGGER,      G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_BIGGER,    FORMATBIGGER_TEXT, FORMATBIGGER_TEXT, ICONFONTBIGGER );
    a = m->createAction( A_FORMAT_SMALLER,      G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_SMALLER,    FORMATSMALLER_TEXT, FORMATSMALLER_TEXT, ICONFONTSMALLER);

    a = m->createAction( A_FORMAT_FONT,      G_FORMAT_FONT, parent );
    m->setActionDatas( A_FORMAT_FONT,    FORMATFONT_TEXT, FORMATFONT_TEXT, ICONFONTFORMAT );

    a = m->createAction( A_ALIGN_CENTER,    G_FORMAT_PARAGRAPH, parent );
    m->setActionDatas( A_ALIGN_CENTER,  ALIGNCENTER_TEXT, ALIGNCENTER_TEXT, ICONALIGNCENTER );
//    a->setShortcut( QKeySequence:: );
    a = m->createAction( A_ALIGN_LEFT,      G_FORMAT_PARAGRAPH, parent );
    m->setActionDatas( A_ALIGN_LEFT,    ALIGNLEFT_TEXT, ALIGNLEFT_TEXT, ICONALIGNLEFT );
//    a->setShortcut( QKeySequence::Print );
    a = m->createAction( A_ALIGN_RIGHT,     G_FORMAT_PARAGRAPH, parent );
    m->setActionDatas( A_ALIGN_RIGHT,   ALIGNRIGHT_TEXT, ALIGNRIGHT_TEXT, ICONALIGNRIGHT );
//    a->setShortcut( QKeySequence::Print );
    a = m->createAction( A_ALIGN_JUSTIFY,   G_FORMAT_PARAGRAPH, parent );
    m->setActionDatas( A_ALIGN_JUSTIFY, ALIGNJUSTIFY_TEXT, ALIGNJUSTIFY_TEXT, ICONALIGNJUSTIFY );
//    a->setShortcut( QKeySequence::Print );

    a = m->createAction( A_FORMAT_TABLE,    G_FORMAT_TABLE, parent );
    m->setActionDatas( A_FORMAT_TABLE,  FORMATTABLE_TEXT, FORMATTABLE_TEXT, ICONTABLE );
}

/** \brief Create the default actions for menu Configuration */
void tkActionManager::createDefaultConfigurationMenuActions( QObject *parent )
{
    Q_ASSERT_X(d->menuExists( M_CONFIGURATION ), "tkActionManager::createDefaultAboutMenuActions","menu about does not exists");
    tkActionManager *m = this;
    QAction *a = 0;
    a = m->createAction( A_PREFERENCES, G_CONFIGURATION, parent );
    a->setMenuRole( QAction::PreferencesRole );
    m->setActionDatas( A_PREFERENCES,  PREFERENCES_TEXT, PREFERENCES_TEXT, ICONPREFERENCES );
//    a->setShortcut( QKeySequence:: );
}

/** \brief Create the default actions for menu About */
void tkActionManager::createDefaultAboutMenuActions( QObject *parent )
{
    Q_ASSERT_X(d->menuExists( M_HELP ), "tkActionManager::createDefaultAboutMenuActions","menu about does not exists");
    tkActionManager *m = this;
    QAction *a = 0;
    a = m->createAction( A_ABOUT,    G_HELP_ABOUT, parent );
    m->setActionDatas( A_ABOUT,  ABOUT_TEXT, ABOUT_TEXT, ICONABOUT );
//    a->setShortcut( QKeySequence::A );
    a->setMenuRole( QAction::AboutRole );
    a = m->createAction( A_APPLICATIONHELP,    G_HELP_HELP, parent );
    m->setActionDatas( A_APPLICATIONHELP,  APPLICATIONHELP_TEXT, APPLICATIONHELP_TEXT, ICONHELP );
    a->setShortcut( QKeySequence::HelpContents );
    a = m->createAction( A_DEBUGHELPER,    G_HELP_HELP, parent );
    m->setActionDatas( A_DEBUGHELPER,  DEBUGHELPER_TEXT, DEBUGHELPER_TEXT, ICONDEBUGHELPER );
    a->setShortcut( tr("Alt+Shift+D", "Shortcut keysequence for default menu About, action Debugging Tools") );
}

/** \brief Inform on translation context. All untranslated text transmitted after calling this function will
     will be translated using the actual context setted.
     If you don't use the constants for group constants_actionmanager, you can specify the translation context
     to use when translation is needed.
     Usage sample :
     \code
     tkActionManager *am = tkActionManager::instance();
     am->setTrContext( this->objectName() );
     // all translations will be searched inside the object translations for all objects created under this line
     am->setTrContext( "myContext" );
     // all translations will be searched inside the myContext translations for all objects created under this line
     am->setTrContext( tkConstants::TK_CONSTANTS_CONTEXT );
     // all translations will be searched inside the constants translations for all objects created under this line
     \endcode
*/
void tkActionManager::setTrContext( const QString &context )
{
    d->m_TrContextList << context;
    d->m_ActualContext++;
}

QMenuBar *tkActionManager::createMenuBar( const QString &menuId, QWidget *parent )
{
    Q_ASSERT_X(!menuId.isEmpty(), "tkActionManager::createMenuBar", "No menu Id defined");
    if (d->menuBarExists(menuId)) {
        tkLog::addError( this, tkTr(CAN_NOT_CREATE_1_ALREADY_EXISTS).arg(menuId) );
        return 0;
    }
    QMenuBar *mb = new QMenuBar(parent);
    mb->setObjectName(menuId);
    d->insertMenuBar( mb, menuId );
    d->mark( mb, menuId );
    connect(mb, SIGNAL(destroyed(QObject*)), this, SLOT(objectToBeDestroyed(QObject*)));
    return mb;
}

QMenu *tkActionManager::createMenu( const QString &menuId, const QString &menuParent_OR_groupParent, const QString &unTrText, const QString &iconPath, QWidget *parentWidget )
{
    Q_ASSERT_X(!menuId.isEmpty(), "tkActionManager::createMenu", "No menu Id defined");
    if (d->menuExists(menuId)) {
        tkLog::addError( this, tkTr(CAN_NOT_CREATE_1_ALREADY_EXISTS).arg(menuId) );
        return 0;
    }
    const QString &parent = menuParent_OR_groupParent;
    if ((!d->groupExists(parent)) && (!d->menuExists(parent)) && (!d->menuBarExists(parent))) {
        tkLog::addError( this, tkTr(CAN_NOT_CREATE_1_NOPARENT).arg(menuId, parent) );
        return 0;
    }
    // create menu
    QMenu *m = new QMenu( parentWidget );
    d->setTextProperties(m, unTrText, "", d->m_ActualContext );
    d->setIconProperties(m, iconPath);
    d->retranslateObject(m);
    m->setObjectName(menuId);
    // store menu into private datas
    if (d->menuBarExists(parent))
        d->insertMenuToMenuBar(m,menuId,parent);
    else if (d->menuExists(parent)) {
        d->insertMenuToMenu(m,menuId,parent);
    }
    else if (d->groupExists(parent)) {
        d->insertMenuToGroup(m,menuId,parent);
    }
    else {
        tkLog::addError( this, tkTr(CAN_NOT_CREATE_1_NOPARENT).arg(menuId));
        delete m; m=0;
        return 0;
    }
    d->mark( m, menuId );
    connect(m, SIGNAL(destroyed(QObject*)), this, SLOT(objectToBeDestroyed(QObject*)));
    return m;
}

QMenu *tkActionManager::createPopMenu( const QString &menuId, const QString &unTrText, const QString &iconPath, QWidget *parentWidget )
{
    Q_ASSERT_X(!menuId.isEmpty(), "tkActionManager::createMenu", "No menu Id defined");
    if (d->menuExists(menuId)) {
        tkLog::addError( this, tkTr(CAN_NOT_CREATE_1_ALREADY_EXISTS).arg(menuId) );
        return 0;
    }
     // create menu
    QMenu *m = new QMenu( parentWidget );
    d->setTextProperties(m, unTrText, "", d->m_ActualContext );
    d->setIconProperties(m, iconPath);
    d->retranslateObject(m);
    m->setObjectName(menuId);
    // store menu into private datas
    d->insertPopUpMenu(m,menuId);
    d->mark( m, menuId );
    connect(m, SIGNAL(destroyed(QObject*)), this, SLOT(objectToBeDestroyed(QObject*)));
    return m;
}

/** \brief Add a group \e groupId to the menu id \e menuId. Returns the error's state. */
bool tkActionManager::appendGroup( const QString &groupId, const QString &menuId )
{
    if (d->groupExists(groupId))
        return true;
    else if (!d->menuExists(menuId))
        tkLog::addError( this, tkTr(CAN_NOT_CREATE_1_NOPARENT).arg(groupId) );
    else {
        d->insertGroupToMenu(groupId,menuId);
        return true;
    }
    return false;
}

QMenu *tkActionManager::menu( const QString &menuId )
{
    if (!d->menuExists(menuId))
        return new QMenu();
    return d->menu(menuId);
}

QMenuBar *tkActionManager::menubar( const QString &menuId )
{
    Q_ASSERT_X(!menuId.isEmpty(), "tkActionManager::menubar", "No id");
    if (!d->menuBarExists(menuId)) {
        tkLog::addError( this, tkTr(CAN_NOT_RETURN_1_DOESNOT_EXISTS).arg(menuId) );
        return 0;
    }
    return d->menuBar(menuId);
}

void tkActionManager::refreshMenuBar( const QString &menuId )
{
    Q_ASSERT_X(!menuId.isEmpty(), "tkActionManager::menubar", "No id");
    if (d->menuBarExists(menuId))
        d->refreshMenuBar(menuId);
}

void tkActionManager::refreshMenu( const QString &menuId )
{
    Q_ASSERT_X(!menuId.isEmpty(), "tkActionManager::menubar", "No id");
    if (d->menuExists(menuId))
        d->refreshMenu(menuId);
}

void tkActionManager::refreshAll()
{
    d->refreshAll();
}

QAction *tkActionManager::action( const QString &id )
{
    Q_ASSERT_X(!id.isEmpty(), "tkActionManager::action", "No id");
    if (!d->actionExists(id)) {
        tkLog::addError( this, tkTr(CAN_NOT_RETURN_1_DOESNOT_EXISTS).arg(id) );
        return 0;
    }
    return d->action(id);
}

bool tkActionManager::actionExists( const QString &id )
{
    return d->actionExists(id);
}

bool tkActionManager::actionsExists( const QStringList &list )
{
    foreach( const QString &id, list )
        if (!d->actionExists(id))
            return false;
    return true;
}

bool tkActionManager::menuExists( const QString &id )
{
    return d->menuExists(id);
}

bool tkActionManager::addAction( QAction *action, const QString &actionId, const QString &toGroup )
{
    Q_ASSERT_X(action, "tkActionManager::addAction", "No action");
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::addAction", "No action Id defined");
    if (!d->groupExists(toGroup)) {
        tkLog::addError( this, tr("Can not add Action %1 : Group does not exist %2").arg(actionId,toGroup) );
        return false;
    }
    // add action to private datas
    d->insertActionToGroup( action, actionId, toGroup );
    // connect to destroyed()
    d->mark( action, actionId );
    connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(objectToBeDestroyed(QObject*)));
    return true;
}

QAction *tkActionManager::createAction( const QString &actionId, const QString &toGroup, QObject *parent )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::createAction", "No action Id defined");
    if (!d->groupExists(toGroup)) {
        tkLog::addError( this, tr("Can not add Action %1 : Group does not exist %2").arg(actionId,toGroup) );
        return 0;
    }
    if (d->actionExists(actionId)) {
        tkLog::addMessage( this, tr("Can not add Action %1 : Action Id already used").arg(actionId) );
        return d->action(actionId);
    }
    // create action
    QAction *a = 0;
    if (!parent)
        a = new QAction(this);
    else
        a = new QAction(parent);
    a->setObjectName(actionId);
    // store action into private datas
    d->insertActionToGroup( a, actionId, toGroup );
    d->mark( a, actionId );
    connect(a, SIGNAL(destroyed(QObject*)), this, SLOT(objectToBeDestroyed(QObject*)));
    return a;
}

void tkActionManager::setActionDatas( const QString &actionId, const QString &unTrText,
                                      const QString &unTrToolTip, const QString &themedIconPath )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::setActionDatas", "No action Id defined");
    if (!d->actionExists(actionId)) {
        tkLog::addError( this, tr("Can not set Text to Action %1 : Action does not exist").arg(actionId) );
    } else {
        QAction * a = d->action(actionId);
        d->setTextProperties( a, unTrText, unTrToolTip, d->m_ActualContext );
        d->setIconProperties( a, themedIconPath );
        d->retranslateObject(a);
    }
}

void tkActionManager::setActionText( const QString &actionId, const QString& unTrText )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::setActionText", "No action Id defined");
    if (!d->actionExists(actionId)) {
        tkLog::addError( this, tr("Can not set Text to Action %1 : Action does not exist").arg(actionId) );
    } else {
        //        tkActionManagerPrivate::ItemDatas *it = d->getActionDatas(actionId);
        //        it->action->setText( tr( unTrText.toAscii() ) );
        //        it->unTrText = unTrText;
        QAction * a = d->action(actionId);
        d->setTextProperties( a, unTrText, "", d->m_ActualContext );
        d->retranslateObject(a);
    }
}

void tkActionManager::setActionToolTip( const QString &actionId, const QString &unTrToolTip )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::setActionToolTip", "No action Id defined");
    if (!d->actionExists(actionId)) {
        tkLog::addError( this, tr("Can not set ToolTip to Action %1 : Action does not exist").arg(actionId) );
    } else {
        //        tkActionManagerPrivate::ItemDatas *it = d->getActionDatas(actionId);
        //        it->action->setToolTip( tr( unTrToolTip.toAscii() ) );
        //        it->unTrToolTip = unTrToolTip;
        QAction * a = d->action(actionId);
        d->setTextProperties( a, "", unTrToolTip, d->m_ActualContext );
        d->retranslateObject(a);
    }
}

void tkActionManager::setActionThemedIcon( const QString &actionId, const QString &iconPath )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::setActionThemedIcon", "No action Id defined");
    if (!d->actionExists(actionId)) {
        tkLog::addError( this, tr("Can not set Icon to Action %1 : Action does not exist").arg(actionId) );
    } else {
        //        tkActionManagerPrivate::ItemDatas *it = d->getActionDatas(actionId);
        //        it->action->setIcon( tkTheme::icon( iconPath ) );
        //        it->themedIconPath = iconPath;
        QAction * a = d->action(actionId);
        d->setIconProperties( a, iconPath );
    }
}

void tkActionManager::setActionUnthemedIcon( const QString &actionId, const QString &iconPath )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::setActionUnthemedIcon", "No action Id defined");
    if (!d->actionExists(actionId)) {
        tkLog::addError( this, tr("Can not set Icon to Action %1 : Action does not exist").arg(actionId) );
    } else {
        //        tkActionManagerPrivate::ItemDatas *it = d->getActionDatas(actionId);
        //        it->action->setIcon( QIcon( iconPath ) );
        //        it->unthemedIconPath = iconPath;
        QAction * a = d->action(actionId);
        d->setIconProperties( a, "", iconPath );
    }
}

void tkActionManager::setActionEnabled( const QString &actionId, bool state )
{
    Q_ASSERT_X(!actionId.isEmpty(), "tkActionManager::setActionUnthemedIcon", "No action Id defined");
    if (!d->actionExists(actionId)) {
        tkLog::addError( this, tr("Can not set EnabledState to Action %1 : Action does not exist").arg(actionId) );
    } else {
        //        tkActionManagerPrivate::ItemDatas *it = d->getActionDatas(actionId);
        //        it->action->setEnabled( state );
        d->action(actionId)->setEnabled(state);
    }
}

void tkActionManager::objectToBeDestroyed( QObject *obj )
{
    QString mark = d->getFullId(obj);
    if (!obj) {
        tkLog::addError( this, tr("POTENTIAL FATAL ERROR : An error occured into tkActionManager::objectToBeDestroyed : object passed does not exists") );
        return;
    }
    if (mark.startsWith(MENUBARPREFIX)) {
        d->removeMenuBar(mark);
    } else if (mark.startsWith(MENUPREFIX)) {
        d->removeMenu(mark);
    } else if (mark.startsWith(ACTIONPREFIX)) {
        d->removeAction(mark);
    }
}

void tkActionManager::retranslate()
{
    d->retranslate();
}
