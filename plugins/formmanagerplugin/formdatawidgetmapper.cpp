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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/constants_db.h>
#include <formmanagerplugin/constants_settings.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QScrollArea>
#include <QStackedLayout>
#include <QTextBrowser>

using namespace Form;
using namespace Internal;

enum { WarnLogChronos=false, WarnDirty=true };

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}

namespace Form {
namespace Internal {
class FormDataWidgetMapperPrivate
{
public:
    FormDataWidgetMapperPrivate(FormDataWidgetMapper *parent) :
        _stack(0),
        _formMain(0),
        _episodeModel(0),
        q(parent)
    {
    }

    ~FormDataWidgetMapperPrivate()
    {}

    void clearStackLayout()
    {
        if (_stack) {
            if (_formMain) {
                QList<Form::FormMain *> forms;
                forms << _formMain;
                forms << _formMain->flattenedFormMainChildren();
                foreach(FormMain *form, forms) {
                    if (form->formWidget()) {
                        form->formWidget()->setParent(0);
                    }
                }
            }

            for(int i=0; i < _stack->count(); ++i) {
                delete _stack->widget(i);
            }
        }
    }

    void populateStack(Form::FormMain *rootForm)
    {
        WARN_FUNC;
        if (!_stack) {
            _stack = new QStackedLayout(q);
            q->setLayout(_stack);
        }
        clearStackLayout();
        _formMain = rootForm;

        // Add the synthesis form
//        QScrollArea *sa = new QScrollArea(q);
//        sa->setWidgetResizable(true);
//        QWidget *w = new QWidget(sa);
//        sa->setWidget(w);
//        QVBoxLayout *vl = new QVBoxLayout(w);
//        vl->setSpacing(0);
//        vl->setMargin(0);
//        QTextBrowser *t = new QTextBrowser(w);
//        t->setReadOnly(true);
//        t->setEnabled(true);
//        vl->addWidget(t);
//        int id = _stack->addWidget(sa);
//        _stackId_FormUuid.insert(id, Constants::PATIENTLASTEPISODES_UUID);

        // add all form's widgets
        if (!rootForm)
            return;

        QList<Form::FormMain *> forms;
        forms << _formMain;
        forms << _formMain->flattenedFormMainChildren();

        foreach(FormMain *form, forms) {
            if (form->formWidget()) {
                QScrollArea *sa = new QScrollArea(_stack->parentWidget());
                sa->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                sa->setWidgetResizable(true);
                QWidget *w = new QWidget(sa);
                sa->setWidget(w);
                w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                QVBoxLayout *vl = new QVBoxLayout(w);
                vl->setSpacing(0);
                vl->setMargin(0);
                vl->addWidget(form->formWidget());
                int id = _stack->addWidget(sa);
                _stackId_FormUuid.insert(id, form->uuid());
                qDebug() << "id: " << id << " form->uuid(): " << form->uuid();
            }
        }
    }

    void useEpisodeModel(Form::FormMain *rootForm)
    {
        // EpisodeModel from the FormManager must not be deleted
        if (_episodeModel) {
            _episodeModel = 0;
        }
        _episodeModel = episodeManager().episodeModel(rootForm);
    }

    QString getCurrentXmlEpisode()
    {
        if (!_formMain)
            return QString::null;

        QHash<QString, QString> xmlData;
        foreach(FormItem *it, _formMain->flattenedFormItemChildren()) {
            if (it->itemData()) {
                xmlData.insert(it->uuid(), it->itemData()->storableData().toString());
            }
        }
        return Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, xmlData, 2, false);
    }

    // Set the current episode into the form and populate the patientmodel too
    void setCurrentEpisode(const QModelIndex &index)
    {
        if (!_episodeModel) {
            if (_formMain)
                LOG_ERROR_FOR(q, "No episode model. FormUid: " + _formMain->uuid());
            else
                LOG_ERROR_FOR(q, "No episode model. FormUid: (0x0)");
            return;
        }
        _currentEpisode = index;

        if (!index.isValid()) {
            LOG_ERROR_FOR(q, "Invalid index when setting current episode. Episode not read.");
            return;
        }

        // show the form widgets
        int stackIndex;
        stackIndex = _formMain ? _stackId_FormUuid.key(_formMain->uuid()) : 0;
        _stack->setCurrentIndex(stackIndex);

        _episodeModel->populateFormWithEpisodeContent(index, true);
    }

public:
    QStackedLayout *_stack;
    QHash<int, QString>_stackId_FormUuid;
    Form::FormMain *_formMain;
    EpisodeModel *_episodeModel;
    QPersistentModelIndex _currentEpisode;

private:
    FormDataWidgetMapper *q;
};
}  // namespace Internal
}  // namespace Form

