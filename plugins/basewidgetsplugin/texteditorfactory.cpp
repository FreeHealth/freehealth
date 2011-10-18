/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "texteditorfactory.h"

#include <formmanagerplugin/iformitem.h>

#include <utils/log.h>

using namespace BaseWidgets;

inline static QStringList getOptions(Form::FormItem *item)
{
    QStringList l;
    l = item->extraDatas().value("options").split(";", QString::SkipEmptyParts);
    l += item->extraDatas().value("option").split(";", QString::SkipEmptyParts);
    return l;
}

TextEditorFactory::TextEditorFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

TextEditorFactory::~TextEditorFactory()
{}

bool TextEditorFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool TextEditorFactory::extensionInitialized()
{
    return true;
}

bool TextEditorFactory::isInitialized() const
{
    return true;
}

QStringList TextEditorFactory::providedWidgets() const
{
    return QStringList() << "texteditor" << "richtext" << "richtexteditor" << "editor" << "html";
}

bool TextEditorFactory::isContainer(const int) const
{
    return false;
}

Form::IFormWidget *TextEditorFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    return new TextEditorForm(formItem, parent);
}


TextEditorForm::TextEditorForm(Form::FormItem *formItem, QWidget *parent) :
        Form::IFormWidget(formItem,parent), m_Text(0)
{
    QBoxLayout *hb = getBoxLayout(Label_OnTop, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);
    hb->setMargin(0);
    hb->setSpacing(0);
    const QStringList &options = getOptions(formItem);
    Editor::TextEditor::Types t = Editor::TextEditor::Simple | Editor::TextEditor::WithTextCompleter;
    if (options.contains("FullEditor", Qt::CaseInsensitive)) {
        t = Editor::TextEditor::Full;
    } else {
        if (options.contains("WithTable", Qt::CaseInsensitive)) {
            t |= Editor::TextEditor::WithTables;
        }
        if (options.contains("WithIO", Qt::CaseInsensitive)) {
            t |= Editor::TextEditor::WithIO;
        }
    }
    m_Text = new Editor::TextEditor(this, t);
    m_Text->setObjectName("TextEditor_" + m_FormItem->uuid());
    hb->addWidget(m_Text);

    // create item data
    TextEditorData *data = new TextEditorData(formItem);
    data->setEditor(m_Text);
    formItem->setItemDatas(data);

    retranslate();
}

TextEditorForm::~TextEditorForm()
{
}

QString TextEditorForm::printableHtml(bool withValues) const
{
    if (withValues) {
        if (getOptions(m_FormItem).contains("DontPrintEmptyValues")) {
            if (m_Text->textEdit()->toPlainText().isEmpty())
                return QString();
        }
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0>"
                   "<thead>"
                   "<tr>"
                   "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                    "%1"
                   "</td>"
                   "</tr>"
                   "</thead>"
                   "<tbody>"
                   "<tr>"
                   "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                   "%2"
                   "</td>"
                   "</tr>"
                   "</tbody>"
                   "</table>")
            .arg(m_FormItem->spec()->label()).arg(m_Text->getHtml().remove("</body>").remove("</html>"));
    } else {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 1em 0em 1em 0em\">"
                       "<thead>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "</tr>"
                       "</thead>"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />"
                       "&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    }
    return QString();
}

void TextEditorForm::retranslate()
{
    m_Label->setText( m_FormItem->spec()->label() );
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
TextEditorData::TextEditorData(Form::FormItem *item) :
        m_FormItem(item), m_Editor(0)
{}

TextEditorData::~TextEditorData()
{}

void TextEditorData::clear()
{
//    WARN_FUNC;
    setStorableData(m_FormItem->valueReferences()->defaultValue());
}

bool TextEditorData::isModified() const
{
    return m_OriginalValue != m_Editor->textEdit()->toHtml();
}

bool TextEditorData::setData(const int ref, const QVariant &data, const int role)
{
//    qWarning() << "TextEditorData::setData" << data << role;
    return true;
}

QVariant TextEditorData::data(const int ref, const int role) const
{
    return m_Editor->textEdit()->toHtml();
}

void TextEditorData::setStorableData(const QVariant &data)
{
//    qWarning() << "TextEditorData::setStorableData" << data;
    if (data.isNull() || (data.toString().size()==1 && data.toString() == "0")) {
        m_OriginalValue.clear();
        m_Editor->textEdit()->clear();
    } else {
        m_OriginalValue = data.toString();
        m_Editor->setHtml(m_OriginalValue);
    }
}

QVariant TextEditorData::storableData() const
{
    return m_Editor->textEdit()->toHtml();
}

