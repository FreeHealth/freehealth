/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * \class Form::FormEditorDialog
 * Dialog that allows users to add <b>sub-forms</b> to the current patient file
*/

#include "formeditordialog.h"
#include "formcore.h"
#include "formtreemodel.h"
#include "constants_db.h"
#include "constants_settings.h"
#include "episodebase.h"
#include "subforminsertionpoint.h"
#include "formmanager.h"
#include "iformio.h"
#include "iformitem.h"
#include "formviewdelegate.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/imainwindow.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QSortFilterProxyModel>

#include "ui_formeditordialog.h"

using namespace Form;
static inline Form::Internal::EpisodeBase *episodeBase() { return Form::Internal::EpisodeBase::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Core::IMainWindow *mainWindow() {return Core::ICore::instance()->mainWindow();}

FormEditorDialog::FormEditorDialog(FormTreeModel *model, EditionModes mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormEditorDialog),
    _formTreeModel(model)
{
    Q_UNUSED(mode);

    ui->setupUi(this);
    // Mange FormSelector options
    ui->formSelector->setIncludeLocalFiles(true);
    ui->formSelector->setExcludeGenderSpecific(true);
    // Exclude all included forms
    QStringList excludeUids;
    foreach(Form::FormMain *form, formManager().allEmptyRootForms())
        excludeUids << form->uuid();
    ui->formSelector->setExcludeFormByUid(excludeUids);
    ui->formSelector->setFormType(Form::FormFilesSelectorWidget::SubForms);

    ui->treeView->setModel(model);
    ui->treeView->expandAll();
    ui->treeView->header()->hide();
    for(int i = 0; i< FormTreeModel::MaxData; ++i)
        ui->treeView->hideColumn(i);
    ui->treeView->showColumn(FormTreeModel::Label);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setStyleSheet(Constants::FORMTREEVIEW_SHEET);
    _delegate = new Internal::FormViewDelegate(ui->treeView);
    _delegate->setFormTreeModel(_formTreeModel);
    ui->treeView->setItemDelegate(_delegate);
    ui->stackedWidget->setCurrentWidget(ui->formAdder);
    ui->currentPatient->setChecked(true);

    setWindowTitle(tr("Form Editor"));
    setWindowIcon(theme()->icon(Core::Constants::ICONFORMS));

    Utils::resizeAndCenter(this, mainWindow());
}

FormEditorDialog::~FormEditorDialog()
{
    delete ui;
}

void FormEditorDialog::on_addForm_clicked()
{
    // Get the uuid of the form receiver (where to insert the sub-form)
    QString insertTo;
    if (!ui->treeView->selectionModel()->hasSelection()) {
        bool yes = Utils::yesNoMessageBox(tr("Insert as root form?"),
                                          tr("You did not selected a form, "
                                             "do you want to add the sub-form as root form?"));
        if (!yes)
            return;
        insertTo = Constants::ROOT_FORM_TAG;
    } else {
        QModelIndex idx = ui->treeView->selectionModel()->currentIndex();
        insertTo = _formTreeModel->data(_formTreeModel->index(idx.row(), FormTreeModel::Uuid, idx.parent())).toString();
    }
    if (insertTo.isEmpty()) {
        LOG_ERROR("Trying to insert sub-form but no receiver was identified");
        return;
    }
    if (ui->formSelector->excludedFormByUid().contains(insertTo, Qt::CaseInsensitive)) {
        LOG_ERROR("Trying to insert sub-form but sub-form is already inserted in the patient file");
        return;
    }

    // Get the selected sub-forms + some checks
    QList<Form::FormIODescription *> selected = ui->formSelector->selectedForms();
    if (selected.isEmpty()) {
        LOG_ERROR("Trying to insert sub-form but sub-forms was selected");
        return;
    }

    // Prepare sub-form insertion point for each selected sub-forms
    QVector<SubFormInsertionPoint> insertions;
    for(int i=0; i < selected.count(); ++i) {
        Form::FormIODescription *insert = selected.at(i);
        SubFormInsertionPoint point(_formTreeModel->modeUid(), insertTo, insert->data(Form::FormIODescription::UuidOrAbsPath).toString());
        point.setEmitInsertionSignal(true); // inform everyone of the newly added subform
        point.setForAllPatient(ui->allPatients->isChecked());
        insertions << point;
        // Send sub-form insertion to the formmanager
        formManager().insertSubForm(point);
    }

    // Save insertions to the episode database
    episodeBase()->addSubForms(insertions);
}

void FormEditorDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