/** Create the widget mapper using the parent widget \e parent */
FormDataWidgetMapper::FormDataWidgetMapper(QWidget *parent) :
    QWidget(parent),
    d(new FormDataWidgetMapperPrivate(this))
{
    setObjectName("FormDataWidgetMapper");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

FormDataWidgetMapper::~FormDataWidgetMapper()
{
}

/** Initialize the datamapper. Must be called before you set the Form::FormMain. */
bool FormDataWidgetMapper::initialize()
{
    return true;
}

/** Clear the current form content but unique episode forms. */
void FormDataWidgetMapper::clear()
{
    if (!d->_formMain)
        return;
//    if (d->_formMain->isUniqueEpisode())
//        return;
    LOG("Clear");
    d->_formMain->clear();
    d->_currentEpisode = QModelIndex();
}

/**
 * Return true if the current content of the mapper is dirty.
 * Asks each Form::FormItemData of the current editing Form::FormMain for their modification state.
 * \sa Form::FormItemData::isModified
 */
bool FormDataWidgetMapper::isDirty() const
{
    if (!d->_formMain) {
        return false;
    }

/*    if (!d->_currentEpisode.isValid()) {
        qDebug() << Q_FUNC_INFO << "current episode" << d->_currentEpisode
                 << "row: " << d->_currentEpisode.row()
                 << "column: " << d->_currentEpisode.column()
                << "model: " << d->_currentEpisode.model()
                 << "not valid: " << d->_formMain->uuid()
                << "name: " << d->_formMain->spec()->label();
        return false;
    }
*/
    // form main is readonly ?
    if (d->_formMain->itemData() && d->_formMain->itemData()->isReadOnly()) {
        LOG(QString("isDirty (form) %1 isReadOnly").arg(d->_formMain->uuid()));
        return false;
    }

    // form main isModified() ?
    if (d->_formMain->itemData() && d->_formMain->itemData()->isModified()) {
        if (WarnDirty)
            LOG(QString("isDirty (form) Uuid: %1 itemData()->isModified(): %2").arg(d->_formMain->uuid()).arg(d->_formMain->itemData()->isModified()));
        return true;
    }
    // ask all current form item data
    foreach(FormItem *it, d->_formMain->flattenedFormItemChildren()) {
        if (it->itemData() && it->itemData()->isModified()) {
            if (WarnDirty) {
                LOG(QString("isDirty (item) %1 %2").arg(it->uuid()).arg(+it->itemData()->isModified()));
            }
            return true;
        }
    }
    if (WarnDirty) {
        LOG(QString("isDirty = FALSE, Form: %1").arg(d->_formMain->uuid()));
    }
    return false;
}

/** Return true if the current content of the mapper is dirty */
QModelIndex FormDataWidgetMapper::currentEditingEpisodeIndex() const
{
    return d->_currentEpisode;
}

/** Return the label of the currently editing episode */
QString FormDataWidgetMapper::currentEpisodeLabel() const
{
    if (d->_formMain && d->_formMain->itemData())
        return d->_formMain->itemData()->data(IFormItemData::ID_EpisodeLabel).toString();
    return QString::null;
}

/** Return the label of the currently editing form */
QString FormDataWidgetMapper::currentFormName() const
{
    if (d->_formMain)
        return d->_formMain->spec()->label();
    return QString::null;
}

/** Define the Form::FormMain to use in the mapper. */
void FormDataWidgetMapper::setCurrentForm(const QString &formUid)
{
    setCurrentForm(formManager().form(formUid));
}

/** Define the Form::FormMain to use in the mapper. */
void FormDataWidgetMapper::setCurrentForm(Form::FormMain *form)
{
    d->clearStackLayout();
    if (!form)
        return;
    d->populateStack(form);
    d->useEpisodeModel(form);
    if (d->_formMain->itemData())
        d->_formMain->itemData()->setModified(false);
}

/** Use the last recorded episode as current episode */
void FormDataWidgetMapper::setLastEpisodeAsCurrent()
{
    setCurrentEpisode(d->_episodeModel->index(0,0));
}

/** Define the current episode index to use in the mapper. */
void FormDataWidgetMapper::setCurrentEpisode(const QModelIndex &index)
{
    d->setCurrentEpisode(index);
}

/** Enable or disable the Form::FormMain Form::IFormWidget */
void FormDataWidgetMapper::setFormWidgetEnabled(bool enabled)
{
    d->_formMain->formWidget()->setEnabled(enabled);
}

/** Take a screenshot of the current editing form widget (populated with the episode index values). */
QPixmap FormDataWidgetMapper::screenshot()
{
    QScrollArea *area = qobject_cast<QScrollArea*>(d->_stack->currentWidget());
    QWidget *widget = area->widget();
    return QPixmap::grabWidget(widget);
}

/** Submit modifications (form header && form content) to the corresponding Form::EpisodeModel. */
bool FormDataWidgetMapper::submit()
{
    const QString &xml = d->getCurrentXmlEpisode();
    QModelIndex xmlIndex = d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::XmlContent);
    if (!d->_episodeModel->setData(xmlIndex, xml)) {
        LOG_ERROR("Unable to save the episode XML content");
        return false;
    }

    QModelIndex userName = d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::UserCreatorName);
    QModelIndex userDate = d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::UserDateTime);
    QModelIndex label = d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::Label);
    QModelIndex prior = d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::Priority);

    d->_episodeModel->setData(label, d->_formMain->itemData()->data(IFormItemData::ID_EpisodeLabel));
    d->_episodeModel->setData(userName, d->_formMain->itemData()->data(IFormItemData::ID_UserName));
    d->_episodeModel->setData(userDate, d->_formMain->itemData()->data(IFormItemData::ID_EpisodeDateTime));
    d->_episodeModel->setData(prior, d->_formMain->itemData()->data(IFormItemData::ID_Priority));

    bool ok = d->_episodeModel->submit();
    if (ok) {
        // Ensure that patientmodel was fed with the save data
        d->_episodeModel->populateFormWithEpisodeContent(d->_currentEpisode, true);
        d->_episodeModel->dataChanged(d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::Priority),
                                      d->_episodeModel->index(d->_currentEpisode.row(), EpisodeModel::Label));
    }
    return ok;
}

