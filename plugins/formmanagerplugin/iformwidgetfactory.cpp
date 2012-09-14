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
#include "iformwidgetfactory.h"
#include "iformitem.h"

/**
 * \class Form::IFormWidgetFactory
 * \brief Interface for the form's widget creation.
 * The factory allow user to create specific Form::IFormWidget according to their 'plugin'
 * name. Factories must be send in the plugins manager objects pool.\n
 * Factory owns form widget plugin that are described by the providedWidgets().\n
 * You can only use the factories in your Form::IFormIO form loader.
 *
 * Initialization of the object are done throught:
 * - initialize(const QStringList &arguments, QString *errorString) first
 * - extensionInitialized() when all dependencies are initialized
 * - isInitialized() must return the initialization state
 */

/**
 * \fn virtual QStringList Form::IFormWidgetFactory::providedWidgets() const = 0;
 * Retun the list of available form widget plugin names (like "identity", "form"...)
 */

/**
 * \fn virtual bool Form::IFormWidgetFactory::isContainer( const int idInStringList ) const = 0;
 * Return true of the form widget plugin is a container. \e idInStringList is the index of the plugin
 * in the providedWidget() list.
 */

/**
 * \fn bool Form::IFormWidgetFactory::isContainer(const QString &name)
 * Same as above with the form widget plugin name \e name.
 */

/**
 * \fn virtual Form::IFormWidget *Form::IFormWidgetFactory::createWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent = 0) = 0;
 * Create the Form::IFormWidget pointer using:
 * - \e name as form widget plugin name (name must be include in the providedWidgets()
 * - the Form::FormItem \e linkedObject as item object
 * - \e parent as QWidget parent
 */

/**
 * \class Form::IFormWidget
 * \brief Represent a form item graphical element uable in any UI.
 *
 * Life cycle:
 * The widgets are only created by the Form::IFormWidgetFactory objects using the
 * Form::IFormWidgetFactory::createWidget(). The widgets are parented, so deletion is automatically
 * done when the parent widget is deleted. The form widget must be linked to a Form::FormItem.
 *
 * Content of the widget:
 * When created, the widget can contain a label and the form widget. The label is populated by the
 * Form::FormItemSpec content. You can create it using createLabel().\n
 * The widget can be a container (like a QGroupBox), see isContainer().You can easily include
 * another Form::IFormWidget using the addWidgetToContainer().
 *
 * Translation of the content:
 * You can use the retranslate() function to translate your UI. This function is called when needed. You
 * don't have to worry about QEvent::LanguageChange.
 *
 * Data exportation:
 * The form widget owns the HTML data transformation mecanism. (This can be improved.)
 */

/**
 * \fn void Form::IFormWidget::addWidgetToContainer(IFormWidget *)
 * Add a new form widget in the container.
 */

/**
 * \fn bool Form::IFormWidget::isContainer() const
 * Return true if the widget is a container. If is container, the widget can receive new widgets
 * from the addWidgetToContainer().
 */

/**
 * \fn void Form::IFormWidget::createLabel(const QString &text, Qt::Alignment horizAlign)
 * Automatically create the label, using the Form::FormItem spec definition.
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
 * should keep an empty space for users to print and feel values on the paper.
 * If the form widget is a container, it should manage all its children HTML output.
 */

/**
 * \fn void Form::IFormWidget::retranslate()
 * Called anytime the widget needs to be translated.
 */


#include <QLocale>
#include <QEvent>
#include <QBoxLayout>

using namespace Form;

IFormWidget::IFormWidget(Form::FormItem *formItem, QWidget *parent) :
    QWidget(parent), m_FormItem(formItem)
{
    m_FormItem->setFormWidget(this);
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
    if (labelOption == Label_OnTop) {
        hb = new QBoxLayout(QBoxLayout::TopToBottom, parent);
        createLabel(text, Qt::AlignTop | Qt::AlignLeft);
        hb->setSpacing(5);
    } else {
        // TODO: Code specific label options
        hb = new QHBoxLayout(parent);
        if (labelOption != Label_NoLabel) {
            createLabel(text, Qt::AlignTop | Qt::AlignLeft);
        }
        hb->setSpacing(5);
        hb->setMargin(5);
    }
    return hb;
}

void IFormWidget::changeEvent(QEvent * event)
{
    QString loc = QLocale().name().left(2);
    if ((event->type() == QEvent::LanguageChange) &&
        (m_OldTrans != loc)) {
        m_OldTrans = loc;
        retranslate();
        event->accept();
    } else {
        QWidget::changeEvent(event);
    }
}

