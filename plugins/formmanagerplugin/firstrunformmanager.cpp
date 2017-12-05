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
#include "firstrunformmanager.h"
#include "formfilesselectorwidget.h"
#include "iformio.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>
#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QEvent>
#include <QGridLayout>
#include <QProgressDialog>

using namespace Form;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

FirstRunFormManagerWizardPage::FirstRunFormManagerWizardPage(QWidget *parent) :
    QWizardPage(parent),
    selector(0)//, m_Wizard(parent)
{
}

void FirstRunFormManagerWizardPage::retranslate()
{
    setTitle(tr("General patient form selection"));
    setSubTitle(tr("You can define your own patient form file, or use the default one. "
                   "All patients will have the same (central) form."));
}

void FirstRunFormManagerWizardPage::initializePage()
{
    if (!selector) {
        QProgressDialog dlg(tr("Reading available forms"), tr("Please wait"), 0, 0, parentWidget());
        dlg.setWindowModality(Qt::WindowModal);
        dlg.setMinimumDuration(100);
        dlg.show();
        dlg.setFocus();
        dlg.setValue(0);

        QGridLayout *layout = new QGridLayout(this);
        setLayout(layout);
        selector = new Form::FormFilesSelectorWidget(this);
        selector->setFormType(Form::FormFilesSelectorWidget::CompleteForms);
        selector->expandAllItems();
        selector->setSelectionType(Form::FormFilesSelectorWidget::SelectionType::Single);
        layout->addWidget(selector, 0, 0);
        adjustSize();
        selector->updateGeometry();

        // check all forms (using IFormIO::checkForUpdates)
        QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
        if (!list.isEmpty()) {
            foreach(Form::IFormIO *io, list) {
                io->checkForUpdates();
                //TODO: manage user interaction & update
            }
        }

        dlg.close();
    }
}

bool FirstRunFormManagerWizardPage::validatePage()
{
    QList<Form::FormIODescription *> sel = selector->selectedForms();
    if (sel.count() != 1) {
        LOG_ERROR("QList<Form::FormIODescription *> sel should contain 1 and only 1 element.");
        return false;
    }
    // Save the selected form in the network settings
    Form::FormIODescription * f = sel.at(0);
    settings()->setDefaultForm(f->data(Form::FormIODescription::UuidOrAbsPath).toString());
    settings()->sync();
    return true;
}

int FirstRunFormManagerWizardPage::nextId() const
{
    return Core::IFirstConfigurationPage::LastPage;
}

void FirstRunFormManagerWizardPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslate();
        break;
    default:
        break;
    }
}
