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
#include "iformwidgetfactory.h"
#include "iformitem.h"

/**
 * \class Form::IFormWidgetFactory
 * \brief Interface for the form's widget creation.
 * The factory allows the user to create a specific Form::IFormWidget according to their 'plugin'
 * name. Factories must be sent into the PluginManager objects pool.\n
 * The factory owns the form widget plugins that are described by the providedWidgets().\n
 * You can only use the factories in your Form::IFormIO form loader.
 *
 * Initialization of the object is done through:
 * - initialize(const QStringList &arguments, QString *errorString) first
 * - extensionInitialized() when all dependencies are initialized
 * - isInitialized() must return the initialization state
 */

/**
 * \fn virtual QStringList Form::IFormWidgetFactory::providedWidgets() const;
 * Returns a list of provided form widget plugin names (like "identity", "form"...)
 */

/**
 * \fn virtual bool Form::IFormWidgetFactory::isContainer(const int idInStringList) const;
 * Returns \e true if the form widget plugin is a container. \e idInStringList is the index of the plugin
 * in the providedWidget() list.
 */

/**
 * \fn bool Form::IFormWidgetFactory::isContainer(const QString &name)
 * Same as above with the form widget plugin name \e name.
 */

/**
 * \fn virtual Form::IFormWidget *Form::IFormWidgetFactory::createWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent = 0);
 * Returns a pointer to a newly created Form::IFormWidget.
 * \param name The name of the form widget plugin (must be included in the providedWidgets())
 * \param linkedObject as item object
 * \param parent as QWidget parent
 */

/**
 * \class Form::IFormWidget
 * \brief Represent a form item graphical element usable in any UI.
 *
 * Life cycle:
 * The widgets are only created by the Form::IFormWidgetFactory objects using the
 * Form::IFormWidgetFactory::createWidget(). The widgets are parented, so deletion is automatically
 * done when the parent widget is deleted. The form widget must be linked to a Form::FormItem.
 *
 * Content of the widget:
 * When created, the widget can contain a label and the form widget. The label is populated by the
 * Form::FormItemSpec content. You can create it using createLabel().\n
 * The widget can be a container (like a QGroupBox), see isContainer(). You can easily include
 * another Form::IFormWidget using the addWidgetToContainer().
 *
 * Translation of the content:
 * You can use the retranslate() callback function to translate your UI. This function is called when needed. You
 * don't have to worry about QEvent::LanguageChange.
 *
 * Data export:
 * The form widget owns the HTML data transformation mecanism. (This can be improved.)
 */

/**
 * \fn void Form::IFormWidget::addWidgetToContainer(IFormWidget *)
 * Add a new form widget to the container.
 */

/**
 * \fn bool Form::IFormWidget::isContainer() const
 * Return \e true if the widget is a container. If it is a container, the widget can recieve new child widgets
 * using addWidgetToContainer().
 */

/**
 * \fn void Form::IFormWidget::createLabel(const QString &text, Qt::Alignment horizAlign)
 * Automatically create the label, using the Form::FormItemSpec definition.
 */

/**
 * \fn QBoxLayout *Form::IFormWidget::getBoxLayout(const int labelOption, const QString &text, QWidget *parent)
 */

/**
 * \fn void Form::IFormWidget::setFormItem(Form::FormItem *link)
 * Define the Form::FormItem linked to this object.
 */

/**
 * \fn Form::FormItem *Form::IFormWidget::formItem()
 * Return the Form::FormItem linked to this object.
 */

/**
 * \fn QString Form::IFormWidget::printableHtml(bool withValues = true) const
 * Return the content of the form widget transformed to HTML. The HTML output should contain
 * the form widget current value if \e withValues is set to true. Otherwise, the HTML output
 * should keep an empty space for users to print and fill in values on the paper.
 * If the form widget is a container, it should return all its children's HTML output.
 */

/**
 * \fn void Form::IFormWidget::retranslate()
 * Called anytime the widget needs to be translated.
 */

/**
 * \fn QWidget *Form::IFormWidget::focusedWidget() const
 * When dynamically loading the forms (using a QtUi file), the formItemWidget is not included
 * in the view, but only its internal widget. To allow the definition of tab orders you must
 * define the widget that will get the focus (and the one that is in the view, so in the
 * QtUi file). It is also the first in the taborder
 * \sa Form::IFormWidget::setFocusedWidget()
 * \sa setLastTabWidget()
 */

/**
 * \fn void Form::IFormWidget::setFocusedWidget(QWidget *widget)
 * \sa Form::IFormWidget::focusedWidget()
 */

/**
 * \fn QWidget *Form::IFormWidget::lastTabWidget() const
 * When the widget contains multiple editors, and when the taborder is defined in the form description,
 * the last tab widget corresponds to the last widget that recieves the tabFocus.
 * By default, the widget is considered as a unique editor and the last tab widget is the
 * focusedWidget.
 * \sa setFocusedWidget(), addTabOrder()
 */

/**
 * \fn void Form::IFormWidget::setLastTabWidget(QWidget *widget)
 * \sa lastTabWidget(), addTabOrder()
 */

/**
 * \fn virtual void Form::IFormWidget::setTabOrder(bool consoleWarn)
 * If the widget contains multiple editors, overload this member and set the internal taborder.
 */

#include <QLocale>
#include <QEvent>
#include <QBoxLayout>

using namespace Form;

IFormWidget::IFormWidget(Form::FormItem *formItem, QWidget *parent) :
    QWidget(parent),
    m_Label(0),
    m_FormItem(formItem),
    m_focusedWidget(0),
    _lastTabWidget(0)
{
    Q_ASSERT(formItem);
    m_FormItem->setFormWidget(this);
    m_OldTrans = QLocale().name().left(2);
}

IFormWidget::~IFormWidget()
{
}

void IFormWidget::createLabel(const QString &text, Qt::Alignment horizAlign)
{
    m_Label = new QLabel(this);
    QLabel *l = m_Label;
    l->setFrameStyle(IFormWidget::ItemLabelFrame);
//    l->setFrameStyle(QFrame::Plain);
//    l->setFrameShape(QFrame::Box);
    l->setText(text);
    l->setAlignment(Qt::AlignTop | horizAlign);
//    l->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    l->setWordWrap(true);
}

QBoxLayout *IFormWidget::getBoxLayout(const int labelOption, const QString &text, QWidget *parent)
{
    QBoxLayout *hb;
    m_Label = new QLabel(this);
    m_Label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    if (labelOption == OnTop) {
        hb = new QBoxLayout(QBoxLayout::TopToBottom, parent);
        createLabel(text, Qt::AlignTop | Qt::AlignLeft);
        hb->setSpacing(5);
    } else {
        // TODO: Code specific label options
        hb = new QHBoxLayout(parent);
        if (labelOption != NoLabel) {
            createLabel(text, Qt::AlignTop | Qt::AlignLeft);
        }
        hb->setSpacing(5);
        hb->setMargin(5);
    }
    return hb;
}

void IFormWidget::changeEvent(QEvent *event)
{
    QString loc = QLocale().name().left(2);
    if ((event->type() == QEvent::LanguageChange) &&
        (m_OldTrans != loc) &&
            m_FormItem) {
        m_OldTrans = loc;
        retranslate();
        event->accept();
        return;
    }
    QWidget::changeEvent(event);
}

