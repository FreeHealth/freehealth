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
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_INTERNAL_FORMCONTEXTUALWIDGET_H
#define FORM_INTERNAL_FORMCONTEXTUALWIDGET_H

#include <coreplugin/contextmanager/icontext.h>
#include <QWidget>
#include <QList>

/**
 * \file formcontextualwidget.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 16 Sept 2012
*/

namespace Form {
namespace Internal {
class FormContext;
class FormActionHandler;
class FormContextualWidgetManager;

class FormContextualWidget : public QWidget
{
    Q_OBJECT
    friend class Form::Internal::FormActionHandler;
    friend class Form::Internal::FormContextualWidgetManager;

public:
    enum WidgetAction {
        Action_Clear = 0,
        Action_CreateEpisode,
        Action_ValidateCurrentEpisode,
        Action_SaveCurrentEpisode,
        Action_RemoveCurrentEpisode,
        Action_TakeScreenShot,
        Action_AddForm,
        Action_PrintCurrentFormEpisode
    };

    explicit FormContextualWidget(QWidget *parent = 0);
    ~FormContextualWidget();

    void addContexts(const QList<int> &contexts);
    QList<int> contexts() const;

protected:
    /** Return true if the \e action must be enabled, or false if it must be disabled. */
    virtual bool enabledActionState(WidgetAction action) const = 0;

protected Q_SLOTS:
    // slots connected to the formactionhandler instance
    virtual bool clear() = 0;
    virtual bool createEpisode() = 0;
    virtual bool validateCurrentEpisode() = 0;
    virtual bool saveCurrentEpisode() = 0;
    virtual bool removeCurrentEpisode() = 0;
    virtual bool takeScreenshotOfCurrentEpisode() = 0;
    virtual bool addForm() = 0;
    virtual bool printFormOrEpisode() = 0;

Q_SIGNALS:
    /** Emit this signal when you need the action handler to update one action enabled state. */
    void actionEnabledStateChanged(Form::Internal::FormContextualWidget::WidgetAction action);

    /** Emit this signal when you need the action handler to update all action enabled state. */
    void actionsEnabledStateChanged();

private:
    FormContext *m_Context;
};

class FormContext : public Core::IContext
{
    Q_OBJECT
public:
    FormContext(FormContextualWidget *w) :
        Core::IContext(w), wgt(w)
    {
        setObjectName("FormContext");
    }

    void setContext(QList<int> c) { ctx = c; }
    void addContext(QList<int> c) { ctx.append(c); }
    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }

private:
    FormContextualWidget *wgt;
    QList<int> ctx;
};

} // namespace Internal
} // namespace Form

#endif // FORM_INTERNAL_FORMCONTEXTUALWIDGET_H

