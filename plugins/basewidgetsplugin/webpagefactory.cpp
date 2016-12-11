/***************************************************************************
 *  Original work:                                                         *
 *  The FreeMedForms project www.freemedforms.com                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *                                                                         *
 *  Modified work:                                                         *
 *  FreeHealth EHR                                                         *
 *  Copyright 2016 Jerome Pinguet                                          *
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
 *  Authors:                                                               *
 *  Eric MAEKER, <eric.maeker@gmail.com>                                   *
 *  Jerome Pinguet, <jerome@jerome.cc                                      *
 *  ...                                                                    *
 ***************************************************************************/

#include "webpagefactory.h"
#include <formmanagerplugin/iformitem.h>
#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <QGridLayout>
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>

using namespace BaseWidgets;

static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}

static inline void executeOnValueChangedScript(Form::FormItem *item)
{
    if (!item->scripts()->onValueChangedScript().isEmpty())
        scriptManager()->evaluate(item->scripts()->onValueChangedScript());
}


/*
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
*/

WebPageFactory::WebPageFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

WebPageFactory::~WebPageFactory()
{}

bool WebPageFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool WebPageFactory::extensionInitialized()
{
    return true;
}

bool WebPageFactory::isInitialized() const
{
    return true;
}

QStringList WebPageFactory::providedWidgets() const
{
    return QStringList() << "webpage";
}

bool WebPageFactory::isContainer(const int) const
{
    return false;
}

Form::IFormWidget *WebPageFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    Q_UNUSED(name);
    return new WebPageForm(formItem, parent);
}

WebPageForm::WebPageForm(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Web(0), m_WebEngineView(0)
{
    QWebEngineView *wev = 0;
    // QtUi Loaded ?
    const QString &widgetName = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widgetName.isEmpty()) {
        // Find layout
        QWebEngineView *w = formItem->parentFormMain()->formWidget()->findChild<QWebEngineView*>(widgetName);
        if (w) {
            wev = w;
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            wev = new QWebEngineView(this);
        }
    } else {
        wev = new QWebEngineView(this);
        //wev->setMargin(0);
        //wev->setSpacing(0);
    }
    //const QStringList &options = formItem->getOptions();
    m_Web = new WebPage(this);
    m_Web->setObjectName("WebPage_" + m_FormItem->uuid());
    m_WebEngineView = wev;
    m_WebEngineView->setPage(m_Web);
    //layout->addWidget(m_WebEngineView);



    setFocusedWidget(m_WebEngineView);
    //setFocusProxy(0);

    // create item data
    WebPageData *data = new WebPageData(formItem);
    data->setWebPage(m_Web);
    formItem->setItemData(data);

    //connect(m_Text->textEdit(), SIGNAL(textChanged()), data, SLOT(onValueChanged()));
    retranslate();
    m_Web->setHtml("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Title of the document</title></head><body>Content of the document......</body></html>");
}

WebPageForm::~WebPageForm()
{
}

QString WebPageForm::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    //connect(this, SIGNAL(html(QString)), this, SLOT(handleHtml(QString)));
    //m_Web->toHtml([this](const QString& result) mutable {emit html(result);});
    return QString();
}

void WebPageForm::retranslate()
{
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
WebPageData::WebPageData(Form::FormItem *item) :
    m_FormItem(item),
    m_Page(0),
    m_ForceModified(false)
{}

WebPageData::~WebPageData()
{}

void WebPageData::clear()
{
    //QString emptyHtml("<!doctype html><title>Empty</title>");
    //m_Page->setHtml(emptyHtml);
    // Reset content to default (if one exists)
    //setStorableData(m_FormItem->valueReferences()->defaultValue());
}

bool WebPageData::isModified() const
{
    /*if (m_ForceModified)
        return true;
    if (m_Page->toHtml( ->page()->toHtml().isEmpty() && m_OriginalValue.isEmpty())
        return false;
    return m_OriginalValue != m_Page->page()->toHtml();
    */
    return true;
}

void WebPageData::setModified(bool modified)
{
    Q_UNUSED(modified);
    /*if (!modified) {
        if (m_Editor->textEdit()->toPlainText().isEmpty())
            m_OriginalValue.clear();
        else
            m_OriginalValue = m_Editor->textEdit()->toHtml();
    } else {
        m_ForceModified = modified;
    }*/
}

void WebPageData::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
    //m_Web->setReadOnly(readOnly);
}

bool WebPageData::isReadOnly() const
{
    //return m_Page->isReadOnly();
    return false;
}

bool WebPageData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (role==Qt::EditRole) {
        m_Page->setHtml(data.toString());
        onValueChanged();
    }
    return true;
}

QVariant WebPageData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    //if (m_Editor->textEdit()->toPlainText().isEmpty())
        return QVariant();
    //return Utils::htmlBodyContent(m_Editor->textEdit()->toHtml());
}

void WebPageData::setStorableData(const QVariant &data)
{
    Q_UNUSED(data);
    /*
    if (data.isNull() || (data.toString().size()==1 && data.toString() == "0")) {
        m_OriginalValue.clear();
        m_Editor->textEdit()->clear();
    } else {
        m_OriginalValue = data.toString();
        m_Editor->setHtml(m_OriginalValue);
    }
    m_ForceModified = false;
    */
}

QVariant WebPageData::storableData() const
{
    //if (m_Editor->textEdit()->toPlainText().isEmpty())
        return QVariant();
    //return m_Editor->textEdit()->toHtml();
}

void WebPageData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
