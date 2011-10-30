/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "formfilesselectorwidget.h"
#include "ui_formfilesselectorwidget.h"
#include "iformio.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

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

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
inline static QList<Form::IFormIO*> refreshIOPlugs() {return pluginManager()->getObjects<Form::IFormIO>();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}


namespace Form {
namespace Internal {

class FormFilesSelectorWidgetPrivate
{
public:
    FormFilesSelectorWidgetPrivate() :
            ui(new Ui::FormFilesSelectorWidget),
            aByCategory(0), aByAuthor(0), aBySpecialties(0), aByType(0),
            m_TreeModel(0),
            m_ActualTreeModelColumn(-1)
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
        query.setGetAllAvailableFormDescriptions(true);
        switch (m_Type) {
        case FormFilesSelectorWidget::AllForms: query.setTypeOfForms(Form::FormIOQuery::CompleteForms | Form::FormIOQuery::SubForms | Form::FormIOQuery::Pages);break;
        case FormFilesSelectorWidget::CompleteForms: query.setTypeOfForms(Form::FormIOQuery::CompleteForms); break;
        case FormFilesSelectorWidget::SubForms: query.setTypeOfForms(Form::FormIOQuery::SubForms); break;
        case FormFilesSelectorWidget::Pages: query.setTypeOfForms(Form::FormIOQuery::Pages); break;
        }
        foreach(Form::IFormIO *io, ios) {
            m_FormDescr = io->getFormFileDescriptions(query);
        }
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
            item->setData(i, Qt::UserRole+1);
            catItem->appendRow(item);
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

    // Create and connect actions
    d->createActions();
    connect(d->aByCategory, SIGNAL(triggered()), this, SLOT(onFilterSelected()));
    connect(d->aByAuthor, SIGNAL(triggered()), this, SLOT(onFilterSelected()));
    connect(d->aBySpecialties, SIGNAL(triggered()), this, SLOT(onFilterSelected()));
    connect(d->aByType, SIGNAL(triggered()), this, SLOT(onFilterSelected()));

    // Get current generic form file
//    QString actual = episodeBase()->getGenericFormFile();
//    actual = actual.mid(settings()->path(Core::ISettings::CompleteFormsPath).length() + 1);
//    d->ui->actualFile->setText(actual);

    // get all Forms description
    d->getDescriptions();

    // prepare the first model = category tree model
    d->aByCategory->trigger();
//    d->createTreeModel(Form::FormIODescription::Category);
    d->ui->treeView->setModel(d->m_TreeModel);
    d->ui->treeView->header()->hide();
//    d->ui->treeView->setRootIndex(d->dirModel->index(settings()->path(Core::ISettings::CompleteFormsPath)));

    // connect actions, buttons...
    connect(d->ui->treeView, SIGNAL(activated(QModelIndex)),this, SLOT(on_treeView_activated(QModelIndex)));
}

FormFilesSelectorWidget::~FormFilesSelectorWidget()
{
    delete d;
}

void FormFilesSelectorWidget::setFormType(FormType type)
{
    if (d->m_Type==type)
        return;
    d->m_Type = type;
    d->getDescriptions();
    d->createTreeModel(d->m_ActualTreeModelColumn, true);
}

void FormFilesSelectorWidget::setSelectionType(SelectionType type)
{
    if (d->m_SelType==type)
        return;
    d->ui->treeView->clearSelection();
    d->ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode(type));
}

void FormFilesSelectorWidget::expandAllItems() const
{
    d->ui->treeView->expandAll();
}

QList<Form::FormIODescription *> FormFilesSelectorWidget::selectedForms() const
{
    QList<Form::FormIODescription *> toReturn;
    QItemSelectionModel *model = d->ui->treeView->selectionModel();
    if (!model->hasSelection())
        return toReturn;
    foreach(const QModelIndex &index, model->selectedIndexes()) {
        int id = index.data(Qt::UserRole+1).toInt();
        if (id >= 0 && id < d->m_FormDescr.count()) {
            Form::FormIODescription *descr = d->m_FormDescr.at(id);
            toReturn << descr;
        }
    }
    return toReturn;
}

void FormFilesSelectorWidget::on_treeView_activated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    // get the FormIODescription
    int id = d->ui->treeView->currentIndex().data(Qt::UserRole+1).toInt();
    if (id >= 0 && id < d->m_FormDescr.count()) {
        Form::FormIODescription *descr = d->m_FormDescr.at(id);
        descr->toTreeWidget(d->ui->treeWidget);
    } else {
        d->ui->treeWidget->clear();
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
