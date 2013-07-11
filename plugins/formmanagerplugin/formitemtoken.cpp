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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Form::Internal::FormItemToken
 * \brief Wrapper over Core::IToken specialized for IFormItem & IFormItemData.
 *
 * Creates multiple tokens to access the formitem values like:
 * - label (translated to the current language)
 * - tooltip (translated to the current language)
 * - htmldescription (translated to the current language)
 */

#include "formitemtoken.h"
#include "constants_db.h"
#include "iformitemdata.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QTextDocument>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Form {
namespace Internal {
class FormItemTokenPrivate
{
public:
    FormItemTokenPrivate(FormItemToken *parent) :
        _item(0),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~FormItemTokenPrivate()
    {
    }

    // Format token uuid according to its ValueType representation
    void formatTokenUid()
    {
        _uid = _item->uuid();
        switch (_valueType) {
        case FormItemToken::FormItemLabel:
            _uid.prepend(QString("%1.%2.")
                         .arg(Constants::TOKENFORM_NAMESPACE)
                         .arg(Constants::TOKENFORM_LABEL_NAMESPACE));
            break;
        case FormItemToken::FormItemTooltip:
            _uid.prepend(QString("%1.%2.")
                         .arg(Constants::TOKENFORM_NAMESPACE)
                         .arg(Constants::TOKENFORM_TOOLTIP_NAMESPACE));
            break;
        case FormItemToken::FormItemPatientModelValue:
            _uid.prepend(QString("%1.%2.%3.")
                         .arg(Constants::TOKENFORM_NAMESPACE)
                         .arg(Constants::TOKENFORM_DATA_NAMESPACE)
                         .arg(Constants::TOKENFORM_PATIENTDATA_NAMESPACE));
            break;
        case FormItemToken::FormItemPrintValue:
            _uid.prepend(QString("%1.%2.%3.")
                         .arg(Constants::TOKENFORM_NAMESPACE)
                         .arg(Constants::TOKENFORM_DATA_NAMESPACE)
                         .arg(Constants::TOKENFORM_PRINTDATA_NAMESPACE));
            break;
        case FormItemToken::FormItemDataValue:
            _uid.prepend(QString("%1.%2.%3.")
                         .arg(Constants::TOKENFORM_NAMESPACE)
                         .arg(Constants::TOKENFORM_DATA_NAMESPACE)
                         .arg(Constants::TOKENFORM_ITEMDATA_NAMESPACE));
            break;
        default: LOG_ERROR_FOR("FormItemToken", "No namespace for this ValueType. Please check the code.");
        } // switch
    }
    
public:
    Form::FormItem *_item;
    int _valueType;
    QString _uid;

private:
    FormItemToken *q;
};
} // namespace Internal
} // end namespace Form

/** Return true if the Form::FormItem can manage a specific value type */
bool FormItemToken::canManageValueType(Form::FormItem *item, ValueType type)
{
    if (!item)
        return false;
    switch (type) {
    case FormItemLabel:
    case FormItemTooltip:
        return (item->spec() != 0);
    case FormItemPatientModelValue:
    case FormItemPrintValue:
    case FormItemDataValue:
        return (item->itemData() != 0);
    } // switch
    return false;
}

/*! Constructor of the Form::Internal::FormItemToken class */
FormItemToken::FormItemToken(Form::FormItem *item, ValueType type) :
    Core::IToken(QString::null),
    d(new FormItemTokenPrivate(this))
{
    d->_item = item;
    d->_valueType = type;
    d->formatTokenUid();
}

/*! Destructor of the Form::Internal::FormItemToken class */
FormItemToken::~FormItemToken()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormItemToken::initialize()
{
    return true;
}

QString FormItemToken::uid() const
{
    return d->_uid;
}

QString FormItemToken::humanReadableName() const
{
    return d->_item->spec()->label();
}

QString FormItemToken::tooltip() const
{
    return d->_item->spec()->tooltip();
}

QString FormItemToken::helpText() const
{
    return d->_item->spec()->tooltip();
}

QString FormItemToken::shortHtmlDescription() const
{
    return d->_item->spec()->description();
}

QVariant FormItemToken::testValue() const
{
    return QString("%1(TestValue)").arg(d->_uid);
}

QVariant FormItemToken::value() const
{
    switch (d->_valueType) {
    case FormItemLabel:
        return d->_item->spec()->label();
    case FormItemTooltip:
        return d->_item->spec()->tooltip();
    case FormItemPatientModelValue:
        if (!d->_item->itemData())
            return QVariant();
        return d->_item->itemData()->data(0, Form::IFormItemData::PatientModelRole);
    case FormItemPrintValue:
    {
        if (!d->_item->itemData())
            return QVariant();
        QString value = d->_item->itemData()->data(0, Form::IFormItemData::PrintRole).toString();
        if (Qt::mightBeRichText(value)) {
            // Improve the HTML output
            value = Utils::htmlReplaceAccents(value);
            if (value.contains("<body") && value.contains("</body>")) {
                QString css = Utils::htmlTakeAllCssContent(value);
                value = Utils::htmlBodyContent(value);
                value.prepend(css);
            }
            value = Utils::htmlReplaceParagraphWithDiv(value);
        }
        return value;
    }
    case FormItemDataValue:
        if (!d->_item->itemData())
            return QVariant();
        return d->_item->itemData()->data(0, 0);
    } // switch
    return QVariant();
}
