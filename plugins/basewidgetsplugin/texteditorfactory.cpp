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
 ***************************************************************************/
#include "texteditorfactory.h"

#include <formmanagerplugin/iformitem.h>
#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

#include <utils/log.h>

using namespace BaseWidgets;

static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}

static inline void executeOnValueChangedScript(Form::FormItem *item)
{
    if (!item->scripts()->onValueChangedScript().isEmpty())
        scriptManager()->evaluate(item->scripts()->onValueChangedScript());
}

static inline QLabel *findLabel(Form::FormItem *item)
{
    QLabel *l = 0;
    // Find label
    const QString &lbl = item->spec()->value(Form::FormItemSpec::Spec_UiLabel).toString();
    if (!lbl.isEmpty()) {
        l = qFindChild<QLabel*>(item->parentFormMain()->formWidget(), lbl);
        if (l) {
            l->setText(item->spec()->label());
        } else {
            LOG_ERROR_FOR("TextEditorFactory", "Label not found");
            l = new QLabel(item->formWidget());
            l->setText(item->spec()->label());
        }
    }
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
    Q_UNUSED(name);
    return new TextEditorForm(formItem, parent);
}


TextEditorForm::TextEditorForm(Form::FormItem *formItem, QWidget *parent) :
        Form::IFormWidget(formItem,parent), m_Text(0)
{
    QLayout *hb = 0;
    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find layout
        QLayout *lay = qFindChild<QLayout*>(formItem->parentFormMain()->formWidget(), layout);
        if (lay) {
            hb = lay;
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            hb = new QHBoxLayout(this);
        }
        // Find Label
        m_Label = findLabel(formItem);
    } else {
        hb = getBoxLayout(Label_OnTop, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);
        hb->setMargin(0);
        hb->setSpacing(0);
    }
    const QStringList &options = formItem->getOptions();
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
    if (options.contains("expanded", Qt::CaseInsensitive)) {
        // TODO: add a filterEvent and resize the textEditor to the maximum of the parent's scroolbar content.
        m_Text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_Text->setMinimumHeight(1000);
    }
    if (!options.contains("TabKeepsFocus", Qt::CaseInsensitive)) {
        m_Text->textEdit()->setTabChangesFocus(true);
    }

    // create item data
    TextEditorData *data = new TextEditorData(formItem);
    data->setEditor(m_Text);
    formItem->setItemData(data);

    connect(m_Text->textEdit(), SIGNAL(textChanged()), data, SLOT(onValueChanged()));
    retranslate();
}

TextEditorForm::~TextEditorForm()
{
}

QString TextEditorForm::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains("notprintable"))
        return QString();

    if (withValues) {
        if (m_FormItem->getOptions().contains("DontPrintEmptyValues")) {
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
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
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

void TextEditorData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_Editor->textEdit()->toHtml();
}

bool TextEditorData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
//    qWarning() << "TextEditorData::setData" << data << role;
    if (role==Qt::EditRole) {
        m_Editor->textEdit()->setHtml(data.toString());
        onValueChanged();
    }
    return true;
}

QVariant TextEditorData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
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

void TextEditorData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
