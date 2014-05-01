/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Forms::FormFilesSelectorWidget
 * A patient form file selector widget. Shows all available forms using
 * - an internal tree model
 * - a category sorter (by category, by specialties...)
 * Allow user to browse available form screenshots.
 */

#include "formfilesselectorwidget.h"
#include "ui_formfilesselectorwidget.h"
#include "iformio.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/global.h>
#include <utils/widgets/imageviewer.h>
#include <extensionsystem/pluginmanager.h>

#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QAction>
#include <QItemSelectionModel>

#include <QDebug>

using namespace Form;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline QList<Form::IFormIO*> refreshIOPlugs() {return pluginManager()->getObjects<Form::IFormIO>();}

const int IndexRole = Qt::UserRole+1;

namespace Form {
namespace Internal {

class FormFilesSelectorWidgetPrivate
{
public:
    FormFilesSelectorWidgetPrivate() :
            ui(new Ui::FormFilesSelectorWidget),
            aByCategory(0), aByAuthor(0), aBySpecialties(0), aByType(0),
            m_TreeModel(0),
            m_ActualTreeModelColumn(-1),
            m_GetLocal(false),
            m_ExcludeGenderSpecific(false)
    {}

    ~FormFilesSelectorWidgetPrivate()
    {
        qDeleteAll(m_FormDescr);
        m_FormDescr.clear();
        delete ui;
//        delete m_IconProvider;
        if (m_TreeModel)
            delete m_TreeModel;
        m_TreeModel = 0;
    }

    void createActions()
    {
        QAction *a = aByCategory = new QAction(ui->toolButton);
        a->setIcon(theme()->icon(Core::Constants::ICONCATEGORY_MANAGER));
        a->setText(QCoreApplication::translate("FormFilesSelectorWidget", "by category"));
        a->setToolTip(QCoreApplication::translate("FormFilesSelectorWidget", "by category"));
        ui->toolButton->addAction(a);

        a = aByAuthor = new QAction(ui->toolButton);
        a->setIcon(theme()->icon(Core::Constants::ICONUSER));
        a->setText(QCoreApplication::translate("FormFilesSelectorWidget", "by author"));
        a->setToolTip(QCoreApplication::translate("FormFilesSelectorWidget", "by author"));
        ui->toolButton->addAction(a);

        a = aBySpecialties = new QAction(ui->toolButton);
        a->setIcon(theme()->icon(Core::Constants::ICONFREEMEDFORMS));
        a->setText(QCoreApplication::translate("FormFilesSelectorWidget", "by specialty"));
        a->setToolTip(QCoreApplication::translate("FormFilesSelectorWidget", "by specialty"));
        ui->toolButton->addAction(a);

        a = aByType = new QAction(ui->toolButton);
        a->setIcon(theme()->icon(Core::Constants::ICONFREEMEDFORMS));
        a->setText(QCoreApplication::translate("FormFilesSelectorWidget", "by type"));
        a->setToolTip(QCoreApplication::translate("FormFilesSelectorWidget", "by type"));
        ui->toolButton->addAction(a);

//        ui->toolButton->setDefaultAction(aByCategory);
    }

    void getDescriptions()
    {
        qDeleteAll(m_FormDescr);
        m_FormDescr.clear();
        ios = refreshIOPlugs();
        Form::FormIOQuery query;
        query.setGetAllAvailableFormDescriptions(m_GetLocal);
        query.setExcludeGenderSpecific(m_ExcludeGenderSpecific);
        switch (m_Type) {
        case FormFilesSelectorWidget::AllForms:
            query.setTypeOfForms(Form::FormIOQuery::CompleteForms | Form::FormIOQuery::SubForms);
            break;
        case FormFilesSelectorWidget::CompleteForms:
            query.setTypeOfForms(Form::FormIOQuery::CompleteForms);
            break;
        case FormFilesSelectorWidget::SubForms:
            query.setTypeOfForms(Form::FormIOQuery::SubForms);
            break;
        // case FormFilesSelectorWidget::Pages:
        //     query.setTypeOfForms(Form::FormIOQuery::Pages);
        //     break;
        default: break;
        }
        foreach(Form::IFormIO *io, ios)
            m_FormDescr = io->getFormFileDescriptions(query);
    }

