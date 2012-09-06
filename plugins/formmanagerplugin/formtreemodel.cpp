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
#include "formtreemodel.h"
#include "iformitem.h"
#include "episodebase.h"

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
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

namespace Form {
namespace Internal {
class FormTreeModelPrivate
{
public:
    FormTreeModelPrivate(FormTreeModel *parent) :
        _rootForm(0),
        q(parent)
    {}

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

    void createFormTree()
    {
        q->clear();

        // getting Forms
        if (WarnFormRetreiving)
            LOG_FOR(q, "Generating FormTreeModel");
        QFont bold;
        bold.setBold(true);

        // create one item per form
        foreach(Form::FormMain *form, _rootForm->flattenFormMainChildren()) {

            QString label = form->spec()->label();
            int nb = episodeBase()->getNumberOfEpisodes(form->uuid());
            if (nb>0)
                label += QString(" (%1)").arg(nb);
            QString iconFile = form->spec()->iconFileName();
            iconFile.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
            QStandardItem *item = new QStandardItem(QIcon(iconFile), label);
            item->setFont(bold);
            linkFormAndItem(form, item);
        }

        // reparent items
        foreach(Form::FormMain *form, _rootForm->flattenFormMainChildren()) {
            QStandardItem *item = formToItem(form);
            QStandardItem *parent = 0;
            if (form->formParent() == _rootForm) {
                parent = q->invisibleRootItem();
            } else {
                parent = formToItem(form->formParent());
            }
            QStandardItem *itemUuid = new QStandardItem(form->uuid());
            QStandardItem *itemEmpty1 = new QStandardItem;
            QStandardItem *itemEmpty2 = new QStandardItem;
            QList<QStandardItem*> cols;
            cols << item << itemUuid << itemEmpty1 << itemEmpty2;
            parent->appendRow(cols);
        }
    }

public:
    Form::FormMain *_rootForm;
    QHash<QStandardItem *, Form::FormMain *> _formToItem;

private:
    FormTreeModel *q;
};
}  // namespace Internal
}  // namespace Form

FormTreeModel::FormTreeModel(Form::FormMain *emptyRootForm, QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::FormTreeModelPrivate(this))
{
    Q_ASSERT(emptyRootForm);
    setObjectName("Form::FormTreeModel");
    d->_rootForm = emptyRootForm;
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(updateFormCount()));
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

/** Return true is the \e index only owns one unique episode. It is supposed that the \e index points to a form */
bool FormTreeModel::isUniqueEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    FormMain *form = d->formForIndex(index);
    if (form)
        return (form->episodePossibilities()==FormMain::UniqueEpisode);
    return false;
}

/** Return true is the \e index does not own episodes. It is supposed that the \e index points to a form */
bool FormTreeModel::isNoEpisode(const QModelIndex &index)
{
    if (!index.isValid())
        return false;
    FormMain *form = d->formForIndex(index);
    if (form)
        return (form->episodePossibilities()==FormMain::NoEpisode);
    return false;
}

Form::FormMain *FormTreeModel::formForIndex(const QModelIndex &index) const
{
    return d->formForIndex(index);
}

bool FormTreeModel::updateFormCount()
{
    QHashIterator<QStandardItem *, Form::FormMain *> i(d->_formToItem);
    while (i.hasNext()) {
        i.next();
        QStandardItem *item = i.key();
        Form::FormMain *form = i.value();
        QString label = form->spec()->label();
        int nb = episodeBase()->getNumberOfEpisodes(form->uuid());
        if (nb>0)
            label += QString(" (%1)").arg(nb);
        item->setText(label);
    }
}
