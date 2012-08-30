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
  \class Form::FormPlaceHolder
  Widget containing the Episode treeView and the forms in a QStackedLayout
  When an episode is activated by the user, the formViewer is set to the corresponding form
  and episode datas. Datas are automatically saved (without any user intervention).
*/

#include "formplaceholder.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "formeditordialog.h"
#include "formtreemodel.h"

#include "ui_formplaceholder.h"

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/episodemodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <listviewplugin/treeview.h>

#include <patientbaseplugin/constants_db.h>

#include <utils/log.h>
#include <utils/widgets/minisplitter.h>
#include <extensionsystem/pluginmanager.h>

#include <extensionsystem/pluginmanager.h>

#include <QTreeView>
#include <QTreeWidgetItem>
#include <QStackedLayout>
#include <QScrollArea>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QEvent>

// Test
#include <QTextBrowser>



#include <QDebug>

using namespace Form;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
inline static Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

namespace {
const char * const TREEVIEW_SHEET =
        " QTreeView {"
        "    show-decoration-selected: 1;"
        "}"

        "QTreeView::item {"
//        "    border: 0px;"
        "    background: base;"
//        "    border-top-color: transparent;"
//        "    border-bottom-color: transparent;"
        "}"

        "QTreeView::item:hover {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 0px solid #bfcde4;"
        "}"

//        "QTreeView::branch:hover {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 0px solid #bfcde4;"
//        "}"

//        "QTreeView::item:selected {"
//        "    border: 0px solid #567dbc;"
//        "}"

        "QTreeView::item:selected {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"

        "QTreeView::branch:selected {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"

//        "QTreeView::item:selected:!active {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
//        "}"
        ;

}

namespace Form {
namespace Internal {

class FormPlaceHolderPrivate
{
public:
    FormPlaceHolderPrivate(FormPlaceHolder *parent) :
        ui(new Ui::FormPlaceHolder),
            _rootForm(0),
            _formTreeModel(0),
            _delegate(0),
            q(parent)
    {
    }

    ~FormPlaceHolderPrivate()
    {
        delete ui;
    }

    void useEpisodeModel(Form::FormMain *form)
    {
        if (!form)
            return;
        EpisodeModel *model = formManager()->episodeModel(form);
        if (!model)
            LOG_ERROR_FOR(q, "No episode model for form " + form->uuid());
        ui->episodeView->setModel(model);
    }

public:
    Ui::FormPlaceHolder *ui;
    FormMain *_rootForm;
    FormTreeModel *_formTreeModel;
    FormItemDelegate *_delegate;

    QHash<int, QString> m_StackId_FormUuid;

private:
    FormPlaceHolder *q;
};

FormItemDelegate::FormItemDelegate(QObject *parent) :
        QStyledItemDelegate(parent)
{
}

void FormItemDelegate::setEpisodeModel(EpisodeModel *model)
{
//    _episodeModel = model;
}

QSize FormItemDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    const bool topLevel = !index.parent().isValid();
    if (topLevel) {
        return QStyledItemDelegate::sizeHint(option, index) + QSize(10,10);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

void FormItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
//    const bool topLevel = !index.parent().isValid();

    // Add the fancy button
//    if (option.state & QStyle::State_MouseOver) {
//        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
//            pressedIndex = QModelIndex();
//        QBrush brush = option.palette.alternateBase();
//        if (index == pressedIndex)
//            brush = option.palette.dark();
//        painter->fillRect(option.rect, brush);
//    }

//    QStyledItemDelegate::paint(painter, option, index);

//    // Draw fancy button
//    if (index.column()==FormTreeModel::EmptyColumn1 && option.state & QStyle::State_MouseOver) {
//        QIcon icon;
//        if (option.state & QStyle::State_Selected) {
//            if (_episodeModel->isEpisode(index)) {
//                icon = theme()->icon(Core::Constants::ICONVALIDATELIGHT);
//            } else {
//                // test the form to be unique or multiple episode
//                if (_episodeModel->isUniqueEpisode(index) && _episodeModel->rowCount(index) == 1)
//                    return;
//                if (_episodeModel->isNoEpisode(index))
//                    return;
//                icon = theme()->icon(Core::Constants::ICONADDLIGHT);
//            }
//        } else {
//            if (_episodeModel->isEpisode(index)) {
//                icon = theme()->icon(Core::Constants::ICONVALIDATEDARK);
//            } else {
//                // test the form to be unique or multiple episode
//                if (_episodeModel->isUniqueEpisode(index) && _episodeModel->rowCount(index) == 1)
//                    return;
//                if (_episodeModel->isNoEpisode(index))
//                    return;
//                icon = theme()->icon(Core::Constants::ICONADDDARK);
//            }
//        }

//        QRect iconRect(option.rect.right() - option.rect.height(),
//                       option.rect.top(),
//                       option.rect.height(),
//                       option.rect.height());

//        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
//    }
}

}
}

