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
#include "calculationwidgets.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iscriptmanager.h>

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QTextDocument>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

#include <QDebug>

using namespace BaseWidgets;
using namespace Internal;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}

namespace {

const char * const  LABEL_ALIGN_TOP   = "labelontop";
const char * const  LABEL_ALIGN_LEFT  = "labelonleft";

const char * const  CHANGE_EPISODE_LABEL    = "changeepisodelabel";
const char * const  SUM_EXTRA_KEY           = "sumof";
const char * const  SUM_REGEXP_EXTRA_KEY    = "sumof_regexp";
const char * const  DONTPRINTEMPTYVALUES    = "DontPrintEmptyValues";
const char * const  SCRIPT_EXTRA_KEY        = "calcScript";
const char * const  CONNECT_EXTRA_KEY       = "connect";
const char * const  CONNECT_REGEXP_EXTRA_KEY= "connect_regexp";
//const char * const  SCRIPT_NS_EXTRA_KEY     = "calcUseNS";

// Options
const char * const  NOT_PRINTABLE           = "notprintable";
const char * const  SHOW_IN_TEXTEDITOR      = "showintexteditor";

enum ProvidedWidget {
    Type_Sum = 0,
    Type_Script
};

}  // End namespace Anonymous

static inline Form::IFormWidget::LabelOptions labelAlignement(Form::FormItem *item, Form::IFormWidget::LabelOptions defaultValue = Form::IFormWidget::OnLeft)
{
    const QStringList &o = item->getOptions();
    if (o.contains(::LABEL_ALIGN_TOP, Qt::CaseInsensitive))
        return Form::IFormWidget::OnTop;
    else if (o.contains(::LABEL_ALIGN_LEFT, Qt::CaseInsensitive))
        return Form::IFormWidget::OnLeft;
    return defaultValue;
}

static inline bool dontPrintEmptyValues(Form::FormItem *item)
{
    return item->getOptions().contains(::DONTPRINTEMPTYVALUES, Qt::CaseInsensitive);
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
            l = new QLabel(item->formWidget());
            l->setText(item->spec()->label());
        }
    }
    return l;
}

CalculationWidgetsFactory::CalculationWidgetsFactory(QObject *parent) :
    IFormWidgetFactory(parent)
{
    // TODO: in all the widgets, manage the readonly state.
}

CalculationWidgetsFactory::~CalculationWidgetsFactory()
{}

bool CalculationWidgetsFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool CalculationWidgetsFactory::extensionInitialized()
{
    return true;
}

bool CalculationWidgetsFactory::isInitialized() const
{
    return true;
}

QStringList CalculationWidgetsFactory::providedWidgets() const
{
    // List must be sync with the enum
    return QStringList() << "sum" << "scriptcalculation";
}

bool CalculationWidgetsFactory::isContainer(const int idInStringList) const
{
    Q_UNUSED(idInStringList);
    return false;
}

