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
 *   Main developers : Eric Maeker                                *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class BaseWidgets::Internal::ButtonMenuPathItemView
 */

#include "buttonmenupathitemview.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <formmanagerplugin/iformitem.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/qmenuitemview.h>

#include <translationutils/constants.h>

#include <QFileSystemModel>

#include <QDebug>

using namespace BaseWidgets;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

/*! Constructor of the BaseWidgets::Internal:ButtonMenuPathItemView class */
ButtonMenuPathItemView::ButtonMenuPathItemView(Form::FormItem *formItem, QWidget *parent)
    : Form::IFormWidget(formItem, parent),
      _menu(0),
      _button(0),
      _model(0)
{
    setObjectName("MenuPathItemView");
    _menu = new Utils::QMenuItemView(this);

    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QToolButton *w = formItem->parentFormMain()->formWidget()->findChild<QToolButton*>(widget);
        if (w) {
            _button = w;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            _button = new QToolButton(this);
        }
        // No Label
    } else {
        // Prepare Widget Layout
        QHBoxLayout *hb = new QHBoxLayout(this);
        hb->addStretch();
        _button = new QToolButton(this);
        _button->setObjectName("ButtonMenuPathItemView_" + m_FormItem->uuid());
        _button->setSizePolicy(QSizePolicy::Preferred , QSizePolicy::Fixed);
        hb->addWidget(_button);
    }

    _model = new QFileSystemModel(this);
    // Get model the path
    QString path = formItem->extraData().value("path");
    if (path.startsWith(Core::Constants::TAG_USER_DOCUMENT_PATH)) {
        path = path.replace(Core::Constants::TAG_USER_DOCUMENT_PATH, settings()->path(Core::ISettings::UserDocumentsPath));
    }

    // Get model filter
    QString filter = formItem->extraData().value("filter");
    _model->setRootPath(path);
    if (!filter.isEmpty())
        _model->setNameFilters(filter.split(";"));

    // Manage button
    _button->setPopupMode(QToolButton::InstantPopup);
    _button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Button icon option
    QString icon = formItem->spec()->value(Form::FormItemSpec::Spec_IconFileName).toString();
    if (!icon.isEmpty()) {
        if (icon.startsWith(Core::Constants::TAG_APPLICATION_THEME_PATH, Qt::CaseInsensitive)) {
            icon = icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
        }
        _button->setIcon(QIcon(icon));
    }
    // Button label and tooltip
    _button->setText(m_FormItem->spec()->label());
    _button->setToolTip(m_FormItem->spec()->tooltip());

    // Manage menu <- model
    _menu->setModel(_model);
    _menu->setRootIndex(_model->index(path));
    _button->setMenu(_menu);

    setFocusedWidget(_button);

    connect(_menu, SIGNAL(triggered(QModelIndex)) , this , SLOT(onIndexTriggered(QModelIndex)));
}

/*! Destructor of the BaseWidgets::Internal:ButtonMenuPathItemView class */
ButtonMenuPathItemView::~ButtonMenuPathItemView()
{
}

void ButtonMenuPathItemView::addWidgetToContainer(Form::IFormWidget *widget)
{
    Q_UNUSED(widget);
}

bool ButtonMenuPathItemView::isContainer() const
{
    return false;
}

QString ButtonMenuPathItemView::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    return QString::null;
}

void ButtonMenuPathItemView::retranslate()
{
    if (_button) {
        _button->setText(m_FormItem->spec()->label());
        _button->setToolTip(m_FormItem->spec()->tooltip());
    }
}

void ButtonMenuPathItemView::onIndexTriggered(const QModelIndex &index)
{
    // TODO: 1) Use the onClicked script  2) if no script -> use populate tag
    //    if (!m_FormItem->scripts()->onClicked().isEmpty())
    //        scriptManager()->evaluate(m_FormItem->scripts()->onClicked());

    // Populate the item with the file content
    QString uid = m_FormItem->extraData().value("populate");
    if (uid.isEmpty()) {
        LOG_ERROR("Populate tag is empty");
        return;
    }
    // Find the item with defined uid
    Form::FormMain *parent = m_FormItem->parentFormMain();
    foreach(Form::FormItem *item, parent->flattenedFormItemChildren()) {
        if (item->uuid().compare(uid, Qt::CaseInsensitive) == 0) {
            if (!item->itemData()) {
                LOG_ERROR("No item data");
                return;
            }
            if (!m_FormItem->getOptions().contains("WithoutConfirmation", Qt::CaseInsensitive)) {
                bool yes = Utils::yesNoMessageBox(tr("Replace content"),
                                                  tr("<b>You are about to replace the content of the form item %1 "
                                                     "with the content of the file %2.</b>\n"
                                                     "Are you really sure you want to replace its content?")
                                                  .arg(item->spec()->label())
                                                  .arg(_model->fileName(index)));
                if (!yes)
                    return;
            }
            // Read file
            QString content = Utils::readTextFile(_model->filePath(index), Utils::DontWarnUser);
            // Process tokens
            patient()->replaceTokens(content);
            user()->replaceTokens(content);
#ifdef WITH_PAD
            content = padTools()->processPlainText(content);
#endif
            item->itemData()->setData(0, content);
            return;
        }
    }
}
