/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "texteditorfactory.h"

#include <formmanagerplugin/iformitem.h>
#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

#include <utils/log.h>
#include <utils/global.h>

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
        l = item->parentFormMain()->formWidget()->findChild<QLabel*>(lbl);
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
        QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(layout);
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
        hb = getBoxLayout(OnTop, m_FormItem->spec()->label(), this);
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
    m_Text->textEdit()->setObjectName("TextEditor_TextEdit_" + m_FormItem->uuid());
    m_Text->setDocumentTitle(m_FormItem->spec()->label());
    hb->addWidget(m_Text);
    if (options.contains("expanded", Qt::CaseInsensitive)) {
        m_Text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    if (!options.contains("TabKeepsFocus", Qt::CaseInsensitive)) {
        m_Text->textEdit()->setTabChangesFocus(true);
    }
    if ( options.contains("PrintDuplicata", Qt::CaseInsensitive)) {
        m_Text->setAlwaysPrintDuplicata(true);
    }
    const QString &papers = formItem->extraData().value("papers");
    if (!papers.isEmpty()) {
        if (papers.compare("Generic", Qt::CaseInsensitive)==0)
            m_Text->setUserDocumentForPrintingProcess(Core::IDocumentPrinter::Papers_Generic_User);
        else if (papers.compare("Administrative", Qt::CaseInsensitive)==0)
            m_Text->setUserDocumentForPrintingProcess(Core::IDocumentPrinter::Papers_Administrative_User);
        else if (papers.compare("Prescription", Qt::CaseInsensitive)==0)
            m_Text->setUserDocumentForPrintingProcess(Core::IDocumentPrinter::Papers_Prescription_User);
    }

    setFocusedWidget(m_Text->textEdit());
    setFocusProxy(m_Text->textEdit());

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

    QString label;
    if (!m_FormItem->spec()->label().isEmpty()) {
        label = QString("<thead>"
                        "<tr>"
                        "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                        "%1"
                        "</td>"
                        "</tr>"
                        "</thead>").arg(m_FormItem->spec()->label());
    }
    if (withValues) {
        if (m_FormItem->getOptions().contains("DontPrintEmptyValues")) {
            if (m_Text->textEdit()->toPlainText().isEmpty())
                return QString();
        }
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0>"
                   "%1"
                   "<tbody>"
                   "<tr>"
                   "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                   "%2"
                   "</td>"
                   "</tr>"
                   "</tbody>"
                   "</table>")
            .arg(label).arg(Utils::htmlBodyContent(m_Text->getHtml()));
    } else {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 1em 0em 1em 0em\">"
                       "%1"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />"
                       "&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(label);
    }
    return QString();
}

void TextEditorForm::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (m_Text)
        m_Text->setToolTip(m_FormItem->spec()->tooltip());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
TextEditorData::TextEditorData(Form::FormItem *item) :
    m_FormItem(item),
    m_Editor(0),
    m_ForceModified(false)
{}

TextEditorData::~TextEditorData()
{}

void TextEditorData::clear()
{
    // Clear TextCursor
    m_Editor->clear();
    QTextCursor cursor(m_Editor->textEdit()->document());
    m_Editor->setTextCursor(cursor);

    // Reset editor content to default (if one exists)
    setStorableData(m_FormItem->valueReferences()->defaultValue());
}

bool TextEditorData::isModified() const
{
    if (m_ForceModified)
        return true;
    if (m_Editor->textEdit()->toPlainText().isEmpty() && m_OriginalValue.isEmpty())
        return false;
    return m_OriginalValue != m_Editor->textEdit()->toHtml();
}

void TextEditorData::setModified(bool modified)
{
    if (!modified) {
        if (m_Editor->textEdit()->toPlainText().isEmpty())
            m_OriginalValue.clear();
        else
            m_OriginalValue = m_Editor->textEdit()->toHtml();
    } else {
        m_ForceModified = modified;
    }
}

void TextEditorData::setReadOnly(bool readOnly)
{
    m_Editor->setReadOnly(readOnly);
}

bool TextEditorData::isReadOnly() const
{
    return m_Editor->isReadOnly();
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
    if (m_Editor->textEdit()->toPlainText().isEmpty())
        return QVariant();
    return Utils::htmlBodyContent(m_Editor->textEdit()->toHtml());
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
    m_ForceModified = false;
}

QVariant TextEditorData::storableData() const
{
    if (m_Editor->textEdit()->toPlainText().isEmpty())
        return QVariant();
    return m_Editor->textEdit()->toHtml();
}

void TextEditorData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
