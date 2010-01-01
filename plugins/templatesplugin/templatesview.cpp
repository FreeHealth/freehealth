/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "templatesview.h"
#include "templatesview_p.h"
#include "ui_templatesview.h"
#include "templatesmodel.h"
#include "itemplates.h"
#include "constants.h"
#include "templateseditdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <QObject>
#include <QWidget>
#include <QDebug>

using namespace Templates;
using namespace Templates::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::UniqueIDManager *uid() {return Core::ICore::instance()->uniqueIDManager();}
static inline Core::ContextManager *contextManager() {return Core::ICore::instance()->contextManager();}

static inline QAction *registerAction(const QString &objName, Core::ActionContainer *menu, const QString &iconName,
                                      const QString &actionName, const QString &group, const QString &tr,
                                      const QList<int> &context, QObject *parent)
{
    QAction *toReturn = new QAction(parent);
    toReturn->setObjectName(objName);
    toReturn->setIcon(theme()->icon(iconName));
    Core::Command *cmd = actionManager()->registerAction(toReturn, actionName, context);
    cmd->setTranslations(tr);
    menu->addAction(cmd, group);
    return toReturn;
}


namespace TemplatesViewConstants
{
    static const char* const C_BASIC              = "context.TemplatesView.basic";
    static const char* const C_BASIC_ADD          = "context.TemplatesView.Add";
    static const char* const C_BASIC_REMOVE       = "context.TemplatesView.Remove";
}

/////////////////////////////////////////////////////////////////////////// List View Manager
TemplatesViewManager *TemplatesViewManager::m_Instance = 0;

TemplatesViewManager *TemplatesViewManager::instance(QObject *parent)
{
    if (!m_Instance) {
        if (!parent) {
            m_Instance = new TemplatesViewManager(qApp);
        } else {
            m_Instance = new TemplatesViewManager(parent);
        }
    }
    return m_Instance;
}

TemplatesViewManager::TemplatesViewManager(QObject *parent) : TemplatesViewActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
}

void TemplatesViewManager::updateContext(Core::IContext *object)
{
//    qWarning() << "*** TemplatesViewManager::updateContext ***";
//    if (object)
//        qWarning() << object;
    Templates::TemplatesView *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }
        view = qobject_cast<Templates::TemplatesView *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        TemplatesViewActionHandler::setCurrentView(view);
    }
}



/////////////////////////////////////////////////////////////////////////// Action Handler
TemplatesViewActionHandler::TemplatesViewActionHandler(QObject *parent) :
        QObject(parent),
        aAddCategory (0),
        aAddTemplate(0),
        aRemoveCategory(0),
        aRemoveTemplate(0),
        aEditCategory(0),
        aEditTemplate(0),
        m_CurrentView(0)
{
    QList<int> context = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC);
    QList<int> addContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_ADD);
    QList<int> removeContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_REMOVE);
