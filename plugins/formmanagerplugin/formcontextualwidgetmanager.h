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
 *   Main Developers:                                                      *
 *       Eric MAEKER <eric.maeker@gmail.com>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_INTERNAL_FORMCONTEXTUALWIDGETMANAGER_H
#define FORM_INTERNAL_FORMCONTEXTUALWIDGETMANAGER_H

#include <coreplugin/contextmanager/icontext.h>
#include <formmanagerplugin/formcontextualwidget.h>

#include <QObject>
#include <QPointer>

/**
 * \file formcontextualwidget.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 16 Sept 2012
*/

namespace Core {
class IContext;
}

namespace Form {
namespace Internal {

class FormActionHandler : public QObject
{
    Q_OBJECT
public:
    FormActionHandler(QObject *parent = 0);
    virtual ~FormActionHandler() {}
    
    void setCurrentView(FormContextualWidget *view);
    
private Q_SLOTS:
    void onClearRequested();
    void onSaveEpisodeRequested();
    void onCreateEpisodeRequested();
    void onValidateEpisodeRequested();
    void onRemoveEpisodeRequested();
    void onTakeScreenshotRequested();
    void onAddFormRequested();
    void onRemoveFormRequested();
    void onPrintFormRequested();
    void showDatabaseInformation();
    
private Q_SLOTS:
    void updateActions();
    void onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::WidgetAction action);
    
protected:
    QAction *aClear;
    QAction *aShowDatabaseInformation;
    QAction *aCreateEpisode, *aValidateEpisode, *aRemoveEpisode, *aSaveEpisode;
    QAction *aTakeScreenshot;
    QAction *aAddForm;
    QAction *aRemoveSubForm;
    QAction *aPrintForm;
    
    QPointer<FormContextualWidget> m_CurrentView;
};

class FormContextualWidgetManager : public FormActionHandler
{
    Q_OBJECT

public:
    explicit FormContextualWidgetManager(QObject *parent = 0);
    ~FormContextualWidgetManager();

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);

private:
    QPointer<Core::IContext> _contextObject;
};

} // namespace Internal
} // namespace Form

#endif // FORM_INTERNAL_FORMCONTEXTUALWIDGETMANAGER_H

