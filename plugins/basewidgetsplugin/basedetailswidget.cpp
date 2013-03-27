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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "basedetailswidget.h"
#include "constants.h"

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/detailswidget.h>

//#include <translationutils/constants.h>
//#include <translationutils/trans_menu.h>
//#include <translationutils/trans_filepathxml.h>

#include <QUiLoader>
#include <QBuffer>

using namespace BaseWidgets;
using namespace Internal;

BaseDetailsWidget::BaseDetailsWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent),
    _detailsWidget(0),
    numberColumns(1),
    i(0),
    col(0),
    row(0)
{
    setObjectName("BaseDetailsWidge_" + m_FormItem->uuid());

    // Create the detailsWidget
    _detailsWidget = new Utils::DetailsWidget(this);
    _detailsWidget->setSummaryText(formItem->spec()->label());

    // Create detailsWidget content
    QWidget *content = 0;
    const QString &uiContent = formItem->spec()->value(Form::FormItemSpec::Spec_UiFileContent).toString();
    if (!uiContent.isEmpty()) {
        // Create the Ui using the QtUi file
        QUiLoader loader;
        QBuffer buf;
        buf.setData(uiContent.toUtf8());
        content = loader.load(&buf, _detailsWidget);

        // Manage options
    } else {
        // TODO: manage error
        LOG_ERROR("Ui file not found: " + formItem->spec()->value(Form::FormItemSpec::Spec_UiFileContent).toString());
    }

    // manage options
    _detailsWidget->setWidget(content);
    if (Constants::hasOption(formItem, Constants::SUMMARY_FONT_BOLD))
        _detailsWidget->setSummaryFontBold(true);
    if (Constants::hasOption(formItem, Constants::EXTRAS_GROUP_EXPANDED))
        _detailsWidget->setState(Utils::DetailsWidget::Expanded);

    // Create the global layout
    const QString &uiLayout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!uiLayout.isEmpty()) {
        QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(uiLayout);
        if (lay) {
            lay->addWidget(_detailsWidget);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
        }
    } else {
        QVBoxLayout *layout = new QVBoxLayout(this);
        setLayout(layout);
        //    layout->setMargin(0);
        //    layout->setSpacing(0);
        layout->addWidget(_detailsWidget);
    }

    // create itemdata
//    BaseFormData *baseFormData = new BaseFormData(formItem);
//    baseFormData->setForm(this);
//    formItem->setItemData(baseFormData);

    retranslate();

}

BaseDetailsWidget::~BaseDetailsWidget()
{
}

void BaseDetailsWidget::addWidgetToContainer(Form::IFormWidget *widget)
{
//    qWarning() << "details add" << widget;
//    if (!widget)
//        return;
//    if (!_detailsWidget->widget() || !_detailsWidget->widget()->layout())
//        return;
//    // it is not possible to add a form inside a container
//    if (widget->formItem()->spec()->pluginName() == "form") // TODO: remove magic number
//        return;
//    // TODO: manage grid / box layouts
////    col = (i % numberColumns);
////    row = (i / numberColumns);
//    _detailsWidget->widget()->layout()->addWidget(widget);//, row, col);
//    qWarning() << "    added";
//    i++;
}

bool BaseDetailsWidget::isContainer() const
{
    return true;
}

// Printing
QString BaseDetailsWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    QStringList html;
    QList<Form::FormItem*> items = m_FormItem->formItemChildren();
    for(int i = 0; i < items.count(); ++i) {
        Form::IFormWidget *w = items.at(i)->formWidget();
        if (w)
            html << w->printableHtml(withValues);
    }
    html.removeAll("");

    if (html.isEmpty() && Constants::dontPrintEmptyValues(m_FormItem))
        return QString();

    int i = 0;
//    int c = 0;
    int r = 0;
    int previousrow = 0;
    QString header, content;
    // Start with the header of the form
    header += QString("<table width=100% border=2 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
                    "<thead>"
                    "<tr>"
                    "<td style=\"vertical-align: top;padding: 5px\">"
                    "<center><span style=\"font-weight: 600;\">%1</span><br />"
                    "%2"
                      "</center>"
                    "</td>"
                    "</tr>"
                    "</thead>"
                    "</table>")
            .arg(m_FormItem->spec()->label())
            .arg("LABEL");

    // recreate the grid as an html table
    int numberColumns = 1;
    foreach(const QString &s, html) {
//        c = (i % numberColumns);
        r = (i / numberColumns);
        if (r>previousrow) {
            previousrow = r;
            content += "</tr><tr>";
        }
        content += QString("<td style=\"vertical-align: top; align: left\">"
                           "%1"
                           "</td>").arg(s);
        ++i;
    }

    return QString("%1"
                   "<table width=100% border=0 cellpadding=0 cellspacing=0 style=\"margin:0px\">"
                   "<tbody>"
                   "<tr>"
                   "%2"
                   "</tr>"
                   "</tbody>"
                   "</table>")
            .arg(header).arg(content);
}

void BaseDetailsWidget::retranslate()
{
    Q_ASSERT(_detailsWidget);
    // TODO: manage extralabel
//    QString extra = m_FormItem->spec()->extraLabel();
//    foreach(Form::FormItem *item, m_FormItem->flattenFormItemChildren()) {
//        if (item->itemData())
//            extra.replace(QString("{%1}").arg(item->uuid()), item->itemData()->data(0).toString());
//        else
//            extra.remove(item->uuid());
//    }

//    if (extra.isEmpty())
        _detailsWidget->setSummaryText(m_FormItem->spec()->label());
//    else
//        _detailsWidget->setSummaryText(m_FormItem->spec()->label() + " - " + extra);
}