//    QList<int> moveContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_MOVE);

    // Edit Menu and Contextual Menu
    Core::ActionContainer *editMenu = actionManager()->actionContainer(Core::Constants::M_EDIT);
    Core::ActionContainer *cmenu = actionManager()->actionContainer(Core::Constants::M_EDIT_TEMPLATES);
    if (!cmenu) {
        cmenu = actionManager()->createMenu(Core::Constants::M_EDIT_TEMPLATES);
        cmenu->appendGroup(Core::Constants::G_EDIT_TEMPLATES);
        cmenu->appendGroup(Core::Constants::G_EDIT_CATEGORIES);
        cmenu->setTranslations(Trans::Constants::M_EDIT_TEMPLATES_TEXT);
        if (editMenu)
            editMenu->addMenu(cmenu, Core::Constants::G_EDIT_TEMPLATES);
    }

    // Add
    aAddCategory = registerAction("TemplatesView.aAddCategory", cmenu, Core::Constants::ICONADD,
                                  Core::Constants::A_CATEGORY_ADD, Core::Constants::G_EDIT_CATEGORIES,
                                  Trans::Constants::CATEGORYADD_TEXT, addContext, this);
    connect(aAddCategory, SIGNAL(triggered()), this, SLOT(addCategory()));

    aAddTemplate = registerAction("TemplatesView.aAddCategory", cmenu, Core::Constants::ICONADD,
                                  Core::Constants::A_TEMPLATE_ADD, Core::Constants::G_EDIT_TEMPLATES,
                                  Trans::Constants::TEMPLATEADD_TEXT, addContext, this);
    connect(aAddTemplate, SIGNAL(triggered()), this, SLOT(addTemplate()));

    // Remove
    aRemoveTemplate = registerAction("TemplatesView.aRemoveTemplate", cmenu, Core::Constants::ICONREMOVE,
                                     Core::Constants::A_TEMPLATE_REMOVE, Core::Constants::G_EDIT_TEMPLATES,
                                     Trans::Constants::TEMPLATEREMOVE_TEXT, removeContext, this);
    connect(aRemoveTemplate, SIGNAL(triggered()), this, SLOT(removeTemplate()));

    aRemoveCategory = registerAction("TemplatesView.aRemoveCategory", cmenu, Core::Constants::ICONREMOVE,
                                     Core::Constants::A_CATEGORY_REMOVE, Core::Constants::G_EDIT_CATEGORIES,
                                     Trans::Constants::CATEGORYREMOVE_TEXT, removeContext, this);
    connect(aRemoveCategory, SIGNAL(triggered()), this, SLOT(removeCategory()));

    // Edit
    aEditTemplate = registerAction("TemplatesView.aEditTemplate", cmenu, Core::Constants::ICONEDIT,
                                   Core::Constants::A_TEMPLATE_EDIT, Core::Constants::G_EDIT_TEMPLATES,
                                   Trans::Constants::TEMPLATEEDIT_TEXT, context, this);
    connect(aEditTemplate, SIGNAL(triggered()), this, SLOT(editCurrentItem()));

    aEditCategory = registerAction("TemplatesView.aEditCategory", cmenu, Core::Constants::ICONEDIT,
                                   Core::Constants::A_CATEGORY_EDIT, Core::Constants::G_EDIT_CATEGORIES,
                                   Trans::Constants::CATEGORYEDIT_TEXT, context, this);
    connect(aEditCategory, SIGNAL(triggered()), this, SLOT(editCurrentItem()));
}

void TemplatesViewActionHandler::setCurrentView(TemplatesView *view)
{
//    if (view)
//        qWarning() << "TemplatesViewActionHandler" << view;
    // disconnect old view
    if (m_CurrentView) {
        disconnect(m_CurrentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(templatesViewItemChanged()));
    }
    m_CurrentView = view;
    if (!view) { // this should never be the case
        return;
    }
    // reconnect some actions
    connect(m_CurrentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(templatesViewItemChanged()));
    updateActions();
}

void TemplatesViewActionHandler::templatesViewItemChanged()
{
    bool templateState = m_CurrentView->currentItemIsTemplate();
    aAddCategory->setEnabled(!templateState);
    aAddTemplate->setEnabled(templateState);
    aRemoveCategory->setEnabled(!templateState);
    aRemoveTemplate->setEnabled(templateState);
    aEditCategory->setEnabled(!templateState);
    aEditTemplate->setEnabled(templateState);
}

void TemplatesViewActionHandler::updateActions()
{
    templatesViewItemChanged();
}

void TemplatesViewActionHandler::editCurrentItem()
{
    if (m_CurrentView)
        m_CurrentView->editCurrentItem();
}

//void TemplatesViewActionHandler::removeItem()
//{ if (m_CurrentView) m_CurrentView->removeItem(); }
//