Form::IFormWidget *CalculationWidgetsFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    int id = providedWidgets().indexOf(name);
    if (id == -1)
        return 0;
    switch (id)
    {
    case ::Type_Sum : return new SumWidget(formItem,parent);
    case ::Type_Script : return new ScriptWidget(formItem,parent);
    default: return 0;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static void addResultToEpisodeLabel(Form::FormMain *parent, QLabel *label, const QVariant &result)
{
    if (!label)
        return;
    if (!parent)
        return;
    QString episodeLabel = parent->itemData()->data(0, Form::IFormItemData::ID_EpisodeLabel).toString();
    QTextDocument doc;
    doc.setHtml(label->text());
    QString add = QString("[[%1 %2]]").arg(doc.toPlainText()).arg(result.toString());
    if (episodeLabel.contains("[[") && episodeLabel.contains("]]")) {
        // remove this part
        int begin = episodeLabel.indexOf("[[");
        int end = episodeLabel.indexOf("]]") + 2;
        episodeLabel.remove(begin, end-begin);
        // add the new value
        episodeLabel.insert(begin, add);
        parent->itemData()->setData(Form::IFormItemData::ID_EpisodeLabel, episodeLabel);
    } else {
        QString tmp = episodeLabel + "; " + add;
        parent->itemData()->setData(Form::IFormItemData::ID_EpisodeLabel, tmp);
    }
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ SumWidget ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
SumWidget::SumWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent),
    line(0)
{
    setObjectName("SumWidget_"+formItem->uuid());
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QLineEdit *le = formItem->parentFormMain()->formWidget()->findChild<QLineEdit*>(widget);
        if (le) {
            line = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            line = new QLineEdit(this);
        }
        m_Label = findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout *hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add LineEdit for the result
        line = new QLineEdit(this);
        line->setObjectName("SumWidgetLineEdit_" + m_FormItem->uuid());
        line->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        hb->addWidget(line);
    }
    retranslate();

    setFocusedWidget(line);

    // connect to parent FormMain
    Form::FormMain *p = formItem->parentFormMain();
    if (p) {
//        qWarning() << "parent" << p->uuid();
        connect(p, SIGNAL(formLoaded()), this, SLOT(connectFormItems()));
    }
}

SumWidget::~SumWidget()
{
}

QString SumWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

    if (!withValues) {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    } else {
        if (dontPrintEmptyValues(m_FormItem) && line->text().isEmpty())
            return QString();
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(line->text());
    }
}

void SumWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (line)
        line->setToolTip(m_FormItem->spec()->tooltip());
}

void SumWidget::connectFormItems()
{
//    qWarning() << "SUM requiered" << formItem()->extraData().value(::SUM_EXTRA_KEY) << formItem()->extraData().value(::SUM_REGEXP_EXTRA_KEY);
    if (formItem()->extraData().value(::SUM_EXTRA_KEY).isEmpty() && formItem()->extraData().value(::SUM_REGEXP_EXTRA_KEY).isEmpty()) {
        LOG_ERROR("Sum widget: No sumof tag <sumof> or <sumof_regexp>.");
        return;
    }
    Form::FormMain *p = formItem()->parentFormMain();
    if (!p) {
        LOG_ERROR("No FormMain parent");
        return;
    }
//            qWarning() << "Parent = " << p->uuid();
    if (!formItem()->extraData().value(::SUM_EXTRA_KEY).isEmpty()) {
        QStringList uuids = formItem()->extraData().value(::SUM_EXTRA_KEY).split(";");
        // get all formitems and connect to the dataChanged signal
        QList<Form::FormItem *> items = p->flattenedFormItemChildren();
        foreach(QString uid, uuids) {
            uid = uid.simplified();
            for(int i = 0; i < items.count(); ++i) {
                Form::FormItem *item = items.at(i);
                if (item==m_FormItem)
                    continue;
                if (item->uuid().compare(uid, Qt::CaseInsensitive)==0) {
                    if (item->itemData()) {
                        //                    qWarning() << "  connecting" << item->uuid();
                        connect(item->itemData(), SIGNAL(dataChanged(int)), this, SLOT(recalculate(int)));
                    }
                }
            }
        }
    } else if (!formItem()->extraData().value(::SUM_REGEXP_EXTRA_KEY).isEmpty()) {
        QRegExp reg(formItem()->extraData().value(::SUM_REGEXP_EXTRA_KEY), Qt::CaseInsensitive, QRegExp::Wildcard);
        QList<Form::FormItem *> items = p->flattenedFormItemChildren();
        for(int i = 0; i < items.count(); ++i) {
            Form::FormItem *item = items.at(i);
            if (item==m_FormItem)
                continue;
            if (item->uuid().contains(reg) && item->itemData()) {
//                qWarning() << "  connecting (regexp)" << item->uuid();
                connect(item->itemData(), SIGNAL(dataChanged(int)), this, SLOT(recalculate(int)));
            }
        }
    }
}