FormPlaceHolder::FormPlaceHolder(QWidget *parent) :
        FormContextualWidget(parent),
        d(new Internal::FormPlaceHolderPrivate(this))
{
    FormManager::instance();
    d->ui->setupUi(this);
    layout()->setMargin(0);
    layout()->setSpacing(0);

    d->ui->formDataMapper->initialize();

    // Manage Form File tree view
    d->ui->formView->setActions(0);
    d->ui->formView->setCommands(QStringList()
                                 << Constants::A_ADDEPISODE
                                 << Constants::A_VALIDATEEPISODE
                                 << Constants::A_ADDFORM
                                 << Constants::A_PRINTFORM);
    d->ui->formView->addContexts(contexts());
    d->ui->formView->setDeselectable(false);
    d->ui->formView->disconnectActionsToDefaultSlots();
//    d->ui->formView->viewport()->setAttribute(Qt::WA_Hover);
//    tree->setItemDelegate((d->_delegate = new Internal::FormItemDelegate(d->ui->formView)));
    d->ui->formView->treeView()->setFrameStyle(QFrame::NoFrame);
    d->ui->formView->treeView()->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->ui->formView->treeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->formView->treeView()->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->formView->treeView()->setAlternatingRowColors(settings()->value(Constants::S_USEALTERNATEROWCOLOR).toBool());
//    tree->setRootIsDecorated(false);
//    QString css = ::TREEVIEW_SHEET;
//    if (settings()->value(Constants::S_USESPECIFICCOLORFORROOTS).toBool()) {
//        css += QString("QTreeView:item:top {background:%1;}")
////                       "QTreeView::branch:top {background:%1;}")
//                .arg(settings()->value(Constants::S_FOREGROUNDCOLORFORROOTS).toString());
//    }
    d->ui->formView->treeView()->setStyleSheet(::TREEVIEW_SHEET);

    connect(d->ui->formView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(d->ui->formView, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));

    Core::Command *cmd = actionManager()->command(Constants::A_ADDEPISODE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(newEpisode()));
    cmd = actionManager()->command(Constants::A_ADDFORM);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(addForm()));
    cmd = actionManager()->command(Constants::A_PRINTFORM);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(printCurrentItem()));

    int width = size().width();
    int third = width/3;
    d->ui->horizontalSplitter->setSizes(QList<int>() << third << width-third);

    d->ui->verticalSplitter->setStretchFactor(0, 1);
    d->ui->verticalSplitter->setStretchFactor(1, 3);
}

FormPlaceHolder::~FormPlaceHolder()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/**
 * Define the Form::FormMain root item to use for the creation of the patient files.
 * This object will manage deletion of the root item and its children.
 */
