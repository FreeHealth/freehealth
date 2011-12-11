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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "baseformwidgets.h"
#include "frenchsocialnumber.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/iformitem.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QStringList>
#include <QMessageBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QStringListModel>

#include "ui_baseformwidget.h"

using namespace BaseWidgets;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

namespace {
    // TypeEnum must be sync with the widgetsName QStringList
    enum TypeEnum {
        Type_Undefined = 0,
        Type_Form,
        Type_Radio,
        Type_Check,
        Type_Combo,
        Type_MultiCheck,
        Type_UniqueList,
        Type_MultiList,
        Type_Spin,
        Type_DoubleSpin,
        Type_ShortText,
        Type_LongText,
        Type_HelpText,
        Type_File,
        Type_Group,
        Type_Date,
        Type_Button,
        Type_Sum,
        Type_FrenchNSS,
        Type_MaxType
    };

    // names must be sync with the type enum
    static const QStringList widgetsName =
            QStringList() << "undef" << "form" << "radio" << "check" << "combo"
            << "multicheck" << "uniquelist" << "multilist" << "spin" << "doublespin"
            << "shorttext" << "longtext" << "helptext" << "file" << "group"
            << "date" << "button" << "sum" << "frenchnss";

    const char * const  EXTRAS_COUNTRY          = "country";
    const char * const  EXTRAS_KEY              = "option";
    const char * const  EXTRAS_KEY2             = "options";
    const char * const  EXTRAS_KEY_COLUMN       = "column";
    const char * const  EXTRAS_COMPACT_VIEW     = "compact";
    const char * const  EXTRAS_GROUP_CHECKABLE  = "checkable";
    const char * const  EXTRAS_GROUP_CHECKED    = "checked";
    const char * const  EXTRAS_ALIGN_VERTICAL   = "vertical";
    const char * const  EXTRAS_ALIGN_HORIZONTAL = "horizontal";
    const char * const  CHANGE_EPISODE_LABEL    = "changeepisodelabel";

    // Date options
    const char * const  DATE_EXTRAS_KEY         = "dateformat";
    const char * const  DATE_NOW                = "now";
    const char * const  DATE_PATIENTLIMITS      = "patientLimits";
    const char * const  SUM_EXTRA_KEY           = "sumof";
    const char * const  SUM_REGEXP_EXTRA_KEY    = "sumof_regexp";

    const char * const  SPIN_EXTRAS_KEY_MIN         = "min";
    const char * const  SPIN_EXTRAS_KEY_MAX         = "max";
    const char * const  SPIN_EXTRAS_KEY_STEP        = "step";

    const char * const  LABEL_ALIGN_TOP   = "labelontop";
    const char * const  LABEL_ALIGN_LEFT  = "labelonleft";
    const char * const  DONTPRINTEMPTYVALUES = "DontPrintEmptyValues";
}

inline static Form::IFormWidget::LabelOptions labelAlignement(Form::FormItem *item, Form::IFormWidget::LabelOptions defaultValue = Form::IFormWidget::Label_OnLeft)
{
    const QStringList &o = item->getOptions();
    if (o.contains(::LABEL_ALIGN_TOP, Qt::CaseInsensitive))
        return Form::IFormWidget::Label_OnTop;
    else if (o.contains(::LABEL_ALIGN_LEFT, Qt::CaseInsensitive))
        return Form::IFormWidget::Label_OnLeft;
    return defaultValue;
}

inline static QStringList getCountries(Form::FormItem *item)
{
    if (!item->extraDatas().value(::EXTRAS_COUNTRY).isEmpty())
        return item->extraDatas().value(::EXTRAS_COUNTRY).split(";");
    return QStringList();
}

inline static int getNumberOfColumns(Form::FormItem *item, int defaultValue = 1)
{
    if (!item->extraDatas().value(::EXTRAS_KEY_COLUMN).isEmpty())
        return item->extraDatas().value(::EXTRAS_KEY_COLUMN).toInt();
    else
        return defaultValue;
}