namespace Templates {
namespace Internal {
class TemplatesViewPrivate : public QObject
{
    Q_OBJECT
public:
    TemplatesViewPrivate(TemplatesView *parent) : QObject(parent), q(parent), m_Model(0), m_ui(0), m_Context(0)
    {
        // Create Actions context
        m_Context = new Templates::Internal::TemplatesViewContext(q);
        m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC));
        m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_ADD));
        m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_REMOVE));
        contextManager()->addContextObject(m_Context);

        // Create Ui
        m_ui = new Ui::TemplatesView;
        m_ui->setupUi(q);
        m_Model = new TemplatesModel(this);
        m_ui->categoryTreeView->setModel(m_Model);
        if (settings()->value(Templates::Constants::S_ALWAYSSHOWEXPANDED, true).toBool())
            m_ui->categoryTreeView->expandAll();
        m_ui->categoryTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
        m_ui->categoryTreeView->setDragEnabled(true);
        m_ui->categoryTreeView->setAcceptDrops(true);
        m_ui->categoryTreeView->setDropIndicatorShown(true);
        int i;
        for(i=1; i < m_ui->categoryTreeView->model()->columnCount(); ++i) {
            m_ui->categoryTreeView->setColumnHidden(i, true);
        }
        QList<int> sizes;
        foreach(const QVariant &s, settings()->value(Constants::S_SPLITTER_SIZES).toList()) {
            sizes << s.toInt();
        }
        if (sizes.count() == 0) {
            sizes << 1 << 1;
        }
        m_ui->splitter->setSizes(sizes);

        // make connections
        connect(m_ui->categoryTreeView->selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this,
                SLOT(on_categoryTreeView_changed(const QModelIndex&,const QModelIndex&)));
        connect(m_ui->categoryTreeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));
        connect(m_ui->splitter, SIGNAL(splitterMoved(int,int)),
                this, SLOT(on_splitter_move()));
    }
    ~TemplatesViewPrivate() {}

public Q_SLOTS:
    void on_categoryTreeView_changed(const QModelIndex &current, const QModelIndex &)
    {
        Q_EMIT q->currentItemChanged();
        QModelIndex sum = m_Model->index(current.row(), TemplatesModel::Data_Summary, current.parent());
        m_ui->summaryTextEdit->setText(sum.data().toString());
    }
    void on_splitter_move()
    {
        QList<QVariant> sizes;
        foreach(int s, m_ui->splitter->sizes())
            sizes << s;
        settings()->setValue(Constants::S_SPLITTER_SIZES, sizes);
    }
    QMenu *getContextMenu()
    {
        return actionManager()->actionContainer(Core::Constants::M_EDIT_TEMPLATES)->menu();
    //    return pop;
    }
    void contextMenu(const QPoint &p)
    {
        QMenu *pop = getContextMenu();
        pop->popup(m_ui->categoryTreeView->mapToGlobal(p));
        pop->exec();
    }
    void on_ModelAboutToReset() {
        /** \todo Store the expanded indexes of the view and restore when view resets. */
    }

public:
    TemplatesView *q;
    TemplatesModel *m_Model;
    Ui::TemplatesView *m_ui;
    TemplatesViewContext *m_Context;
};
}
}

TemplatesView::TemplatesView(QWidget *parent) :
    QWidget(parent),
    d(new TemplatesViewPrivate(this))
{
}

TemplatesView::~TemplatesView()
{
    delete d->m_ui;
    d->m_ui = 0;
    if (d) {
        delete d;
        d = 0;
    }
}

QItemSelectionModel *TemplatesView::selectionModel() const
{
    return d->m_ui->categoryTreeView->selectionModel();
}

bool TemplatesView::currentItemIsTemplate() const
{
    QModelIndex index = d->m_ui->categoryTreeView->selectionModel()->currentIndex();
    if (index.isValid()) {
        return d->m_Model->isTemplate(index);
    }
    return false;
}

void TemplatesView::editCurrentItem()
{
    QModelIndex idx = d->m_ui->categoryTreeView->selectionModel()->currentIndex();
    if (!idx.isValid())
        return;
    TemplatesEditDialog dlg(this);
    dlg.setModel(d->m_Model);
    dlg.setModelIndex(idx);
    dlg.exec();
}


void TemplatesView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

#include "templatesview.moc"