void FormPlaceHolder::setRootForm(Form::FormMain *rootForm)
{
    if (!rootForm)
        return;
    d->_rootForm = rootForm;

    // Manage Form tree view / model
    if (d->_formTreeModel)
        delete d->_formTreeModel;
    d->_formTreeModel = new FormTreeModel(rootForm, this);
    d->_formTreeModel->initialize();

    QTreeView *tree = d->ui->formView->treeView();
    tree->setModel(d->_formTreeModel);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i=0; i < FormTreeModel::MaxData; ++i)
        tree->setColumnHidden(i, true);
    tree->setColumnHidden(FormTreeModel::Label, false);
    tree->setColumnHidden(FormTreeModel::EmptyColumn1, false);
    tree->header()->hide();
    tree->header()->setStretchLastSection(false);
    tree->header()->setResizeMode(FormTreeModel::Label, QHeaderView::Stretch);
    tree->header()->setResizeMode(FormTreeModel::EmptyColumn1, QHeaderView::Fixed);
    tree->header()->resizeSection(FormTreeModel::EmptyColumn1, 16);
    tree->expandAll();

    // Manage Episode table view / model
    QTableView *table = d->ui->episodeView;
    for(int i=0; i < EpisodeModel::MaxData; ++i)
        table->setColumnHidden(i, true);
    table->setColumnHidden(EpisodeModel::Label, false);
    table->setColumnHidden(EpisodeModel::UserCreatorName, false);
    table->setColumnHidden(EpisodeModel::UserDate, false);
//    table->setColumnHidden(EpisodeModel::CreationDate, false);


    Core::Command *cmd = actionManager()->command(Constants::A_SHOWPATIENTLASTEPISODES);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(showLastEpisodeSynthesis()));

    // start on the Last Episode Synthesis view
//    tree->setCurrentIndex(d->_episodeModel->index(0,0));
    setCurrentForm(Constants::PATIENTLASTEPISODES_UUID);
}

void FormPlaceHolder::handlePressed(const QModelIndex &index)
{
    qWarning() << "handlePressed";
    if (index.column() == FormTreeModel::Label) {
//        d->_episodeModel->activateEpisode(index);
    } else if (index.column() == FormTreeModel::EmptyColumn1) {
        d->_delegate->pressedIndex = index;
    }
}

void FormPlaceHolder::handleClicked(const QModelIndex &index)
{
    qWarning() << "handleClicked";
    if (index.column() == FormTreeModel::EmptyColumn1) { // the funky button
//        if (d->_episodeModel->isForm(index)) {
//            newEpisode();
//        } else {
//            // TODO: validateEpisode
////            validateEpisode();
//        }

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QTreeView *tree = d->ui->formView->treeView();
        QWidget *vp = tree->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    } else if(index.column() == FormTreeModel::Label) {
        setCurrentEditingItem(index);
    }
}

void FormPlaceHolder::addBottomWidget(QWidget *bottom)
{
    // TODO: here
//    d->m_GeneralLayout->addWidget(bottom, d->m_GeneralLayout->rowCount(), 0, 0, d->m_GeneralLayout->columnCount());
}

void FormPlaceHolder::showLastEpisodeSynthesis()
{
    qWarning() << "showMastEpisode";
    setCurrentForm(Constants::PATIENTLASTEPISODES_UUID);
}

void FormPlaceHolder::setCurrentForm(Form::FormMain *form)
{
    if (form)
        qWarning() << "FormPlaceHolder::setCurrentForm" << form->uuid();
    else
        qWarning() << "FormPlaceHolder::setCurrentForm (0x0)";
    d->ui->formDataMapper->setCurrentForm(form);
    d->ui->episodeView->setModel(formManager()->episodeModel(form));
}

void FormPlaceHolder::setCurrentForm(const QString &formUid)
{
    setCurrentForm(formManager()->form(formUid));

//    // change the stack and populate as needed
//    d->m_Stack->setCurrentIndex(d->m_StackId_FormUuid.key(formUuid));
//    if (d->m_Stack->currentWidget()) {
//        if (formUuid==Constants::PATIENTLASTEPISODES_UUID) {
//            if (!d->_episodeModel)
//                return;
//            qApp->setOverrideCursor(QCursor(Qt::BusyCursor));
//            QTextBrowser *browser = d->m_Stack->currentWidget()->findChild<QTextBrowser*>();
//            browser->setText(d->_episodeModel->lastEpisodesSynthesis());
//            qApp->restoreOverrideCursor();
//        }
//    }
}

