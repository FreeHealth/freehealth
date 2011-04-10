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
 ***************************************************************************/
#include "texteditorfactory.h"

#include <formmanagerplugin/iformitem.h>

using namespace BaseWidgets;

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
    const QString &options = formItem->extraDatas().value("options");
    Editor::TextEditor::Types t = Editor::TextEditor::Simple;
    if (options.compare("FullEditor", Qt::CaseInsensitive) == 0) {
        t = Editor::TextEditor::Full;
    } else {
        if (options.compare("WithTable", Qt::CaseInsensitive) == 0) {
            t |= Editor::TextEditor::WithTables;
        }
        if (options.compare("WithIO", Qt::CaseInsensitive) == 0) {
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

void TextEditorForm::retranslate()
{
    /** \todo iformitem --> one spec per language ? */
    //     m_Label->setText( m_FormItem->spec()->label() );
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
TextEditorData::TextEditorData(Form::FormItem *item) :
        m_FormItem(item), m_Editor(0)
{}

TextEditorData::~TextEditorData()
{}

void TextEditorData::clear()
{
    setStorableData(m_FormItem->valueReferences()->defaultValue());
}

bool TextEditorData::isModified() const
{
    return m_OriginalValue != m_Editor->textEdit()->toHtml();
}

bool TextEditorData::setData(const int ref, const QVariant &data, const int role)
{
    qWarning() << "TextEditorData::setData" << data << role;
    return true;
}

QVariant TextEditorData::data(const int ref, const int role) const
{
    return m_Editor->textEdit()->toHtml();
}

void TextEditorData::setStorableData(const QVariant &data)
{
//    qWarning() << "TextEditorData::setStorableData" << data;
    if (!data.isValid()) 
        return;
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

