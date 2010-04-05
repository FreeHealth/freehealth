/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef TKACTIONMANAGER_H
#define TKACTIONMANAGER_H

#include <tkGlobal.h>
#include <tkExporter.h>
#include <tkConstants.h>
class tkActionManagerPrivate;

#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

/**
 * \file tkActionManager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.11
 * \date 16 July 2009
*/

class Q_TK_EXPORT tkActionManager : public QObject
{
    Q_OBJECT
    friend const void tkGlobal::initLib();

public:
    static tkActionManager *instance();
    ~tkActionManager();

    void createDefaultMenusAndGroups( QWidget * parent = 0 );
    void createDefaultFileMenu( QWidget * parent = 0 );
    void createDefaultEditMenu( QWidget * parent = 0, const QString &toMenuOrMenuBar = QString::null );
    void createDefaultFormatMenu( QWidget *parent = 0, const QString &toMenuOrMenuBar = QString::null );
    void createDefaultConfigurationMenu( QWidget *parent = 0 );
    void createDefaultAboutMenu( QWidget * parent = 0 );

    void createDefaultActions( QObject *parent = 0 );
    void createDefaultFileMenuActions( QObject *parent = 0 );
    void createDefaultEditMenuActions( QObject *parent = 0 );
    void createDefaultFormatMenuActions( QObject *parent = 0 );
    void createDefaultConfigurationMenuActions( QObject *parent = 0 );
    void createDefaultAboutMenuActions( QObject *parent = 0 );

    void setTrContext( const QString &context );

    // Managing menus and groups
    QMenuBar * createMenuBar( const QString &menuId, QWidget *parent );
    QMenu * createMenu( const QString &menuId, const QString &menuParent_OR_groupParent,
                        const QString &untranslatedName,
                        const QString &iconPath = QString::null, QWidget *parentWidget = 0 );
    QMenu * createPopMenu( const QString &menuId,
                           const QString &untranslatedName,
                           const QString &iconPath = QString::null, QWidget *parentWidget = 0 );
    bool appendGroup( const QString &groupId, const QString &menuId );
    void refreshMenuBar( const QString &menuId );
    void refreshMenu( const QString &menuId );
    void refreshAll();

    // Adding and create actions
    bool addAction( QAction *action, const QString &actionId, const QString &toGroup );
    QAction *createAction( const QString &actionId, const QString &toGroup, QObject *parent = 0 );

    void setActionDatas( const QString &actionId, const QString &unTrText,
                         const QString &unTrToolTip=QString::null, const QString &themedIconPath=QString::null );
    void setActionText( const QString &actionId, const QString &unTrText );         // allows automatic translation
    void setActionToolTip( const QString &actionId, const QString &unTrToolTip );   // allows automatic translation
    void setActionThemedIcon( const QString &actionId, const QString &iconPath );   // allows automatic translation
    void setActionUnthemedIcon( const QString &actionId, const QString &iconPath );
    void setActionEnabled( const QString &actionId, bool state );

    // Removing menus/menubars/actions/groups : delete pointer

    // Getters
    QMenu *menu( const QString &id );
    QMenuBar *menubar( const QString &id );
    QAction *action( const QString &id );
    bool actionExists( const QString &id );
    bool actionsExists( const QStringList &list );
    bool menuExists( const QString &id );

protected:
    tkActionManager( QObject *parent = 0 );

private Q_SLOTS:
    void objectToBeDestroyed(QObject*);
    void retranslate();

private:
    tkActionManagerPrivate *d;
    static tkActionManager *m_Instance;
};

#endif // TKACTIONMANAGER_H
