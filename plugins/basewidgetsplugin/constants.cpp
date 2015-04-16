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
 ***************************************************************************/
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}

namespace BaseWidgets {
namespace Constants {

Form::IFormWidget::LabelOptions labelAlignement(Form::FormItem *item, Form::IFormWidget::LabelOptions defaultValue)
{
    const QStringList &o = item->getOptions();
    if (o.contains(LABEL_ALIGN_TOP, Qt::CaseInsensitive))
        return Form::IFormWidget::OnTop;
    else if (o.contains(LABEL_ALIGN_LEFT, Qt::CaseInsensitive))
        return Form::IFormWidget::OnLeft;
    return defaultValue;
}

QStringList getCountries(Form::FormItem *item)
{
    if (!item->extraData().value(EXTRAS_COUNTRY).isEmpty())
        return item->extraData().value(EXTRAS_COUNTRY).split(";");
    return QStringList();
}

int getNumberOfColumns(Form::FormItem *item, int defaultValue)
{
    if (!item->extraData().value(EXTRAS_KEY_COLUMN).isEmpty())
        return item->extraData().value(EXTRAS_KEY_COLUMN).toInt();
    else
        return defaultValue;
}

int isCompactView(Form::FormItem *item, bool defaultValue)
{
    if (item->getOptions().contains(EXTRAS_COMPACT_VIEW, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

int isGroupCheckable(Form::FormItem *item, bool defaultValue)
{
    if (item->getOptions().contains(EXTRAS_GROUP_CHECKABLE, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

int isGroupCollapsible(Form::FormItem *item, bool defaultValue)
{
    if (item->getOptions().contains(EXTRAS_GROUP_COLLAPSIBLE, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

int isGroupExpanded(Form::FormItem *item, bool defaultValue)
{
    if (item->getOptions().contains(EXTRAS_GROUP_EXPANDED, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

int isGroupChecked(Form::FormItem *item, bool defaultValue)
{
    if (item->getOptions().contains(EXTRAS_GROUP_CHECKED, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

int isRadioHorizontalAlign(Form::FormItem *item, bool defaultValue)
{
    if (item->getOptions().contains(EXTRAS_ALIGN_HORIZONTAL, Qt::CaseInsensitive))
        return true;
    else if (item->getOptions().contains(EXTRAS_ALIGN_VERTICAL, Qt::CaseInsensitive))
        return false;
    return defaultValue;
}

QString getDateFormat(Form::FormItem *item, const QString &defaultValue)
{
    if (!item->extraData().value(DATE_EXTRAS_KEY).isEmpty()) {
        return item->extraData().value(DATE_EXTRAS_KEY);
    }
    return defaultValue;
}

bool dontPrintEmptyValues(Form::FormItem *item)
{
    return item->getOptions().contains(DONTPRINTEMPTYVALUES, Qt::CaseInsensitive);
}

bool hideHeaderOnUniqueEpisode(Form::FormItem *item)
{
    return item->getOptions().contains(HIDEHEADERONUNIQUEEPISODE, Qt::CaseInsensitive);
}

void executeOnValueChangedScript(Form::FormItem *item)
{
    if (!item->scripts()->onValueChangedScript().isEmpty())
        scriptManager()->evaluate(item->scripts()->onValueChangedScript());
}

QLabel *findLabel(Form::FormItem *item)
{
    QLabel *l = 0;
    // Find label
    const QString &lbl = item->spec()->value(Form::FormItemSpec::Spec_UiLabel).toString();
    if (!lbl.isEmpty()) {
        l = item->parentFormMain()->formWidget()->findChild<QLabel*>(lbl);
        if (l) {
            l->setText(item->spec()->label());
        } else {
            l = new QLabel(item->formWidget());
        }
    }
    return l;
}

bool hasOption(Form::FormItem *item, const QString &option)
{
    return item->getOptions().contains(option, Qt::CaseInsensitive);
}

}  // namespace Constants
}  // namespace BaseWidgets