    void createTreeModel(const int treeItemReference, bool forceUpdate = false)
    {
        if (!forceUpdate && (m_ActualTreeModelColumn == treeItemReference))
            return;
        m_ActualTreeModelColumn = treeItemReference;
        QFont bold;
        bold.setBold(true);
        if (!m_TreeModel) {
            m_TreeModel = new QStandardItemModel;
        } else {
            m_TreeModel->clear();
        }
        QStandardItem *parentItem = m_TreeModel->invisibleRootItem();
        QHash<QString, QStandardItem *> categories;
        for(int i=0; i < m_FormDescr.count(); ++i) {
            Form::FormIODescription *descr = m_FormDescr.at(i);
            // Do not manage excluded forms
            if (_excludeUids.contains(descr->data(Form::FormIODescription::UuidOrAbsPath).toString(), Qt::CaseInsensitive))
                continue;

            const QString &cat = descr->data(treeItemReference).toString();
            QStandardItem *catItem = 0;
            if (!categories.contains(cat)) {
                QString catName = descr->data(treeItemReference).toString();
                if (catName.isEmpty()) {
                    catName = QCoreApplication::translate("FormFilesSelectorWidget", "Default Forms");
                }
                catItem = new QStandardItem(catName);
                catItem->setFont(bold);
                categories.insert(cat, catItem);
                parentItem->appendRow(catItem);
            } else {
                catItem = categories.value(cat);
            }
            QStandardItem *item = new QStandardItem(descr->data(FormIODescription::ShortDescription).toString());
            item->setData(i, IndexRole);
            catItem->appendRow(item);
            // highlight ?
            if (!m_HightlightUuid.isEmpty()) {
                if (descr->data(FormIODescription::UuidOrAbsPath).toString()==m_HightlightUuid) {
                    item->setFont(bold);
                }
            }
        }
    }

public:
    Ui::FormFilesSelectorWidget *ui;
    FormFilesSelectorWidget::FormType m_Type;
    QAction *aByCategory, *aByAuthor, *aBySpecialties, *aByType;
    QList<Form::IFormIO*> ios;
    QList<Form::FormIODescription *> m_FormDescr;
    QStandardItemModel *m_TreeModel;
    int m_ActualTreeModelColumn, m_SelType;
    QString m_HightlightUuid;
    bool m_GetLocal, m_ExcludeGenderSpecific;
    QStringList _excludeUids;
};

}  // End namespace Internal
}  // End namespace Form



FormFilesSelectorWidget::FormFilesSelectorWidget(QWidget *parent, const FormType type, const SelectionType selType) :
    QWidget(parent),
    d(new FormFilesSelectorWidgetPrivate)
{
    d->m_Type = type;
    d->m_SelType = selType;
    d->ui->setupUi(this);
    layout()->setMargin(0);
    layout()->setSpacing(5);

    // Create and connect actions
    d->createActions();
    connect(d->aByCategory, SIGNAL(triggered()), this, SLOT(onFilterSelected()));
    connect(d->aByAuthor, SIGNAL(triggered()), this, SLOT(onFilterSelected()));
    connect(d->aBySpecialties, SIGNAL(triggered()), this, SLOT(onFilterSelected()));
    connect(d->aByType, SIGNAL(triggered()), this, SLOT(onFilterSelected()));

//    // get all Forms description
//    d->getDescriptions();

    // prepare the first model = category tree model
    d->aByCategory->trigger();
    d->ui->formsTreeView->setModel(d->m_TreeModel);
    d->ui->formsTreeView->header()->hide();

    // connect actions, buttons...
    connect(d->ui->formsTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(onDescriptionSelected(QModelIndex,QModelIndex)));
    connect(d->ui->screenshotsButton, SIGNAL(clicked()), this, SLOT(showScreenShot()));
}

FormFilesSelectorWidget::~FormFilesSelectorWidget()
{
    delete d;
}

/**
 * Define the type of form to use in the selector (complete forms, subforms, both...),
 * and get all available forms descriptions.
*/
void FormFilesSelectorWidget::setFormType(FormType type)
{
    if (d->m_Type==type)
        return;
    d->m_Type = type;
    d->getDescriptions();
    d->createTreeModel(d->m_ActualTreeModelColumn, true);
}

/**
 * Some forms are gender specific, by setting this property to true, all form gender will be checked
 * and excluded if the gender is different to the current patient gender. Otherwise, gender is not checked.
 * By default, this property is set to false.
 * Note: must be defined before the FormType using setFormType().
 */
void FormFilesSelectorWidget::setExcludeGenderSpecific(bool excludeGenderSpecific)
{
    d->m_ExcludeGenderSpecific = excludeGenderSpecific;
}

