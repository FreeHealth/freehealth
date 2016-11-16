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
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *       Jerome PINGUET <jerome@jerome.cc                                  *
 ***************************************************************************/
#pragma once

#include <coreplugin/contextmanager/icontext.h>

#include <QWidget>
#include <QList>

/**
 * \file pmhcontextualwidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 12 Feb 2011
 * \internal
*/

namespace PMH {
namespace Internal {
class PmhContext;
class PmhActionHandler;
//class PmhContextualWidgetManager;
//}

class PmhContextualWidget : public QWidget
{
    Q_OBJECT
    friend class PmhActionHandler;
//    friend class Pmh::Internal::PmhContextualWidgetManager;

public:
    enum WidgetAction {
        Action_Clear = 0,
        Action_CreateEpisode,               /*!< Create new episode */
        Action_ValidateCurrentEpisode,      /*!< "Sign" currently selected episode, so that it can't be changed any more */
        Action_SaveCurrentEpisode,          /*!< Save currently selected episode to database */
        Action_RenewCurrentEpisode,         /*!< Renew the currently selected episode */
        Action_RemoveCurrentEpisode,        /*!< Remove currently selected episode */
        Action_TakeScreenShot,              /*!< Take a screenshot of the current form */
        //Action_AddForm,                     /*!< Add Form to treeview */
        //Action_RemoveSub,                   /*!< Remove Form and all subforms from treeview */
        Action_PrintCurrentFormEpisode      /*!< Print currently selected episode */
    };

    /*explicit*/ PmhContextualWidget(QWidget *parent = 0);
    virtual ~PmhContextualWidget();

    Core::IContext *context() const;

    /** Return true if the \e action must be enabled, or false if it must be disabled. */
    virtual bool enableAction(WidgetAction action) const = 0;

protected Q_SLOTS:
    // slots connected to the formactionhandler instance
    virtual bool clear() = 0;
    virtual bool createEpisode() = 0;
    virtual bool validateCurrentEpisode() = 0;
    virtual bool renewEpisode() = 0;
    virtual bool saveCurrentEpisode() = 0;
    virtual bool removeCurrentEpisode() = 0;
    virtual bool takeScreenshotOfCurrentEpisode() = 0;
    virtual bool printFormOrEpisode() = 0;

Q_SIGNALS:
    /** Emit this signal when you need the action handler to update one action enabled state. */
    void actionEnabledStateChanged(PMH::Internal::PmhContextualWidget::WidgetAction action);

    /** Emit this signal when you need the action handler to update all action enabled state. */
    void actionsEnabledStateChanged();

private:
    PmhContext *m_Context;
};

class PmhContext : public Core::IContext
{
public:
    PmhContext(PmhContextualWidget *w) : Core::IContext(w)
    {
        setObjectName("PmhContext");
        setWidget(w);
    }
};

}  // End namespace Internal
}  // End namespace PMH