void FormPlaceHolder::setCurrentEditingItem(const QModelIndex &index)
{
    Form::FormMain *form = d->_formTreeModel->formForIndex(index);
    d->ui->formDataMapper->setCurrentForm(form);
    d->useEpisodeModel(form);


//    if (!d) {
//        return;
//    }
//    if (!d->_episodeModel) {
//        return;
//    }

//    const QString &formUuid = d->_episodeModel->index(index.row(), FormTreeModel::FormUuid, index.parent()).data().toString();
//    if (formUuid==Constants::PATIENTLASTEPISODES_UUID && d->m_Stack->currentIndex()==0) {
//        return;
//    }
//    setCurrentForm(formUuid);
//    bool isEpisode = d->_episodeModel->isEpisode(index);
//    if (isEpisode) {
//        qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(true);
//        d->_episodeModel->activateEpisode(index, formUuid);
//    } else {
//        // TODO: code here : show HtmlSynthesis in a label
//        d->_episodeModel->activateEpisode(QModelIndex(), formUuid);
//    }
//    foreach(Form::FormMain *form, d->_rootForm->flattenFormMainChildren()) {
//        if (form->uuid()==formUuid) {
//            form->formWidget()->setEnabled(isEpisode);
//            break;
//        }
//    }
}

//void FormPlaceHolder::reset()
//{
//    d->ui->formView->update();
//    d->ui->formView->expandAll();
//    d->populateStackLayout();
//}

void FormPlaceHolder::newEpisode()
{
    if (!d->ui->formView->selectionModel())
        return;
    if (!d->ui->formView->selectionModel()->hasSelection())
        return;
    // get the parent form
//    QModelIndex index;
//    index = d->ui->formView->selectionModel()->selectedIndexes().at(0);
//    while (!d->_episodeModel->isForm(index)) {
//        index = index.parent();
//    }

//    // test the form to be unique or multiple episode
//    if (d->_episodeModel->isUniqueEpisode(index) && d->_episodeModel->rowCount(index) == 1)
//        return;
//    if (d->_episodeModel->isNoEpisode(index))
//        return;

//    // create a new episode the selected form and its children
//    if (!d->_episodeModel->insertRow(0, index)) {
//        LOG_ERROR("Unable to create new episode");
//        return;
//    }
//    // activate the newly created main episode
//    d->ui->formView->selectionModel()->clearSelection();
//    d->ui->formView->selectionModel()->setCurrentIndex(d->_episodeModel->index(0,0,index), QItemSelectionModel::Select | QItemSelectionModel::Rows);
//    const QString &formUuid = d->_episodeModel->index(index.row(), Form::FormTreeModel::FormUuid, index.parent()).data().toString();
//    setCurrentForm(formUuid);
//    qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(true);
//    d->_episodeModel->activateEpisode(d->_episodeModel->index(0,0,index), formUuid);
//    foreach(Form::FormMain *form, d->_rootForm->flattenFormMainChildren()) {
//        if (form->uuid()==formUuid) {
//            form->formWidget()->setEnabled(true);
//            break;
//        }
//    }
}

void FormPlaceHolder::removeEpisode()
{
    if (!d->ui->formView->selectionModel())
        return;
    // TODO: code here: removeEpisode
}

void FormPlaceHolder::validateEpisode()
{
    if (!d->ui->formView->selectionModel())
        return;
    // TODO: code here: validateEpisode
}

