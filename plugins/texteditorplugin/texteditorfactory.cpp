#include "texteditorfactory.h"

//#include <texteditorplugin/texteditor.h>

#include <formmanagerplugin/iformitem.h>

using namespace Editor;

TextEditorFactory::TextEditorFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

TextEditorFactory::~TextEditorFactory()
{}

bool TextEditorFactory::initialize(const QStringList &arguments, QString *errorString)
{
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
    return QStringList() << "texteditor" << "richtext" << "richtexteditor" << "editor";
}

bool TextEditorFactory::isContainer(const int) const
{
    return false;
}

Form::IFormWidget *TextEditorFactory::createWidget(const QString &name, Form::FormItem *linkedObject, QWidget *parent)
{
    return new TextEditorForm(linkedObject, parent);
}


TextEditorForm::TextEditorForm(Form::FormItem *linkedObject, QWidget *parent) :
        Form::IFormWidget(linkedObject,parent), m_Text(0)
{
    QHBoxLayout *hb = new QHBoxLayout(this);
    hb->setMargin(0);
    hb->setSpacing(0);
    const QString &options = linkedObject->extraDatas().value("options");
    TextEditor::Types t = TextEditor::Simple;
    if (options.compare("FullEditor", Qt::CaseInsensitive) == 0) {
        t = TextEditor::Full;
    } else {
        if (options.compare("WithTable", Qt::CaseInsensitive) == 0) {
            t |= TextEditor::WithTables;
        }
        if (options.compare("WithIO", Qt::CaseInsensitive) == 0) {
            t |= TextEditor::WithIO;
        }
    }
    m_Text = new TextEditor(this, t);
    m_Text->setObjectName("TextEditor_" + QString::number(m_LinkedObject->uuid()));
    hb->addWidget(m_Text);
    retranslate();
}

TextEditorForm::~TextEditorForm()
{
}

QVariant TextEditorForm::value() const
{
    // TODO: this */
    return "needs to implement script engine and ask it for the scripted value of the form";
}

void TextEditorForm::retranslate()
{
    // TODO: iformitem --> one spec per language ? */
    //     m_Label->setText( m_LinkedObject->spec()->label() );
}
