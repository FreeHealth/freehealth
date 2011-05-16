/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "firstrunformmanager.h"
#include "formfilesselectorwidget.h"

#include <QEvent>
#include <QGridLayout>

using namespace Form;
using namespace Internal;

FirstRunFormManagerWizardPage::FirstRunFormManagerWizardPage(QWidget *parent) :
        QWizardPage(parent)//, m_Wizard(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    selector = new Form::FormFilesSelectorWidget(this, Form::FormFilesSelectorWidget::CompleteForms);
    selector->expandAllItems();
    layout->addWidget(selector, 0, 0);
    selector->updateGeometry();
}

void FirstRunFormManagerWizardPage::retranslate()
{
    setTitle(tr("General patient form selection"));
    setSubTitle(tr("You can define your own patient form file, or use the default one. "
                   "Select it from here. All patients will have the same forms, but you can "
                   "add subforms anywhere in the form."));
}

bool FirstRunFormManagerWizardPage::validatePage()
{
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