inline static int isCompactView(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_COMPACT_VIEW, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

inline static int isGroupCheckable(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_GROUP_CHECKABLE, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

inline static int isGroupChecked(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_GROUP_CHECKED, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

inline static int isRadioHorizontalAlign(Form::FormItem *item, bool defaultValue = true)
{
    if (item->getOptions().contains(::EXTRAS_ALIGN_HORIZONTAL, Qt::CaseInsensitive))
        return true;
    else if (item->getOptions().contains(::EXTRAS_ALIGN_VERTICAL, Qt::CaseInsensitive))
        return false;
    return defaultValue;
}

inline static QString getDateFormat(Form::FormItem *item, const QString & defaultValue = "dd MM yyyy")
{
    if (!item->extraDatas().value(::DATE_EXTRAS_KEY).isEmpty()) {
        return item->extraDatas().value(::DATE_EXTRAS_KEY);
    }
    return defaultValue;
}

inline static bool dontPrintEmptyValues(Form::FormItem *item)
{
    return item->getOptions().contains(::DONTPRINTEMPTYVALUES, Qt::CaseInsensitive);
}

BaseWidgetsFactory::BaseWidgetsFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

BaseWidgetsFactory::~BaseWidgetsFactory()
{}

bool BaseWidgetsFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool BaseWidgetsFactory::extensionInitialized()
{
    return true;
}

bool BaseWidgetsFactory::isInitialized() const
{
    return true;
}

QStringList BaseWidgetsFactory::providedWidgets() const
{
    return ::widgetsName;
}

bool BaseWidgetsFactory::isContainer(const int idInStringList) const
{
    return (idInStringList == ::Type_Form) ||
            (idInStringList == ::Type_Group);
}

Form::IFormWidget *BaseWidgetsFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    int id = ::widgetsName.indexOf(name);
    if (id == -1)
        return 0;
    switch (id)
    {
    case ::Type_Form : return new BaseForm(formItem,parent);
    case ::Type_Group : return new BaseGroup(formItem,parent);
    case ::Type_Check : return new BaseCheck(formItem,parent);
    case ::Type_Radio : return new BaseRadio(formItem,parent);
    case ::Type_ShortText : return new BaseSimpleText(formItem,parent,true);
    case ::Type_LongText : return new BaseSimpleText(formItem,parent,false);
    case ::Type_HelpText : return new BaseHelpText(formItem,parent);
    case ::Type_MultiList : return new BaseList(formItem,parent,false);
    case ::Type_UniqueList : return new BaseList(formItem,parent,true);
    case ::Type_Combo : return new BaseCombo(formItem,parent);
    case ::Type_Date : return new BaseDate(formItem,parent);
    case ::Type_Spin : return new BaseSpin(formItem,parent);
    case ::Type_DoubleSpin : return new BaseSpin(formItem,parent,true);
    case ::Type_Button : return new BaseButton(formItem,parent);
    case ::Type_Sum : return new SumWidget(formItem,parent);
    case ::Type_FrenchNSS : return new FrenchSocialNumberFormWidget(formItem,parent);
    default: return 0;
    }
    return 0;
}



/** \todo Verify usage of clear() in all itemData() --> originalValue ? */


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \class BaseForm
   \brief Creates a form
   Manages some Form::FormItem::extraDatas() (that is a QHash<QString, QString>) :
   - "col=" ; "numberOfColumns"
*/

BaseForm::BaseForm(Form::FormItem *formItem, QWidget *parent) :
        Form::IFormWidget(formItem,parent),
        m_ContainerLayout(0),
        m_Header(0)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    QWidget *header = new QWidget(this);
    m_Header = new Ui::BaseFormWidget;
    m_Header->setupUi(header);

    m_EpisodeDate = m_Header->dateEdit;
    m_EpisodeDate->setDisplayFormat(QLocale().dateFormat(QLocale::LongFormat));
    m_EpisodeDate->setEnabled(false);
    m_EpisodeDate->setCalendarPopup(true);

    m_EpisodeLabel = m_Header->lineEdit;
    m_EpisodeLabel->setEnabled(false);
    m_Header->label->setText(m_FormItem->spec()->label());

    // create main widget
    QWidget *mainWidget = new QWidget;
    mainLayout->addWidget(mainWidget);
    mainLayout->addStretch();

    // create container layout
    m_ContainerLayout = new QGridLayout(mainWidget);
    // Retrieve the number of columns for the gridlayout (lays in extraDatas() of linked FormItem)
    numberColumns = getNumberOfColumns(m_FormItem);
    if (isCompactView(m_FormItem)) {
        mainLayout->setMargin(0);
        mainLayout->setSpacing(2);
        m_ContainerLayout->setMargin(0);
        m_ContainerLayout->setSpacing(2);
    }

    m_ContainerLayout->addWidget(header, 0, 0, 1, numberColumns);
    i = numberColumns * 2;
    row = 0;
    col = 0;

    // create itemdata
    BaseFormData *datas = new BaseFormData(formItem);
    datas->setForm(this);
    formItem->setItemDatas(datas);
}

BaseForm::~BaseForm()
{
    if (m_Header) {
        delete m_Header;
        m_Header = 0;
    }
}

void BaseForm::addWidgetToContainer(IFormWidget * widget)
{
    if (!widget)
        return;
    if (!m_ContainerLayout)
        return;
    // it is not possible to add a form inside a container
    if (widget->formItem()->spec()->pluginName() == ::widgetsName[::Type_Form])
        return;
    col = (i % numberColumns);
    row = (i / numberColumns);
    m_ContainerLayout->addWidget(widget , row, col);
    i++;
}

QString BaseForm::printableHtml(bool withValues) const
{
    QStringList html;
    QList<Form::FormItem*> items = m_FormItem->formItemChildren();
    for(int i = 0; i < items.count(); ++i) {
        html << items.at(i)->formWidget()->printableHtml(withValues);
    }
    html.removeAll("");

//    qWarning() << m_FormItem->spec()->label() << dontPrintEmptyValues(m_FormItem) << html.count();
//    qWarning() << m_FormItem->getOptions();

    if (html.isEmpty() && dontPrintEmptyValues(m_FormItem))
        return QString();

//    qWarning() << html;

    int i = 0;
    int c = 0;
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
            .arg(m_EpisodeLabel->toolTip().replace("right", "center").replace("<p ", "<span ").replace("</p>", "</span>"));

    // recreate the grid as an html table
    foreach(const QString &s, html) {
        c = (i % numberColumns);
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

void BaseForm::retranslate()
{
    m_Header->label->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseFormData::BaseFormData(Form::FormItem *item) :
        m_FormItem(item), m_Form(0)
{}

BaseFormData::~BaseFormData()
{}

void BaseFormData::clear()
{
    m_Data.clear();
    m_Form->m_EpisodeDate->setDate(QDate::currentDate());
    m_Form->m_EpisodeLabel->clear();
    m_Form->m_EpisodeLabel->setEnabled(false);
    m_Form->m_EpisodeDate->setEnabled(false);
}

bool BaseFormData::isModified() const
{
    /** \todo here */
    return true;
}

bool BaseFormData::setData(const int ref, const QVariant &data, const int role)
{
    m_Data.insert(role, data);
    switch (role) {
    case ID_EpisodeDate:
        m_Form->m_EpisodeDate->setDate(m_Data.value(role).toDate());
        m_Form->m_EpisodeDate->setEnabled(true);
        break;
    case ID_EpisodeLabel:
        m_Form->m_EpisodeLabel->setText(m_Data.value(role).toString());
        m_Form->m_EpisodeLabel->setEnabled(true);
        break;
    }
    m_Form->m_EpisodeDate->setToolTip(QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                                       .arg(m_Data.value(ID_EpisodeDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)))
                                       .arg(m_Data.value(ID_EpisodeLabel).toString().replace(" ", "&nbsp;"))
                                       .arg(m_Data.value(ID_UserName).toString().replace(" ", "&nbsp;")));
    m_Form->m_EpisodeLabel->setToolTip(QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                                       .arg(m_Data.value(ID_EpisodeDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)))
                                       .arg(m_Data.value(ID_EpisodeLabel).toString().replace(" ", "&nbsp;"))
                                       .arg(m_Data.value(ID_UserName).toString().replace(" ", "&nbsp;")));
    return true;
}

QVariant BaseFormData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    /** \todo code here : IFormItemData should have a submit method */
    switch (role) {
    case ID_EpisodeDate: return m_Form->m_EpisodeDate->date();
    case ID_EpisodeLabel: return m_Form->m_EpisodeLabel->text();
    }
    return QVariant();
}


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseGroup implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseGroup::BaseGroup(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Group(0), m_ContainerLayout(0)
{
//    bool useGrid = true;
//    bool useFormGrid = false;
//    if (formItem->getOptions().contains("form", Qt::CaseInsensitive)) {
//        useGrid = false;
//        useFormGrid = true;
//    } else if (formItem->getOptions().contains("nogrid", Qt::CaseInsensitive)) {
//        useGrid = false;
//        useFormGrid = false;
//    }
    QVBoxLayout * vblayout = new QVBoxLayout(this);
    m_Group = new QGroupBox(this);
    m_Group->setTitle(m_FormItem->spec()->label());
    vblayout->addWidget(m_Group);
    this->setLayout(vblayout);

    // Retrieve the number of columns for the gridlayout (lays in extraDatas() of linked FormItem)
    numberColumns = getNumberOfColumns(m_FormItem, 2);

    // Create the gridlayout with all the widgets
    m_ContainerLayout = new QGridLayout(m_Group);
    i = 0;
    row = 0;
    col = 0;
    if (isCompactView(m_FormItem)) {
        vblayout->setMargin(0);
        vblayout->setSpacing(2);
        m_ContainerLayout->setMargin(0);
        m_ContainerLayout->setSpacing(2);
    }

    // Check country specific options
    const QStringList &countries = getCountries(formItem);
    if (!countries.isEmpty()) {
        if (!countries.contains(QLocale().name().right(2), Qt::CaseInsensitive))
            this->hide();
    }

    if (isGroupCheckable(m_FormItem, false)) {
        m_Group->setCheckable(true);
        m_Group->setChecked(isGroupChecked(m_FormItem,false));
        //          connect(m_Group, SIGNAL(clicked(bool)),
        //                   this,    SLOT  (updateObject(bool)));
        //          connect(mfo(m_FormItem), SIGNAL(valueChanged()),
        //                   this,     SLOT  (updateWidget()));
    }
    m_Group->setLayout(m_ContainerLayout);
}

BaseGroup::~BaseGroup()
{
}

void BaseGroup::addWidgetToContainer(IFormWidget * widget)
{
    if (!widget)
        return;
    if (!m_ContainerLayout)
        return;
    // it is not possible to add a form inside a container
    if (widget->formItem()->spec()->pluginName() == ::widgetsName[::Type_Form])
        return;
    col = (i % numberColumns);
    row = (i / numberColumns);
    m_ContainerLayout->addWidget(widget , row, col);
    i++;
}

QString BaseGroup::printableHtml(bool withValues) const
{
    // Check country specific options
    const QStringList &countries = getCountries(m_FormItem);
    if (!countries.isEmpty()) {
        if (!countries.contains(QLocale().name().right(2), Qt::CaseInsensitive))
            return QString();
    }

    QStringList html;
    QString content;
    QList<Form::FormItem*> items = m_FormItem->formItemChildren();
    for(int i = 0; i < items.count(); ++i) {
        html << items.at(i)->formWidget()->printableHtml(withValues);
    }
    // remove empty values
    html.removeAll("");

    // group is empty ?
    if (withValues) {
        if (html.isEmpty())
            return QString();
    }
    if (html.isEmpty() && dontPrintEmptyValues(m_FormItem))
        return QString();

    // recreate the grid as an html table
    int i = 0;
    int c = 0;
    int r = 0;
    int previousrow = 0;
    foreach(const QString &s, html) {
        c = (i % numberColumns);
        r = (i / numberColumns);
        if (r>previousrow) {
            previousrow = r;
            content += "</tr><tr>";
        }
        ++i;
        if (i==(html.count()-1)) {
        content += QString("<td style=\"vertical-align: top; align: left; padding-left: 5px; padding-top: 5px; padding-right: 5px; padding-bottom: 0px\">"
                           "%1"
                           "</td>").arg(s);
        } else {
            content += QString("<td style=\"vertical-align: top; align: left; padding-left: 5px; padding-top: 5px; padding-right: 5px; padding-bottom: 5px\">"
                               "%1"
                               "</td>").arg(s);
        }
    }

    return QString("<table width=100% border=1 cellpadding=0 cellspacing=0 style=\"margin: 5px 0px 0px 0px\">"
                   "<thead>"
                   "<tr>"
                   "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\" colspan=%1>"
                    "%2"
                   "</td>"
                   "</tr>"
                   "</thead>"
                   "<tbody>"
                   "<tr>"
                   "<td>"
                   "<table width=100% border=0 cellpadding=0 cellspacing=0 style=\"margin: 0px\">"
                   "<tbody>"
                   "<tr>"
                   "%3"
                   "</tr>"
                   "</tbody>"
                   "</table>"
                   "</td>"
                   "</tr>"
                   "</tbody>"
                   "</table>")
            .arg(numberColumns)
            .arg(m_FormItem->spec()->label())
            .arg(content);
}

void BaseGroup::retranslate()
{
    m_Group->setTitle(m_FormItem->spec()->label());
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseCheck ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCheck::BaseCheck(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Check(0)
{
    QHBoxLayout * hb = new QHBoxLayout(this);
    // Add Buttons
    m_Check = new QCheckBox(this);
    m_Check->setObjectName("Checkbox_" + m_FormItem->uuid());
    hb->addWidget(m_Check);
    retranslate();
    // create itemdata
    m_ItemData = new BaseCheckData(formItem);
    m_ItemData->setCheckBox(m_Check);
    formItem->setItemDatas(m_ItemData);
}

BaseCheck::~BaseCheck()
{
}

QString BaseCheck::printableHtml(bool withValues) const
{
    // ⍌⎕☒☑
    if (withValues) {
        if (m_Check->isChecked())
            return QString("%1&nbsp;%2").arg("☒").arg(m_FormItem->spec()->label());
        else if (!m_FormItem->getOptions().contains("printonlychecked", Qt::CaseInsensitive))
            return QString("%1&nbsp;%2").arg("⎕").arg(m_FormItem->spec()->label());
    } else {
        return QString("%1&nbsp;%2").arg("⎕").arg(m_FormItem->spec()->label());
    }
    return QString();
}

void BaseCheck::retranslate()
{
    m_Check->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseCheckData::BaseCheckData(Form::FormItem *item) :
        m_FormItem(item), m_Check(0)
{}

BaseCheckData::~BaseCheckData()
{}

void BaseCheckData::setCheckBox(QCheckBox *chk)
{
    m_Check = chk;
    clear();
}

void BaseCheckData::clear()
{
    const QString &s = m_FormItem->valueReferences()->defaultValue().toString();
    m_Check->setChecked(false);

    if (s.isEmpty())
        return;

    if (s.compare("checked", Qt::CaseInsensitive)==0)
        m_Check->setChecked(true);
    else if (s.compare("unchecked", Qt::CaseInsensitive)==0)
        m_Check->setChecked(false);
    else if (s.compare("partial", Qt::CaseInsensitive)==0)
        m_Check->setCheckState(Qt::PartiallyChecked);
}

bool BaseCheckData::isModified() const
{
    return m_OriginalValue != m_Check->checkState();
}

bool BaseCheckData::setData(const int ref, const QVariant &data, const int role)
{
//    qWarning() << "BaseCheckData::setData" << data << role;
    if (role==Qt::EditRole || role==Qt::DisplayRole) {
        if (data.canConvert(QVariant::Int))  { // Tristate
            m_Check->setCheckState(Qt::CheckState(data.toInt()));
        }
    }
    return true;
}

QVariant BaseCheckData::data(const int ref, const int role) const
{
    return m_Check->checkState();
}

void BaseCheckData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    Qt::CheckState state = Qt::CheckState(data.toInt());
    m_Check->setCheckState(state);
    m_OriginalValue = state;
}

QVariant BaseCheckData::storableData() const
{
    return m_Check->checkState();
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseRadio ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseRadio::BaseRadio(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_ButGroup(0)
{
    setObjectName("BaseRadio");
    // Prepare Widget Layout and label
    QBoxLayout *hb = getBoxLayout(labelAlignement(formItem, Label_OnTop), m_FormItem->spec()->label(), this);

    // Add QLabel
//    m_Label->setSizePolicy(QSizePolicy::Preferred , QSizePolicy::Preferred);
    hb->addWidget(m_Label);

    // Add Buttons
    QGroupBox *gb = new QGroupBox(this);
    m_ButGroup = new QButtonGroup(this);
    //     gb->setFlat(true);
    //     QSizePolicy sizePolicy = gb->sizePolicy();
    //     sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    //     gb->setSizePolicy(sizePolicy);
    QBoxLayout *radioLayout = 0;
    if (isRadioHorizontalAlign(m_FormItem)) {
        radioLayout = new QBoxLayout(QBoxLayout::LeftToRight, gb);
    } else {
        radioLayout = new QBoxLayout(QBoxLayout::TopToBottom, gb);
    }
//    qWarning() << isRadioHorizontalAlign(m_FormItem);
    radioLayout->setContentsMargins(1, 0, 1, 0);
    QRadioButton *rb = 0;
    int i = 0;

//    qWarning() << m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
//    qWarning() << m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);

    const QStringList &uids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    foreach (const QString &v, m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible)) {
        rb = new QRadioButton(this);
        m_ButGroup->addButton(rb);
        if (i < uids.count()) {
            rb->setObjectName(uids.at(i));
            rb->setProperty("id", uids.at(i));
        } else {
            if (m_FormItem->parentFormItem())
                LOG_ERROR(QString("No uuid defined for the form item: %1 in form %2").arg(v).arg(m_FormItem->parentFormItem()->uuid()));
            else
                LOG_ERROR(QString("No uuid defined for the form item: %1").arg(v));
        }
        rb->setText(v);
        i++;
        radioLayout->addWidget(rb);
        m_RadioList.append(rb);
    }
    hb->addWidget(gb);

    connect(m_ButGroup,SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));

    // create the FormItemData
    BaseRadioData *data = new BaseRadioData(m_FormItem);
    data->setBaseRadio(this);
    m_FormItem->setItemDatas(data);
}

BaseRadio::~BaseRadio()
{}

QString BaseRadio::printableHtml(bool withValues) const
{
    if (dontPrintEmptyValues(m_FormItem) && m_ButGroup->checkedButton()==0)
        return QString();

    // ⚪⚫
    QStringList html;
    bool horiz = isRadioHorizontalAlign(m_FormItem);
    foreach (QRadioButton *button, m_RadioList) {
        if (withValues) {
            if (button->isChecked()) {
                html << QString("%1&nbsp;%2").arg("⚫").arg(button->text());
                continue;
            }
        }
        html << QString("%1&nbsp;%2").arg("⚪").arg(button->text());
    }
    if (horiz) {
        QString buttons;
        foreach(const QString &s, html) {
            buttons += QString("<td style=\"vertical-align: center; align: center; padding: 2px\">"
                               "%1"
                               "</td>")
                    .arg(s);

        }
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0 style=\"margin: 5px 0px 0px 0px\">"
                       "<thead>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\" colspan=%3>"
                        "%1"
                       "</td>"
                       "</tr>"
                       "</thead>"
                       "<tbody>"
                       "<tr>"
                       "%2"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(buttons).arg(html.count());
    } else {
        QString buttons;
        foreach(const QString &s, html) {
            buttons += QString("<tr><td style=\"vertical-align: center; align: center\">"
                               "%1"
                               "</td></tr>")
                    .arg(s);

        }
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0 style=\"margin: 5px 0px 0px 0px\">"
                       "<thead>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                        "%1"
                       "</td>"
                       "</tr>"
                       "</thead>"
                       "<tbody>"
                       "%2"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(buttons);
    }
    return QString();
}

void BaseRadio::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());

    if (m_RadioList.size()) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_RadioList.count()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        int i = 0;
        foreach (QRadioButton *button, m_RadioList) {
            button->setText(list.at(i));
            i++;
        }
    }
}

void BaseRadio::buttonClicked(QAbstractButton *radio)
{
    formItem()->itemDatas()->setData(0, radio->property("id"), Form::IFormItemData::ID_ForCalculations);
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseRadioData::BaseRadioData(Form::FormItem *item) :
        m_FormItem(item), m_Radio(0)
{
}

BaseRadioData::~BaseRadioData()
{}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseRadioData::clear()
{
    QString id = m_FormItem->valueReferences()->defaultValue().toString();
    m_Radio->m_ButGroup->setExclusive(false);
    foreach(QRadioButton *b, m_Radio->m_RadioList) {
        b->setChecked(false);
    }
    m_Radio->m_ButGroup->setExclusive(true);
    foreach(QRadioButton *b, m_Radio->m_RadioList) {
        if (b->property("id").toString()==id) {
            b->setChecked(true);
            break;
        }
    }
}

bool BaseRadioData::isModified() const
{
    foreach(QRadioButton *but, m_Radio->m_RadioList) {
        if (but->isChecked()) {
//            qWarning() << "Radio selected" << but->property("id").toString() << "modified" << (m_OriginalValue != but->property("id").toString());
            return m_OriginalValue != but->property("id").toString();
        }
    }
    return true;
}

bool BaseRadioData::setData(const int ref, const QVariant &data, const int role)
{
    // receive ref=0; data=uid of activated radio; role=IFormItemData::RoleRepresentation
//    qWarning() << "BaseRadioData::setData" << data << role;
    if (role==Form::IFormItemData::ID_ForCalculations) {
        Q_EMIT dataChanged(ref); // just emit the dataChanged signal
    }
    return true;
}

QVariant BaseRadioData::data(const int ref, const int role) const
{
    if (role==Form::IFormItemData::ID_ForCalculations) {
        // return selected value::numerical (if exists)
        QString selectedUid;
        foreach(QRadioButton *but, m_Radio->m_RadioList) {
            if (but->isChecked()) {
                selectedUid = but->property("id").toString();
                break;
            }
        }
        int id = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(selectedUid);
        const QStringList &vals = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Numerical);
//        qWarning() << "Radio -> DATA" << selectedUid << id << vals;
        if (id < vals.count() && id >= 0)
            return vals.at(id);
    }
    return QVariant();
}

void BaseRadioData::setStorableData(const QVariant &data)
{
    // Storable data == id of the selected radio button
//    if (!data.isValid())
//        return;
    QString id = data.toString();
    foreach(QRadioButton *b, m_Radio->m_RadioList) {
        if (b->property("id").toString()==id) {
            b->setChecked(true);
            break;
        }
    }
    m_OriginalValue = id;
    Q_EMIT dataChanged(0); // just emit the dataChanged signal
//    qWarning() << "Radio orig" << id;
}

QVariant BaseRadioData::storableData() const
{
    foreach(QRadioButton *but, m_Radio->m_RadioList) {
        if (but->isChecked())
            return but->property("id");
    }
    /** \todo return the default value ? */
    return QVariant();
}



//--------------------------------------------------------------------------------------------------------
//------------------------------------------- BaseSimpleText -------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSimpleText::BaseSimpleText(Form::FormItem *formItem, QWidget *parent, bool shortText) :
    Form::IFormWidget(formItem,parent), m_Line(0), m_Text(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add List and manage size
    if (shortText) {
        //          if (!(mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop))
        //          {
        //               Qt::Alignment alignment = m_Label->alignment();
        //               alignment &= ~(Qt::AlignVertical_Mask);
        //               alignment |= Qt::AlignVCenter;
        //               m_Label->setAlignment(alignment);
        //          }

        m_Line = new QLineEdit(this);
        m_Line->setObjectName("Line_" + m_FormItem->uuid());
        m_Line->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        //          m_Line->setInputMask(mfo(m_FormItem)->mask());
        //          m_Line->setCursorPosition(0);
        hb->addWidget(m_Line);
    } else {
        m_Text = new QTextEdit(this);
        m_Text->setObjectName("Text_" + m_FormItem->uuid());
        m_Text->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
        hb->addWidget(m_Text);
    }

    // Create the FormItemData
    BaseSimpleTextData *data = new BaseSimpleTextData(m_FormItem);
    data->setBaseSimpleText(this);
    m_FormItem->setItemDatas(data);
}

BaseSimpleText::~BaseSimpleText()
{
}

QString BaseSimpleText::printableHtml(bool withValues) const
{
    if (withValues) {
        if (dontPrintEmptyValues(m_FormItem)) {
            if (m_Line && m_Line->text().isEmpty())
                return QString();
            else if (m_Text && m_Text->toPlainText().isEmpty())
                return QString();
        }
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
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
                .arg(m_FormItem->spec()->label()).arg(m_Text->toHtml().remove("</body>").remove("</html>"));
    } else {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
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

void BaseSimpleText::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseSimpleTextData::BaseSimpleTextData(Form::FormItem *item) :
        m_FormItem(item), m_Text(0)
{
}

BaseSimpleTextData::~BaseSimpleTextData()
{}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseSimpleTextData::clear()
{
    const QString &s = m_FormItem->valueReferences()->defaultValue().toString();
    if (m_Text->m_Line)
        m_Text->m_Line->setText(s);
    else if (m_Text->m_Text)
        m_Text->m_Text->setPlainText(s);
}

bool BaseSimpleTextData::isModified() const
{
    if (m_Text->m_Line)
        return m_OriginalValue != m_Text->m_Line->text();
    else if (m_Text->m_Text)
        return m_OriginalValue != m_Text->m_Text->toPlainText();
    return true;
}

bool BaseSimpleTextData::setData(const int ref, const QVariant &data, const int role)
{
    return true;
}

QVariant BaseSimpleTextData::data(const int ref, const int role) const
{
    return QVariant();
}

void BaseSimpleTextData::setStorableData(const QVariant &data)
{
    m_OriginalValue = data.toString();
    if (m_Text->m_Line)
        m_Text->m_Line->setText(m_OriginalValue);
    else if (m_Text->m_Text)
        m_Text->m_Text->setPlainText(m_OriginalValue);
}

QVariant BaseSimpleTextData::storableData() const
{
    if (m_Text->m_Line)
        return m_Text->m_Line->text();
    else if (m_Text->m_Text)
        return m_Text->m_Text->toPlainText();
    return QVariant();
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseHelpText -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseHelpText::BaseHelpText(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent)
{
    QHBoxLayout * hb = new QHBoxLayout(this);
    // Add QLabel
    createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
    // Setting objectName to hide/show via a simple option button
    m_Label->setObjectName("HelpText_" + m_FormItem->uuid());
    hb->addWidget(m_Label);
}

BaseHelpText::~BaseHelpText()
{
}

QString BaseHelpText::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    if (!m_FormItem->getOptions().contains("notprintable", Qt::CaseInsensitive)) {
        return QString("<table width=100% border=0 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    }
    return QString();
}

void BaseHelpText::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseLists --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseList::BaseList(Form::FormItem *formItem, QWidget *parent, bool uniqueList) :
    Form::IFormWidget(formItem,parent), m_List(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add List and manage size
    m_List = new QListView(this);
    m_List->setObjectName("List_" + m_FormItem->uuid());
    m_List->setUniformItemSizes(true);
    m_List->setAlternatingRowColors(true);
    m_List->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
    if (uniqueList)
        m_List->setSelectionMode(QAbstractItemView::SingleSelection);
    else
        m_List->setSelectionMode(QAbstractItemView::MultiSelection);

    const QStringList &possibles = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
    m_Model = new QStringListModel(possibles, this);
    m_List->setModel(m_Model);

    hb->addWidget(m_List);

    // create FormItemData
    BaseListData *data = new BaseListData(m_FormItem);
    data->setBaseList(this);
    m_FormItem->setItemDatas(data);
}

BaseList::~BaseList()
{
}

QString BaseList::printableHtml(bool withValues) const
{
    QString content;
    if (!withValues) {
        foreach(const QString &v, m_Model->stringList()) {
            content += "<li>" + v + "</li>";
        }
    } else {
        QModelIndexList indexes = m_List->selectionModel()->selectedIndexes();
        if (dontPrintEmptyValues(m_FormItem) && indexes.isEmpty())
            return QString();
        qSort(indexes);
        foreach(const QModelIndex &i, indexes) {
            content += "<li>" + i.data().toString() + "</li>";
        }
    }
    if (!content.isEmpty()) {
        content.prepend("<ul>");
        content.append("</ul>");
    }
    return content;
}

void BaseList::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
    if (m_List) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_Model->rowCount()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        // keep selection
        QModelIndexList indexes = m_List->selectionModel()->selectedIndexes();
        m_Model->setStringList(list);
        foreach(const QModelIndex &i, indexes) {
            m_List->selectionModel()->select(i, QItemSelectionModel::Select);
        }
    }
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseListData::BaseListData(Form::FormItem *item) :
        m_FormItem(item), m_List(0)
{
}

BaseListData::~BaseListData()
{
}

void BaseListData::setSelectedItems(const QString &s)
{
    QItemSelectionModel *selModel = m_List->m_List->selectionModel();
    selModel->clearSelection();
    if (s.isEmpty())
        return;

    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    if (s.contains("`@`")) {
        // multilist
        foreach(const QString &i, s.split("`@`", QString::SkipEmptyParts)) {
            int row = uuids.lastIndexOf(i);
            QModelIndex idx = m_List->m_Model->index(row, 0);
            selModel->select(idx, QItemSelectionModel::Select);
        }
    } else {
        int row = uuids.lastIndexOf(s);
        QModelIndex idx = m_List->m_Model->index(row, 0);
        selModel->select(idx, QItemSelectionModel::Select);
    }
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseListData::clear()
{
    setSelectedItems(m_FormItem->valueReferences()->defaultValue().toString());
}

bool BaseListData::isModified() const
{
    QStringList actual = storableData().toStringList();
    return actual != m_OriginalValue;
}

bool BaseListData::setData(const int ref, const QVariant &data, const int role)
{
    return true;
}

QVariant BaseListData::data(const int ref, const int role) const
{
    return QVariant();
}

void BaseListData::setStorableData(const QVariant &data)
{    
    setSelectedItems(data.toString());
    m_OriginalValue = data.toStringList();
    qSort(m_OriginalValue);
}

/** Storable data of a List is the uuids of the selected items sorted in alphabetic order. */
QVariant BaseListData::storableData() const
{
    QItemSelectionModel *selModel = m_List->m_List->selectionModel();

    if (!selModel->hasSelection())
        return QVariant();

    QStringList selected;
    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
        selected.append(uuids.at(idx.row()));
    }
    qSort(selected);
    return selected.join("`@`");
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseCombo --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCombo::BaseCombo(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Combo(0)
{
    // Prepare Widget Layout and label
    QBoxLayout *hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add List and manage size
    m_Combo = new QComboBox(this);
    m_Combo->setObjectName("Combo_" + m_FormItem->uuid());
    m_Combo->addItems(m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible));
    hb->addWidget(m_Combo);

    // create FormItemData
    BaseComboData *data = new BaseComboData(m_FormItem);
    data->setBaseCombo(this);
    m_FormItem->setItemDatas(data);
}

BaseCombo::~BaseCombo()
{
}

QString BaseCombo::printableHtml(bool withValues) const
{
    QString content;
    if (!withValues) {
        for(int i = 0; i < m_Combo->count(); ++i) {
            content += "<li>" + m_Combo->itemData(i).toString() + "</li>";
        }
    } else {
        if (m_Combo->currentIndex()==-1)
            return QString();
        content += "<li>" + m_Combo->currentText() + "</li>";
    }
    if (!content.isEmpty()) {
        content.prepend("<ul>");
        content.append("</ul>");
    }
    return content;
}

void BaseCombo::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
    if (m_Combo) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_Combo->count()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        // refresh combo items
        int id = m_Combo->currentIndex();
        m_Combo->clear();
        m_Combo->addItems(list);
        m_Combo->setCurrentIndex(id);
    }
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseComboData::BaseComboData(Form::FormItem *item) :
        m_FormItem(item), m_Combo(0)
{
}

BaseComboData::~BaseComboData()
{
}

int BaseComboData::selectedItem(const QString &s)
{
    m_Combo->m_Combo->setCurrentIndex(-1);
    if (s.isEmpty())
        return -1;

    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    int row = uuids.lastIndexOf(s);
    m_Combo->m_Combo->setCurrentIndex(row);
    return row;
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseComboData::clear()
{
    selectedItem(m_FormItem->valueReferences()->defaultValue().toString());
}

bool BaseComboData::isModified() const
{
    return m_OriginalValue != m_Combo->m_Combo->currentIndex();
}

bool BaseComboData::setData(const int ref, const QVariant &data, const int role)
{
    return true;
}

QVariant BaseComboData::data(const int ref, const int role) const
{
    return QVariant();
}

void BaseComboData::setStorableData(const QVariant &data)
{
    m_OriginalValue = selectedItem(data.toString());
}

QVariant BaseComboData::storableData() const
{
    int row = m_Combo->m_Combo->currentIndex();
    if (row < 0)
        return QVariant();
    return m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(row);
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseDate ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseDate::BaseDate(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Date(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add Date selector and manage date format
    m_Date = new QDateTimeEdit(this);
    m_Date->setObjectName("Date_" + m_FormItem->uuid());
    m_Date->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    m_Date->setDisplayFormat(getDateFormat(m_FormItem));
    m_Date->setCalendarPopup(true);
    hb->addWidget(m_Date);

    // Manage options
    const QStringList &options = formItem->getOptions();
    if (options.contains(::DATE_NOW, Qt::CaseInsensitive))
        m_Date->setDateTime(QDateTime::currentDateTime());
    if (options.contains(::DATE_PATIENTLIMITS, Qt::CaseInsensitive)) {
        connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onPatientChanged()));
        onPatientChanged();
    }

    // create FormItemData
    BaseDateData *data = new BaseDateData(m_FormItem);
    data->setBaseDate(this);
    m_FormItem->setItemDatas(data);
}

BaseDate::~BaseDate()
{
}

void BaseDate::onPatientChanged()
{
    if (!patient()->data(Core::IPatient::DateOfBirth).isNull()) {
        m_Date->setMinimumDate(patient()->data(Core::IPatient::DateOfBirth).toDate());
    } else {
        m_Date->setMinimumDate(QDate::currentDate().addYears(-200));
    }
    if (!patient()->data(Core::IPatient::DateOfDeath).isNull()) {
        m_Date->setMaximumDate(patient()->data(Core::IPatient::DateOfDeath).toDate());
    } else {
        m_Date->setMaximumDate(QDate::currentDate().addYears(200));
    }
}

QString BaseDate::printableHtml(bool withValues) const
{
    QString content;
    if (!withValues) {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top;\" width=50%>"
                       "&nbsp;"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    } else {
        if (dontPrintEmptyValues(m_FormItem) && m_Date->date().isNull())
            return QString();
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top;\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(m_Date->date().toString(getDateFormat(m_FormItem)));
    }
    return content;
}

void BaseDate::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseDateData::BaseDateData(Form::FormItem *item) :
        m_FormItem(item), m_Date(0)
{
}

BaseDateData::~BaseDateData()
{
}

void BaseDateData::setDate(const QString &s)
{
    m_Date->m_Date->clear();
    m_Date->m_Date->setDateTime(QDateTime::fromString(s, Qt::ISODate));
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseDateData::clear()
{
    m_OriginalValue = m_FormItem->valueReferences()->defaultValue().toString();
    setDate(m_OriginalValue);
}

bool BaseDateData::isModified() const
{
    return m_OriginalValue != m_Date->m_Date->dateTime().toString(Qt::ISODate);
}

bool BaseDateData::setData(const int ref, const QVariant &data, const int role)
{
    return true;
}

QVariant BaseDateData::data(const int ref, const int role) const
{
    return QVariant();
}

void BaseDateData::setStorableData(const QVariant &data)
{
    setDate(data.toString());
    m_OriginalValue = data.toString();
}

QVariant BaseDateData::storableData() const
{
    return m_Date->m_Date->dateTime().toString(Qt::ISODate);
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------ BaseSpin --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSpin::BaseSpin(Form::FormItem *formItem, QWidget *parent, bool doubleSpin) :
    Form::IFormWidget(formItem,parent), m_Spin(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);
    //     if (!(mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop))
    //     {
    //          Qt::Alignment alignment = m_Label->alignment();
    //          alignment &= ~(Qt::AlignVertical_Mask);
    //          alignment |= Qt::AlignVCenter;
    //          m_Label->setAlignment(alignment);
    //     }

    // Add spin
    if (doubleSpin) {
        QDoubleSpinBox *spin = new QDoubleSpinBox(this);
        spin->setObjectName("DoubleSpin_" + m_FormItem->uuid());
        spin->setMinimum(formItem->extraDatas().value(::SPIN_EXTRAS_KEY_MIN, "0").toDouble());
        spin->setMaximum(formItem->extraDatas().value(::SPIN_EXTRAS_KEY_MAX, "10000").toDouble());
        spin->setSingleStep(formItem->extraDatas().value(::SPIN_EXTRAS_KEY_STEP, "0.1").toDouble());
        m_Spin = spin;
    } else {
        QSpinBox *spin = new QSpinBox(this);
        spin->setObjectName("Spin_" + m_FormItem->uuid());
        spin->setMinimum(formItem->extraDatas().value(::SPIN_EXTRAS_KEY_MIN, "0").toInt());
        spin->setMaximum(formItem->extraDatas().value(::SPIN_EXTRAS_KEY_MAX, "10000").toInt());
        spin->setSingleStep(formItem->extraDatas().value(::SPIN_EXTRAS_KEY_STEP, "1").toInt());
        m_Spin = spin;
    }
    m_Spin->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    hb->addWidget(m_Spin);

    // manage options

    // create FormItemData
    BaseSpinData *data = new BaseSpinData(m_FormItem);
    data->setBaseSpin(this);
    m_FormItem->setItemDatas(data);
}

BaseSpin::~BaseSpin()
{}

QString BaseSpin::printableHtml(bool withValues) const
{
    QString content;
    if (!withValues) {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top;\" width=50%>"
                       "&nbsp;"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    } else {
        QSpinBox *spin = qobject_cast<QSpinBox*>(m_Spin);
        QString value;
        if (spin) {
            value = QString::number(spin->value());
        } else {
            QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin);
            if (dspin) {
                value = QString::number(dspin->value());
            }
        }
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top;\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(value);
    }
    return content;
}

void BaseSpin::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseSpinData::BaseSpinData(Form::FormItem *item) :
        m_FormItem(item), m_Spin(0)
{
}

BaseSpinData::~BaseSpinData()
{
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseSpinData::clear()
{
    m_OriginalValue = m_FormItem->valueReferences()->defaultValue().toDouble();
    QSpinBox *spin = qobject_cast<QSpinBox*>(m_Spin->m_Spin);
    if (spin) {
        spin->setValue(m_FormItem->valueReferences()->defaultValue().toInt());
    } else {
        QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin->m_Spin);
        if (dspin) {
            dspin->setValue(m_OriginalValue);
        }
    }
}

bool BaseSpinData::isModified() const
{
    return m_OriginalValue != storableData().toDouble();
}

bool BaseSpinData::setData(const int ref, const QVariant &data, const int role)
{
    return true;
}

QVariant BaseSpinData::data(const int ref, const int role) const
{
    return QVariant();
}

void BaseSpinData::setStorableData(const QVariant &data)
{
    m_OriginalValue = data.toDouble();
    QSpinBox *spin = qobject_cast<QSpinBox*>(m_Spin->m_Spin);
    if (spin) {
        spin->setValue(data.toInt());
        return;
    }
    QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin->m_Spin);
    if (dspin) {
        dspin->setValue(data.toDouble());
    }
}

QVariant BaseSpinData::storableData() const
{
    QSpinBox *spin = qobject_cast<QSpinBox*>(m_Spin->m_Spin);
    if (spin) {
        return spin->value();
    }
    QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin->m_Spin);
    if (dspin) {
        return dspin->value();
    }
    return QVariant();
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ BaseButton ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseButton::BaseButton(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Button(0)
{
    QHBoxLayout * hb = new QHBoxLayout(this);
    hb->addStretch();

    m_Button = new QPushButton(this);
    m_Button->setObjectName("Button_" + m_FormItem->uuid());
    m_Button->setText(m_FormItem->spec()->label());
    m_Button->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    hb->addWidget(m_Button);
    connect(m_Button, SIGNAL(clicked()) , this , SLOT(buttonClicked()));
}

BaseButton::~BaseButton()
{}

void BaseButton::buttonClicked()
{
    /** \todo run script */
//    m_FormItem->scripts()->runScript();
}

void BaseButton::retranslate()
{
    m_Button->setText(m_FormItem->spec()->label());
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------ SumWidget ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
SumWidget::SumWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent), line(0)
{
    setObjectName("SumWidget_"+formItem->uuid());
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add LineEdit for the result
    line = new QLineEdit(this);
    line->setObjectName("SumWidgetLineEdit_" + m_FormItem->uuid());
    line->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    hb->addWidget(line);

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
    m_Label->setText(m_FormItem->spec()->label());
}

void SumWidget::connectFormItems()
{
//    qWarning() << "SUM requiered" << formItem()->extraDatas().value(::SUM_EXTRA_KEY) << formItem()->extraDatas().value(::SUM_REGEXP_EXTRA_KEY);
    Form::FormMain *p = formItem()->parentFormMain();
    if (!p) {
        LOG_ERROR("No FormMain parent");
        return;
    }
    //        qWarning() << "Parent = " << p->uuid();
    if (!formItem()->extraDatas().value(::SUM_EXTRA_KEY).isEmpty()) {
        QStringList uuids = formItem()->extraDatas().value(::SUM_EXTRA_KEY).split(";");
        // get all formitems and connect to the dataChanged signal
        QList<Form::FormItem *> items = p->flattenFormItemChildren();
        foreach(QString uid, uuids) {
            uid = uid.simplified();
            for(int i = 0; i < items.count(); ++i) {
                Form::FormItem *item = items.at(i);
                if (item->uuid().compare(uid, Qt::CaseInsensitive)==0) {
//                    qWarning() << "  connecting" << item->uuid();
                    connect(item->itemDatas(), SIGNAL(dataChanged(int)), this, SLOT(recalculate(int)));
                }
            }
        }
    } else if (!formItem()->extraDatas().value(::SUM_REGEXP_EXTRA_KEY).isEmpty()) {
        QRegExp reg(formItem()->extraDatas().value(::SUM_REGEXP_EXTRA_KEY), Qt::CaseInsensitive, QRegExp::Wildcard);
        QList<Form::FormItem *> items = p->flattenFormItemChildren();
        for(int i = 0; i < items.count(); ++i) {
            Form::FormItem *item = items.at(i);
            if (item->uuid().contains(reg) && item->itemDatas()) {
//                qWarning() << "  connecting (regexp)" << item->uuid();
                connect(item->itemDatas(), SIGNAL(dataChanged(int)), this, SLOT(recalculate(int)));
            }
        }
    }
}

void SumWidget::recalculate(const int modifiedRef)
{
    Q_UNUSED(modifiedRef);
//    qWarning() << "SUM recalculate" << formItem()->extraDatas().value(::SUM_EXTRA_KEY);
    Form::FormMain *p = formItem()->parentFormMain();
    if (!p) {
        LOG_ERROR("No FormMain parent");
        return;
    }
    double sum = 0;
    if (!formItem()->extraDatas().value(::SUM_EXTRA_KEY).isEmpty()) {
        QStringList uuids = formItem()->extraDatas().value(::SUM_EXTRA_KEY).split(";");
        // get all formitems and connect to the dataChanged signal
        QList<Form::FormItem *> items = p->flattenFormItemChildren();
        foreach(const QString &uid, uuids) {
            for(int i = 0; i < items.count(); ++i) {
                Form::FormItem *item = items.at(i);
                if (item->uuid().compare(uid, Qt::CaseInsensitive)==0) {
                    QVariant val = item->itemDatas()->data(0, Form::IFormItemData::ID_ForCalculations);
                    sum += val.toDouble();
                }
            }
        }
    } else if (!formItem()->extraDatas().value(::SUM_REGEXP_EXTRA_KEY).isEmpty()) {
        QRegExp reg(formItem()->extraDatas().value(::SUM_REGEXP_EXTRA_KEY), Qt::CaseInsensitive, QRegExp::Wildcard);
        QList<Form::FormItem *> items = p->flattenFormItemChildren();
        for(int i = 0; i < items.count(); ++i) {
            Form::FormItem *item = items.at(i);
            if (item->uuid().contains(reg) && item->itemDatas()) {
                QVariant val = item->itemDatas()->data(0, Form::IFormItemData::ID_ForCalculations);
                sum += val.toDouble();
            }
        }
    }
    line->setText(QString::number(sum));
    if (formItem()->getOptions().contains(::CHANGE_EPISODE_LABEL, Qt::CaseInsensitive)) {
        QString episodeLabel = p->itemDatas()->data(0, Form::IFormItemData::ID_EpisodeLabel).toString();
        QTextDocument doc(this);
        doc.setHtml(m_Label->text());
        QString add = QString("[[%1 %2]]").arg(doc.toPlainText()).arg(sum);
        if (episodeLabel.contains("[[") && episodeLabel.contains("]]")) {
            // remove this part
            int begin = episodeLabel.indexOf("[[");
            int end = episodeLabel.indexOf("]]") + 2;
            episodeLabel.remove(begin, end-begin);
            // add the new value
            episodeLabel.insert(begin, add);
            p->itemDatas()->setData(0, episodeLabel, Form::IFormItemData::ID_EpisodeLabel);
        } else {
            p->itemDatas()->setData(0, episodeLabel + "; " + add, Form::IFormItemData::ID_EpisodeLabel);
        }
    }
}
