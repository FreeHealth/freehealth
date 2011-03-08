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
#include "formmanagerpreferencespage.h"
#include "formfilesselectorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QHBoxLayout>

using namespace Form;
using namespace Internal;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }


FormManagerPreferencesPage::FormManagerPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("FormManagerPreferencesPage"); }

FormManagerPreferencesPage::~FormManagerPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString FormManagerPreferencesPage::id() const { return objectName(); }
QString FormManagerPreferencesPage::name() const { return tr("Selector"); }
QString FormManagerPreferencesPage::category() const { return tr("Forms"); }

void FormManagerPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void FormManagerPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void FormManagerPreferencesPage::finish() { delete m_Widget; }

void FormManagerPreferencesPage::checkSettingsValidity()
{
//    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
//    defaultvalues.insert(Core::Constants::S_USE_EXTERNAL_DATABASE, false);
//
//    foreach(const QString &k, defaultvalues.keys()) {
//        if (settings()->value(k) == QVariant())
//            settings()->setValue(k, defaultvalues.value(k));
//    }
//    settings()->sync();
}

QWidget *FormManagerPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new FormManagerPreferencesWidget(parent);
    return m_Widget;
}


FormManagerPreferencesWidget::FormManagerPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    m_Selector = new Form::FormFilesSelectorWidget(this);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(m_Selector);
}

void FormManagerPreferencesWidget::saveToSettings(Core::ISettings *)
{}

void FormManagerPreferencesWidget::changeEvent(QEvent *e)
{}