void SumWidget::recalculate(const int modifiedRef)
{
    Q_UNUSED(modifiedRef);
//    qWarning() << "SUM recalculate" << formItem()->extraData().value(::SUM_EXTRA_KEY);
    Form::FormMain *p = formItem()->parentFormMain();
    if (!p) {
        LOG_ERROR("No FormMain parent");
        return;
    }
    double sum = 0;
    if (!formItem()->extraData().value(::SUM_EXTRA_KEY).isEmpty()) {
        QStringList uuids = formItem()->extraData().value(::SUM_EXTRA_KEY).split(";");
        // get all formitems and connect to the dataChanged signal
        QList<Form::FormItem *> items = p->flattenedFormItemChildren();
        foreach(const QString &uid, uuids) {
            for(int i = 0; i < items.count(); ++i) {
                Form::FormItem *item = items.at(i);
                if (item->uuid().compare(uid, Qt::CaseInsensitive)==0) {
                    QVariant val = item->itemData()->data(0, Form::IFormItemData::CalculationsRole);
                    sum += val.toDouble();
                }
            }
        }
    } else if (!formItem()->extraData().value(::SUM_REGEXP_EXTRA_KEY).isEmpty()) {
        QRegExp reg(formItem()->extraData().value(::SUM_REGEXP_EXTRA_KEY), Qt::CaseInsensitive, QRegExp::Wildcard);
        QList<Form::FormItem *> items = p->flattenedFormItemChildren();
        for(int i = 0; i < items.count(); ++i) {
            Form::FormItem *item = items.at(i);
            if (item->uuid().contains(reg) && item->itemData()) {
                QVariant val = item->itemData()->data(0, Form::IFormItemData::CalculationsRole);
                sum += val.toDouble();
            }
        }
    }
    line->setText(QString::number(sum));
    if (formItem()->getOptions().contains(::CHANGE_EPISODE_LABEL, Qt::CaseInsensitive)) {
        addResultToEpisodeLabel(p, m_Label, QVariant(sum));
    }
}


//--------------------------------------------------------------------------------------------------------
//---------------------------------------- ScriptWidget --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
ScriptWidget::ScriptWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent),
    line(0),
    m_Editor(0)
{
    setObjectName("ScriptWidget_"+formItem->uuid());
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QLineEdit *le = formItem->parentFormMain()->formWidget()->findChild<QLineEdit*>(widget);
        if (le) {
            line = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            line = new QLineEdit(this);
        }
        m_Label = findLabel(formItem);
    } else {
        const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
        if (!layout.isEmpty()) {
            m_Editor = new Editor::TextEditor(this);
            m_Editor->setObjectName("ScriptWidgetTextEditor_" + m_FormItem->uuid());
            m_Editor->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
            // Find widget
            QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(layout);
            if (lay) {
                lay->addWidget(m_Editor);
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            }
            m_Label = findLabel(formItem);
        } else {
            // Prepare Widget Layout and label
            QBoxLayout *hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
            hb->addWidget(m_Label);

            // Add LineEdit for the result
            if (formItem->getOptions().contains(::SHOW_IN_TEXTEDITOR, Qt::CaseInsensitive)) {
                m_Editor = new Editor::TextEditor(this);
                m_Editor->setObjectName("ScriptWidgetTextEditor_" + m_FormItem->uuid());
                m_Editor->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
                hb->addWidget(m_Editor);
            } else {
                line = new QLineEdit(this);
                line->setObjectName("ScriptWidgetLineEdit_" + m_FormItem->uuid());
                line->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
                hb->addWidget(line);
            }
        }
    }

    setFocusedWidget(line);

    retranslate();

    // connect to parent FormMain
    Form::FormMain *p = formItem->parentFormMain();
    if (p) {
//        qWarning() << "parent" << p->uuid();
        connect(p, SIGNAL(formLoaded()), this, SLOT(connectFormItems()));
    }
}

