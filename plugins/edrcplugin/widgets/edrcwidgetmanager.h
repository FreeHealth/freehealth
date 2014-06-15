/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                      *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_EDRC_CONTEXTUALWIDGET_MANAGER_H
#define EDRC_INTERNAL_EDRC_CONTEXTUALWIDGET_MANAGER_H

#include <coreplugin/contextmanager/icontext.h>
#include <QObject>
#include <QPointer>

/**
 * \file edrcwidgetmanager.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 28 Sept 2013
*/

namespace Core {
class IContext;
}

namespace eDRC {
namespace Internal {
class EdrcActionManager;
class EdrcContextualWidget;

class EdrcActionManager : public QObject
{
    Q_OBJECT
public:
    EdrcActionManager(QObject *parent = 0);
    virtual ~EdrcActionManager() {}
    
    void setCurrentView(EdrcContextualWidget *view);
    
private Q_SLOTS:
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSavePDF();
    void filePrint();
    void filePrintPreview();
    void editItem();
    void addItem();
    void removeItem();
    void clearItems();
    void renewItem();
    void showDatabaseInformation();
    void aboutSFMG();

private Q_SLOTS:
    void updateActions();
    
protected:
    QAction *aClear, *aFileOpen, *aFileSave, *aFileSaveAs, *aFileSavePDF, *aFilePrint, *aFilePrintPreview;
    QAction *aAddItem, *aRemoveItem, *aEditItem, *aRenewItem;
    QAction *aShowDatabaseInformation, *aAboutSFMG;
    
    QPointer<EdrcContextualWidget> m_CurrentView;
};

class EdrcWidgetManager : public EdrcActionManager
{
    Q_OBJECT
public:
    explicit EdrcWidgetManager(QObject *parent = 0);
    ~EdrcWidgetManager();
    
    EdrcContextualWidget *currentView() const;
    
private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);
};

} // namespace Internal
} // namespace eDRC

#endif // EDRC_INTERNAL_EDRC_CONTEXTUALWIDGET_MANAGER_H

