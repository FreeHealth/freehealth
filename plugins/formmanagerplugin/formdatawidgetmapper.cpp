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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Form::Internal::FormDataWidgetMapper
 * This mapper acts on forms (Form::FormMain) like a QDataWidgetMapper.\n
 * This mapper manages the UI's creation (according to the current form) and the
 * mapping ui <-> FormItemData.\n
 * It is connected to the central Form::FormTreeModel and the Form::EpisodeModel accessible from
 * the form manager.
 * \sa Form::FormManager, Form::FormManager::formTreeModel(), Form::FormManager::episodeModel()
 */

#include "formdatawidgetmapper.h"
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/constants_db.h>

#include <utils/log.h>

#include <QScrollArea>
#include <QStackedLayout>
#include <QTextBrowser>

using namespace Form;
using namespace Internal;

static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }

namespace Form {
namespace Internal {
class FormDataWidgetMapperPrivate
{
public:
    FormDataWidgetMapperPrivate(FormDataWidgetMapper *parent) :
        _stack(0),
        _formMain(0),
        q(parent)
    {
    }

    ~FormDataWidgetMapperPrivate()
    {}

    void clearStackLayout()
    {
        delete _stack;
        _stack = new QStackedLayout(q);
        q->setLayout(_stack);
    }

    void populateStack(Form::FormMain *rootForm)
    {
        Q_ASSERT(_stack);
        if (!_stack)
            return;

        clearStackLayout();

        _formMain = rootForm;

        // Add the synthesis form
        QScrollArea *sa = new QScrollArea(q);
        sa->setWidgetResizable(true);
        QWidget *w = new QWidget(sa);
//        QPalette palette = w->palette();
//        QString gender = patient()->data(Core::IPatient::Gender).toString();
//        if (gender=="M")
//            palette.setColor(QPalette::Background, Patients::Constants::maleColor.lighter(140));
//        else if (gender=="F")
//            palette.setColor(QPalette::Background, Patients::Constants::femaleColor.lighter(140));
//        else
//            palette.setColor(QPalette::Background, Patients::Constants::hermaColor.lighter());
//        w->setPalette(palette);
        sa->setWidget(w);
        QVBoxLayout *vl = new QVBoxLayout(w);
        vl->setSpacing(0);
        vl->setMargin(0);
        QTextBrowser *t = new QTextBrowser(w);
        t->setReadOnly(true);
        t->setEnabled(true);
        vl->addWidget(t);
        int id = _stack->addWidget(sa);
        _stackId_FormUuid.insert(id, Constants::PATIENTLASTEPISODES_UUID);

        // add all form's widgets
        if (!rootForm)
            return;

        foreach(FormMain *form, rootForm->flattenFormMainChildren()) {
            if (form->formWidget()) {
                QScrollArea *sa = new QScrollArea(_stack->parentWidget());
                sa->setWidgetResizable(true);
                QWidget *w = new QWidget(sa);
                sa->setWidget(w);
                QVBoxLayout *vl = new QVBoxLayout(w);
                vl->setSpacing(0);
                vl->setMargin(0);
                vl->addWidget(form->formWidget());
                int id = _stack->addWidget(sa);
                _stackId_FormUuid.insert(id, form->uuid());
                form->formWidget()->setEnabled(false);
            }
        }
    }

public:
    QStackedLayout *_stack;
    QHash<int, QString>_stackId_FormUuid;
    Form::FormMain *_formMain;

private:
    FormDataWidgetMapper *q;
};
}  // namespace Internal
}  // namespace Form

FormDataWidgetMapper::FormDataWidgetMapper(QWidget *parent) :
    QWidget(parent),
    d(new FormDataWidgetMapperPrivate(this))
{
    setObjectName("FormDataWidgetMapper");
    // Create the ui
}

FormDataWidgetMapper::~FormDataWidgetMapper()
{
}

bool FormDataWidgetMapper::initialize()
{
    return true;
}

void FormDataWidgetMapper::setCurrentForm(const QString &formUid)
{
    setCurrentForm(formManager()->form(formUid));
}

void FormDataWidgetMapper::setCurrentForm(Form::FormMain *form)
{
    if (!form)
        return;
    qWarning() << "FormDataWidgetMapper::setCurrentForm" << form->uuid();
    d->clearStackLayout();
    d->populateStack(form);
//    d->useEpisodeModel(form);
}

void FormDataWidgetMapper::setCurrentEpisode(const QVariant &uid)
{
}

void FormDataWidgetMapper::setCurrentEpisode(const QModelIndex &index)
{
}