ScriptWidget::~ScriptWidget()
{
}

QString ScriptWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

    if (!withValues) {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    } else {
        QString content;
        if (line)
            content = line->text();
        else
            content = m_Editor->getHtml();
        if (dontPrintEmptyValues(m_FormItem) && content.isEmpty())
            return QString();
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(content);
    }
}

void ScriptWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (line)
        line->setToolTip(m_FormItem->spec()->tooltip());
    if (m_Editor)
        m_Editor->setToolTip(m_FormItem->spec()->tooltip());
}

void ScriptWidget::connectFormItems()
{
//    qWarning() << "CalculationScript" << formItem()->extraData().value(::CONNECT_EXTRA_KEY) << formItem()->extraData().value(::CONNECT_REGEXP_EXTRA_KEY);
    if (formItem()->extraData().value(::CONNECT_EXTRA_KEY).isEmpty() &&
            formItem()->extraData().value(::CONNECT_REGEXP_EXTRA_KEY).isEmpty()) {
        LOG_ERROR("Calculation script widget: No <connect> tag.");
        return;
    }
    Form::FormMain *p = formItem()->parentFormMain();
    if (!p) {
        LOG_ERROR("No FormMain parent");
        return;
    }
//    qWarning() << "Parent = " << p->uuid();
//    QString ns = formItem()->extraData().value(::SCRIPT_NS_EXTRA_KEY);
    QString itemList = formItem()->extraData().value(::CONNECT_EXTRA_KEY);
    QString regexp = formItem()->extraData().value(::CONNECT_REGEXP_EXTRA_KEY);
    // Find all items to connect
    QList<Form::FormItem *> children = p->flattenedFormItemChildren();
    if (!itemList.isEmpty()) {
        QStringList items = itemList.split(";");
        for(int i = 0; i < children.count(); ++i) {
            Form::FormItem *item = children.at(i);
            if (item==m_FormItem)
                continue;
            QString uuid = item->uuid();
            if (items.contains(uuid, Qt::CaseInsensitive)) {
                if (item->itemData()) {
//                    qWarning() << "  connecting" << item->uuid();
                    connect(item->itemData(), SIGNAL(dataChanged(int)), this, SLOT(recalculate(int)));
                }
            }
        }
    } else if (!regexp.isEmpty()) {
        QRegExp reg(regexp, Qt::CaseInsensitive, QRegExp::Wildcard);
        for(int i = 0; i < children.count(); ++i) {
            Form::FormItem *item = children.at(i);
            if (item==m_FormItem)
                continue;
            QString uuid = item->uuid();
            if (uuid.contains(reg)) {
                if (item->itemData()) {
//                    qWarning() << "  connecting (regexp)" << item->uuid();
                    connect(item->itemData(), SIGNAL(dataChanged(int)), this, SLOT(recalculate(int)));
                }
            }
        }
    }
}

void ScriptWidget::recalculate(const int modifiedRef)
{
    Q_UNUSED(modifiedRef);
//    qWarning() << "CalculationScript recalculate" << formItem()->extraData().value(::SCRIPT_EXTRA_KEY);
    Form::FormMain *p = formItem()->parentFormMain();
    if (!p) {
        LOG_ERROR("No FormMain parent");
        return;
    }
    QString script = formItem()->extraData().value(::SCRIPT_EXTRA_KEY);

    // evaluate the script using the Core::IScriptEngine
    QScriptValue val = scriptManager()->evaluate(script);
    QString result = val.toString();

//    qWarning() << val.toVariant() << result;

    if (line)
        line->setText(result);
    else
        m_Editor->setHtml(result);
    if (formItem()->getOptions().contains(::CHANGE_EPISODE_LABEL, Qt::CaseInsensitive)) {
        addResultToEpisodeLabel(p, m_Label, result);
    }
}