void FormPlaceHolder::addForm()
{
    if (!isVisible())
        return;
    // save current episode
//    if (d->ui->formView->selectionModel()->hasSelection()) {
//        // something to save ?
//        QModelIndex index = d->ui->formView->selectionModel()->selectedIndexes().at(0);
//        if (d->_episodeModel->isEpisode(index)) {
//            // get the parent form
//            QModelIndex formIndex = index.parent();
//            while (!d->_episodeModel->isForm(formIndex)) {
//                formIndex = formIndex.parent();
//            }
//            d->_episodeModel->saveEpisode(d->ui->formView->currentIndex(), d->_episodeModel->index(formIndex.row(), FormTreeModel::FormUuid, formIndex.parent()).data().toString());
//        }
//    }
//    // open the form editor dialog
//    FormEditorDialog dlg(d->_episodeModel, FormEditorDialog::DefaultMode, this);
//    if (dlg.exec()==QDialog::Accepted) {
//        // refresh stack widget
//        d->populateStackLayout();
//        // activate last episode synthesis
//        d->ui->formView->treeView()->setCurrentIndex(d->_episodeModel->index(0,0));
//        showLastEpisodeSynthesis();
////        d->m_Stack->setCurrentIndex(d->m_StackId_FormUuid.key(dlg.lastInsertedFormUid()));
//    }
}

void FormPlaceHolder::printCurrentItem()
{
//    qWarning() << Q_FUNC_INFO;
    if (!d->ui->formView->selectionModel())
        return;

    // get the current index
//    QTreeView *tree = d->ui->formView->treeView();
//    QModelIndex index = tree->rootIndex();
//    if (tree->selectionModel()->hasSelection())
//        index = tree->selectionModel()->selectedIndexes().at(0);
//    QModelIndex form = index;
//    while (!d->_episodeModel->isForm(form)) {
//        form = form.parent();
//    }
//    if (!form.isValid())
//        return;

//    QString htmlToPrint;
//    QString title;
//    QModelIndex formUid = d->_episodeModel->index(form.row(), Form::FormTreeModel::FormUuid, form.parent());
//    if (formUid.data().toString()==Constants::PATIENTLASTEPISODES_UUID) {
//        // Print patient synthesis
//        htmlToPrint = d->_episodeModel->lastEpisodesSynthesis();
//        title = QApplication::translate(Constants::FORM_TR_CONTEXT, Constants::PATIENTLASTEPISODES_TEXT);
//    } else {
//        // Print episode
//        if (d->_episodeModel->isEpisode(index))
//            setCurrentEpisode(index);
//        Form::FormMain *formMain = d->_episodeModel->formForIndex(form);
//        if (formMain) {
//            htmlToPrint = "<html><body>" + formMain->printableHtml(d->_episodeModel->isEpisode(index)) + "</body></html>";
//            title = formMain->spec()->label();
//        }
//    }

//    if (htmlToPrint.isEmpty())
//        return;

//    Core::IDocumentPrinter *p = printer();
//    if (!p) {
//        LOG_ERROR("No IDocumentPrinter found");
//        return;
//    }
//    p->clearTokens();
//    QHash<QString, QVariant> tokens;

//    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, title);
////    // create a token for each FormItem of the FormMain
////    foreach(FormItem *item, formMain->flattenFormItemChildren()) {
////        if (item->itemDatas())
////            tokens.insert(item->uuid(), item->itemDatas()->data(0, Form::IFormItemData::ID_Printable));
////    }
//    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

//    // print
//    p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Generic_User, false);
}

void FormPlaceHolder::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        // if showing patient synthesis or last episode -> retranslate by querying the model
//        QTreeView *tree = d->ui->formView->treeView();
//        if (tree->selectionModel()) {
//            QModelIndex index = tree->selectionModel()->currentIndex();
//            const QString &formUuid = d->_episodeModel->index(index.row(), FormTreeModel::FormUuid, index.parent()).data().toString();
//            if (formUuid==Constants::PATIENTLASTEPISODES_UUID) {
//                setCurrentForm(formUuid);
//            }
//        }
    }
    QWidget::changeEvent(event);
}
