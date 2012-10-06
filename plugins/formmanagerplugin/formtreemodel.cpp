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
 * \class Form::FormTreeModel
 * Model for forms. Get your model from Form::FormManager::formTreeModel()
 */

#include "formtreemodel.h"
#include "iformitem.h"
#include "episodebase.h"
#include "formmanager.h"
#include "formcore.h"
#include "formcollection.h"
#include "subforminsertionpoint.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QDebug>

enum {WarnFormRetreiving=true};

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

namespace Form {
namespace Internal {
class FormTreeModelPrivate
{
public:
    FormTreeModelPrivate(FormTreeModel *parent) :
        q(parent)
    {
    }

    void linkFormAndItem(Form::FormMain *form, QStandardItem *item)
    {
        _formToItem.insert(item, form);
    }

    Form::FormMain *itemToForm(QStandardItem *item)
    {
        return _formToItem.value(item);
    }

    QStandardItem *formToItem(Form::FormMain *form)
    {
        return _formToItem.key(form);
    }

    Form::FormMain *formForIndex(const QModelIndex &index)
    {
        QModelIndex firstRow = q->index(index.row(), 0, index.parent());
        return itemToForm(q->itemFromIndex(firstRow));
    }

    QString formLabelWithEpisodeCount(Form::FormMain *form)
    {
        QString label = form->spec()->label();
        int nb = episodeBase()->getNumberOfEpisodes(form->uuid());
        if (nb>0)
            label += QString(" (%1)").arg(nb);
        return label;
    }

    QIcon formIcon(Form::FormMain *form)
    {
        QString iconFile = form->spec()->iconFileName();
        iconFile.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
        return QIcon(iconFile);
    }

    void createItems(const QList<Form::FormMain *> &emptyrootforms)
    {
        QFont bold;
        bold.setBold(true);
        for(int i=0; i<emptyrootforms.count(); ++i) {
            Form::FormMain *root = emptyrootforms.at(i);
            foreach(Form::FormMain *form, root->flattenFormMainChildren()) {
                QStandardItem *item = new QStandardItem(formIcon(form), formLabelWithEpisodeCount(form));
                item->setFont(bold);
                linkFormAndItem(form, item);
            }
        }
    }

    void reparentItems(const QList<Form::FormMain *> &emptyrootforms, QStandardItem *defaultRootParent = 0)
    {
        if (!defaultRootParent)
            defaultRootParent = q->invisibleRootItem();
        for(int i=0; i < emptyrootforms.count(); ++i) {
            Form::FormMain *root = emptyrootforms.at(i);
            foreach(Form::FormMain *form, root->flattenFormMainChildren()) {
                QStandardItem *item = formToItem(form);
                QStandardItem *parent = 0;
                if (form->formParent() == root) {
                    parent = q->invisibleRootItem();
                } else {
                    parent = formToItem(form->formParent());
                }
                QStandardItem *itemUuid = new QStandardItem(form->uuid());
                QStandardItem *itemEmpty1 = new QStandardItem;
                QStandardItem *itemEmpty2 = new QStandardItem;
                QList<QStandardItem*> cols;
                cols << item << itemUuid << itemEmpty1 << itemEmpty2;
                //            qWarning() << parent << cols << form->uuid() << form->formParent();
                parent->appendRow(cols);
            }
        }
    }

    void createFormTree()
    {
        _formToItem.clear();
        q->clear();

        // getting Forms
        if (WarnFormRetreiving) {
            LOG_FOR(q, QString("Generating FormTreeModel: %1 empty root form(s)").arg(_rootForms.count()));
            for(int i = 0; i <_rootForms.count(); ++i) {
                LOG_FOR(q, QString("    - mode: %1; children: %2")
                        .arg(_rootForms.at(i)->modeUniqueName())
                        .arg(_rootForms.at(i)->firstLevelFormMainChildren().count()));
            }
        }
        createItems(_rootForms);
        reparentItems(_rootForms);
    }

    bool addSubForm(const SubFormInsertionPoint &insertionPoint)
    {
        qWarning()<< "ADDSUBFORM" << q->objectName() << insertionPoint;

        QList<Form::FormMain *> forms = formManager().subFormCollection(insertionPoint.subFormUid()).emptyRootForms();

        // check if receiver is in the model
        QStandardItem *receiver = 0;
        if (insertionPoint.receiverUid() == Constants::ROOT_FORM_TAG) {
            receiver = q->invisibleRootItem();
            qWarning() << "  --> Receiver == invisiblerootitem" << q->invisibleRootItem();
        } else {
            for(int i=0; i< forms.count(); ++i) {
                Form::FormMain *root = forms.at(i);
                foreach(Form::FormMain *form, root->flattenFormMainChildren()) {
                    if (form->uuid() == insertionPoint.receiverUid()) {
                        receiver = formToItem(form);
                        break;
                    }
                }
            }
        }

        qWarning() << "RECEIVER" << receiver << forms.count();

        if (!receiver)
            return false;
        createItems(forms);
        reparentItems(forms, receiver);

        return true;
    }

