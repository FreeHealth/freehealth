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

enum {WarnLogChronos=true};

static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

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
        if (_stack)
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

        QList<Form::FormMain *> forms;
        forms << _formMain;
        forms << _formMain->flattenFormMainChildren();

        foreach(FormMain *form, forms) {
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
//                form->formWidget()->setEnabled(false);
            }
        }
    }

    void useEpisodeModel(Form::FormMain *rootForm)
    {
        // EpisodeModel from the FormManager must not be deleted
        if (_episodeModel) {
            _episodeModel = 0;
        }
        _episodeModel = formManager()->episodeModel(rootForm);
    }

    QString createXmlEpisode(const QString &formUid)
    {
        // TODO: code here : use a QDomDocument
        FormMain *form = _formMain->formMainChild(formUid);
        if (!form)
            return QString::null;
//        bool formIsModified = false;

        QHash<QString, FormItem *> items;
        foreach(FormItem *it, form->flattenFormItemChildren()) {
            // TODO: check nested items?
            if (it->itemData()) {
//                if (it->itemDatas()->isModified()) {
//                    qWarning() << it->uuid() << "is modified";
//                    formIsModified = true;
//                }
                items.insert(it->uuid(), it);
            }
        }

//        if (formIsModified) {
            // create the XML episode file
            QHash<QString, QString> xmlData;
            foreach(FormItem *it, items) {
                xmlData.insert(it->uuid(), it->itemData()->storableData().toString());
            }
            return Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, xmlData, 2, false);
//        }

//        return QString();
    }

//    void feedFormWithEpisodeContent(Form::FormMain *form, EpisodeData *episode, bool feedPatientModel = false)
    void feedFormWithEpisodeContent(const QModelIndex &index, bool feedPatientModel = false)
    {
        qWarning() << "feedForm" << _formMain << _stackId_FormUuid << _stack->currentIndex();
        if (!_episodeModel) {
            if (_formMain)
                LOG_ERROR_FOR(q, "No episode model. FormUid: " + _formMain->uuid());
            else
                LOG_ERROR_FOR(q, "No episode model. FormUid: (0x0)");
            return;
        }

        // show the form widgets
        int stackIndex;
        (_formMain) ? stackIndex = _stackId_FormUuid.key(_formMain->uuid()) : stackIndex = 0;
        _stack->setCurrentIndex(stackIndex);

        QTime chrono;
        if (WarnLogChronos)
            chrono.start();

        _formMain->clear();
        q->setEnabled(false);

        QModelIndex xmlIndex = _episodeModel->index(index.row(), EpisodeModel::XmlContent);
        const QString &xml = _episodeModel->data(xmlIndex).toString();

//        qWarning() << xml;

        if (xml.isEmpty()) {
            return;
        }

        // read the xml'd content
        QHash<QString, QString> datas;
        if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
            QModelIndex uid = _episodeModel->index(index.row(), EpisodeModel::Uuid);
            LOG_ERROR_FOR(q, QString("Error while reading episode content (%1)").arg(_episodeModel->data(uid).toString()));
            return;
        }

        // put datas into the FormItems of the form
        // XML content ==
        // <formitemuid>value</formitemuid>
        QHash<QString, FormItem *> items;
        foreach(FormItem *it, _formMain->flattenFormItemChildren()) {
            items.insert(it->uuid(), it);
        }

        // Populate the FormMain item data (username, userdate, label)
        QModelIndex userName = _episodeModel->index(index.row(), EpisodeModel::UserCreatorName);
        QModelIndex userDate = _episodeModel->index(index.row(), EpisodeModel::UserDate);
        QModelIndex label = _episodeModel->index(index.row(), EpisodeModel::Label);
        _formMain->itemData()->setData(IFormItemData::ID_EpisodeDate, _episodeModel->data(userDate));
        _formMain->itemData()->setData(IFormItemData::ID_EpisodeLabel, _episodeModel->data(label));
        _formMain->itemData()->setData(IFormItemData::ID_UserName, _episodeModel->data(userName));

        // Populate FormItem data and the patientmodel
        foreach(FormItem *it, items.values()) {
            if (!it) {
                qWarning() << "FormManager::activateForm :: ERROR: no item: " << items.key(it);
                continue;
            }
            if (!it->itemData())
                continue;

            it->itemData()->setStorableData(datas.value(it->uuid()));
            if (feedPatientModel && it->patientDataRepresentation() >= 0)
                    patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));
        }

        q->setEnabled(true);
        if (WarnLogChronos)
            Utils::Log::logTimeElapsed(chrono, q->objectName(), "feedFormWithEpisodeContent");
    }

public:
    QStackedLayout *_stack;
    QHash<int, QString>_stackId_FormUuid;
    Form::FormMain *_formMain;
    EpisodeModel *_episodeModel;

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

bool FormDataWidgetMapper::isDirty() const
{
    if (!d->_formMain)
        return false;
    // ask all current form item data
    foreach(FormItem *it, d->_formMain->flattenFormItemChildren()) {
        if (it->itemData() && it->itemData()->isModified()) {
            return true;
        }
    }
    return false;
}

void FormDataWidgetMapper::setCurrentForm(const QString &formUid)
{
    setCurrentForm(formManager()->form(formUid));
}

void FormDataWidgetMapper::setCurrentForm(Form::FormMain *form)
{
    d->clearStackLayout();
    if (!form)
        return;
    qWarning() << "FormDataWidgetMapper::setCurrentForm" << form->uuid();
    d->populateStack(form);
    d->useEpisodeModel(form);
}

void FormDataWidgetMapper::setCurrentEpisode(const QVariant &uid)
{
}

void FormDataWidgetMapper::setCurrentEpisode(const QModelIndex &index)
{
    qWarning() << "FormDataWidgetMapper::setCurrentEpisode" << index.data().toString();

    // populate the form item data
    d->feedFormWithEpisodeContent(index, true);
}

bool FormDataWidgetMapper::submit()
{
    return true;
}