/**
 * Exclude some form using a uuid matching. Use the emptyroot uuid.
 * This param must be defined before calling setFormType().
 * \code
 * uuid example: __completeForms__/gp_basic1
 * \endcode
 */
void FormFilesSelectorWidget::setExcludeFormByUid(const QStringList &formuids)
{
    d->_excludeUids = formuids;
}

/**
 * Returns the excluded forms using a uuid matching. Uses the emptyroot uuid.
 */
const QStringList &FormFilesSelectorWidget::excludedFormByUid() const
{
    return d->_excludeUids;
}

/** Define the selection type to single item selection or multiple item selection. */
void FormFilesSelectorWidget::setSelectionType(SelectionType type)
{
    if (d->m_SelType==type)
        return;
    d->ui->formsTreeView->clearSelection();
    d->ui->formsTreeView->setSelectionMode(QAbstractItemView::SelectionMode(type));
}

/** Selector presents form in a tree view, you can expand all its item using this function. */
void FormFilesSelectorWidget::expandAllItems() const
{
    d->ui->formsTreeView->expandAll();
}

/**
 * The form selector asks Form::IFormIO engines for forms. You can require only
 * forms from database and include local files. By default, local files are not
 * included in the form selector.
 * If you set this value you must redefine the FormType using setFormType().
 */
void FormFilesSelectorWidget::setIncludeLocalFiles(bool includeLocal)
{
    d->m_GetLocal = includeLocal;
}

/** Return the currently selected form descriptions */
QList<Form::FormIODescription *> FormFilesSelectorWidget::selectedForms() const
{
    QList<Form::FormIODescription *> toReturn;
    QItemSelectionModel *model = d->ui->formsTreeView->selectionModel();
    if (!model->hasSelection())
        return toReturn;
    foreach(const QModelIndex &index, model->selectedIndexes()) {
        int id = index.data(IndexRole).toInt();
        if (id >= 0 && id < d->m_FormDescr.count()) {
            Form::FormIODescription *descr = d->m_FormDescr.at(id);
            toReturn << descr;
        }
    }
    return toReturn;
}

/** Highlight a specific form in the selector (using its \e uuidOrAbsPath) */
void FormFilesSelectorWidget::highlighForm(const QString &uuidOrAbsPath)
{
    d->m_HightlightUuid = uuidOrAbsPath;
    d->createTreeModel(d->m_ActualTreeModelColumn, true);
}

void FormFilesSelectorWidget::onDescriptionSelected(const QModelIndex &index, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!index.isValid() || !index.parent().isValid()) {
        d->ui->textBrowser->clear();
        return;
    }
    // get the FormIODescription
    int id = d->ui->formsTreeView->currentIndex().data(IndexRole).toInt();
    if (IN_RANGE_STRICT_MAX(id, 0, d->m_FormDescr.count())) {
        Form::FormIODescription *descr = d->m_FormDescr.at(id);
        d->ui->screenshotsButton->setEnabled(descr->hasScreenShots());
        d->ui->textBrowser->setHtml(descr->toHtml());
    } else {
        d->ui->textBrowser->clear();
    }
}

void FormFilesSelectorWidget::onFilterSelected()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    if (action == d->aByCategory) {
        d->createTreeModel(Form::FormIODescription::Category);
    } else if (action == d->aByAuthor) {
        d->createTreeModel(Form::FormIODescription::Author);
    } else if (action == d->aBySpecialties) {
        d->createTreeModel(Form::FormIODescription::Specialties);
    } else if (action == d->aByType) {
        d->createTreeModel(Form::FormIODescription::TypeName);
    }
    d->ui->toolButton->setDefaultAction(action);
}

void FormFilesSelectorWidget::showScreenShot()
{
    // Get screenshots from FormIOReader
    int id = d->ui->formsTreeView->currentIndex().data(IndexRole).toInt();
    if (id >= 0 && id < d->m_FormDescr.count()) {
        Form::FormIODescription *descr = d->m_FormDescr.at(id);
        // Create ImageViewer dialog
        Utils::ImageViewer dlg(this);
        dlg.setPixmaps(descr->screenShots());
        dlg.exec();
    }
}

void FormFilesSelectorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (d->ui) {
            d->ui->retranslateUi(this);
            // correcting a Qt bug
            d->ui->toolButton->defaultAction()->trigger();
        }
        break;
    default:
        break;
    }
}