    /** Update the episode count of the form element \e form */
    bool updateFormCount(Form::FormMain *form)
    {
        if (!form)
            return false;
        QStandardItem *item = formToItem(form);
        if (!item)
            return false;
        item->setText(formLabelWithEpisodeCount(form));
        item->setToolTip(item->text());
        return true;
    }

public:
    QList<Form::FormMain *> _rootForms;
    QString _modeUid;
    QHash<QStandardItem *, Form::FormMain *> _formToItem;

private:
    FormTreeModel *q;
};
}  // namespace Internal
}  // namespace Form

FormTreeModel::FormTreeModel(const FormCollection &collection, QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::FormTreeModelPrivate(this))
{
    setObjectName("Form::FormTreeModel::" + collection.formUid() + collection.modeUid());
    d->_rootForms = collection.emptyRootForms();
    d->_modeUid = collection.modeUid();
//    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormsLoaded()));
}

FormTreeModel::~FormTreeModel()
{
    if (d)
        delete d;
    d = 0;
}

void FormTreeModel::initialize()
{
    d->createFormTree();
}

void FormTreeModel::refreshFormTree()
{
    d->createFormTree();
    reset();
}

static QString formTooltip(Form::FormMain *form)
{
    QString tooltip = form->spec()->tooltip();
    if (!tooltip.isEmpty())
        tooltip.prepend("<br />");
    // find the empty root parent
    Form::FormMain *root = form->rootFormParent();
    if (!root)
        root = form;
    return QString("<p style=\"font-weight:bold;\">%1</p>%2"
//                   "<p style=\"font-size:small;margin-left:10px;color:gray\">"
//                   "%2: %3<br />"
//                   "%4: %5<br />"
//                   "%6: %7"
//                   "%8</p>"
//                   "%8: %9<br />"
//                   "%10: %11"
//                   "</p>"
                   )
            .arg(form->spec()->label().replace(" ","&nbsp;"))
//            .arg(tkTr(Trans::Constants::VERSION))
//            .arg(root->spec()->version())
//            .arg(tkTr(Trans::Constants::LAST_MODIFICATION))
//            .arg(QLocale().toString(form->spec()->modificationDate(), QLocale::LongFormat).replace(" ","&nbsp;"))
//            .arg(tkTr(Trans::Constants::AUTHOR))
//            .arg(root->spec()->author())
            .arg(tooltip)

//            .arg(tkTr(Trans::Constants::VENDOR))
//            .arg(tkTr(Trans::Constants::DATA_TYPE))
//            .arg(item.pack.dataTypeName())
            ;
    return QString();
}

QVariant FormTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::ToolTipRole) {
        Form::FormMain *form = d->formForIndex(index);
        if (!form)
            return QVariant();
        return formTooltip(form);
    }
    return QStandardItemModel::data(index, role);
}

Qt::ItemFlags FormTreeModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/** Return the mode uid associated with the model */
const QString &FormTreeModel::modeUid() const
{
    return d->_modeUid;
}

/** Return true if the \e index only owns one unique episode. It is supposed that the \e index points to a form */
bool FormTreeModel::isUniqueEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    const FormMain *form = d->formForIndex(index);
    if (form)
        return (form->episodePossibilities() == FormMain::UniqueEpisode);
    return false;
}

/*! Return true if the \e index does not own episodes. It is supposed that the \e index points to a form */
bool FormTreeModel::isNoEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    const FormMain *form = d->formForIndex(index);
    if (form)
        return (form->episodePossibilities() == FormMain::NoEpisode);
    return false;
}

/** Return the Form::FormMain pointer corresponding to the index. */
Form::FormMain *FormTreeModel::formForIndex(const QModelIndex &index) const
{
    return d->formForIndex(index);
}

/** Remove all subForms from the model */
void FormTreeModel::clearSubForms()
{
}

/** Add a subform to the model according the \e insertionPoint */
bool FormTreeModel::addSubForm(const SubFormInsertionPoint &insertionPoint)
{
    bool ok = d->addSubForm(insertionPoint);
    reset();
    return ok;
}

/** Clear the content of all the model's forms */
bool FormTreeModel::clearFormContents()
{
    foreach(Form::FormMain *form, d->_rootForms) {
        form->clear();
    }
    return true;
}

/** Update the episode count of the form element corresponding to the \e index */
bool FormTreeModel::updateFormCount(const QModelIndex &index)
{
    if (index.isValid())
        return d->updateFormCount(formForIndex(index));
    return false;
}

/** Update the episode count of each form of the model. The number count is directly extracted from the episode database. */
bool FormTreeModel::updateFormCount()
{
    foreach(Form::FormMain *form, d->_formToItem.values())
        if (!d->updateFormCount(form))
            return false;
    return true;
}

void FormTreeModel::onPatientFormsLoaded()
{
    d->createFormTree();
    reset();
}
