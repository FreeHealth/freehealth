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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "baseformwidgets.h"
#include "frenchsocialnumber.h"
//#include "austriansocialnumber.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>

#include <formmanagerplugin/iformitem.h>

#include <utils/global.h>
#include <utils/log.h>

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_filepathxml.h>

#include <QStringList>
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
#include <QFileDialog>

#include <QtUiTools/QUiLoader>
#include <QBuffer>

#include "ui_baseformwidget.h"

using namespace BaseWidgets;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

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
        Type_FrenchNSS,
        Type_AustrianSVNR,
        Type_MaxType
    };

    // names must be sync with the type enum
    static const QStringList widgetsName =
            QStringList() << "undef" << "form" << "radio" << "check" << "combo"
            << "multicheck" << "uniquelist" << "multilist" << "spin" << "doublespin"
            << "shorttext" << "longtext" << "helptext" << "file" << "group"
            << "date" << "button" << "frenchnss" << "austriansvnr";

    const char * const  EXTRAS_COUNTRY          = "country";
    const char * const  EXTRAS_KEY              = "option";
    const char * const  EXTRAS_KEY2             = "options";
    const char * const  EXTRAS_KEY_COLUMN       = "column";
    const char * const  EXTRAS_COMPACT_VIEW     = "compact";
    const char * const  EXTRAS_GROUP_CHECKABLE  = "checkable";
    const char * const  EXTRAS_GROUP_COLLAPSIBLE  = "collapsible";
    const char * const  EXTRAS_GROUP_EXPANDED  = "expanded";
    const char * const  EXTRAS_GROUP_CHECKED    = "checked";
    const char * const  EXTRAS_ALIGN_VERTICAL   = "vertical";
    const char * const  EXTRAS_ALIGN_HORIZONTAL = "horizontal";

    // Date options
    const char * const  DATE_EXTRAS_KEY         = "dateformat";
    const char * const  DATE_NOW                = "now";
    const char * const  DATE_PATIENTLIMITS      = "patientLimits";

    // Spins
    const char * const  SPIN_EXTRAS_KEY_MIN         = "min";
    const char * const  SPIN_EXTRAS_KEY_MAX         = "max";
    const char * const  SPIN_EXTRAS_KEY_STEP        = "step";

    // General options
    const char * const  LABEL_ALIGN_TOP      = "labelontop";
    const char * const  LABEL_ALIGN_LEFT     = "labelonleft";
    const char * const  DONTPRINTEMPTYVALUES = "DontPrintEmptyValues";
    const char * const  HIDEHEADERONUNIQUEEPISODE = "HideHeaderOnUniqueEpisode";
    const char * const  NOT_PRINTABLE        = "notprintable";
}

static inline Form::IFormWidget::LabelOptions labelAlignement(Form::FormItem *item, Form::IFormWidget::LabelOptions defaultValue = Form::IFormWidget::Label_OnLeft)
{
    const QStringList &o = item->getOptions();
    if (o.contains(::LABEL_ALIGN_TOP, Qt::CaseInsensitive))
        return Form::IFormWidget::Label_OnTop;
    else if (o.contains(::LABEL_ALIGN_LEFT, Qt::CaseInsensitive))
        return Form::IFormWidget::Label_OnLeft;
    return defaultValue;
}

static inline QStringList getCountries(Form::FormItem *item)
{
    if (!item->extraData().value(::EXTRAS_COUNTRY).isEmpty())
        return item->extraData().value(::EXTRAS_COUNTRY).split(";");
    return QStringList();
}

static inline int getNumberOfColumns(Form::FormItem *item, int defaultValue = 1)
{
    if (!item->extraData().value(::EXTRAS_KEY_COLUMN).isEmpty())
        return item->extraData().value(::EXTRAS_KEY_COLUMN).toInt();
    else
        return defaultValue;
}

static inline int isCompactView(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_COMPACT_VIEW, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

static inline int isGroupCheckable(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_GROUP_CHECKABLE, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

static inline int isGroupCollapsible(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_GROUP_COLLAPSIBLE, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

static inline int isGroupExpanded(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_GROUP_EXPANDED, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

static inline int isGroupChecked(Form::FormItem *item, bool defaultValue = false)
{
    if (item->getOptions().contains(::EXTRAS_GROUP_CHECKED, Qt::CaseInsensitive))
        return true;
    return defaultValue;
}

static inline int isRadioHorizontalAlign(Form::FormItem *item, bool defaultValue = true)
{
    if (item->getOptions().contains(::EXTRAS_ALIGN_HORIZONTAL, Qt::CaseInsensitive))
        return true;
    else if (item->getOptions().contains(::EXTRAS_ALIGN_VERTICAL, Qt::CaseInsensitive))
        return false;
    return defaultValue;
}

static inline QString getDateFormat(Form::FormItem *item, const QString & defaultValue = "dd MM yyyy")
{
    if (!item->extraData().value(::DATE_EXTRAS_KEY).isEmpty()) {
        return item->extraData().value(::DATE_EXTRAS_KEY);
    }
    return defaultValue;
}

static inline bool dontPrintEmptyValues(Form::FormItem *item)
{
    return item->getOptions().contains(::DONTPRINTEMPTYVALUES, Qt::CaseInsensitive);
}

static inline bool hideHeaderOnUniqueEpisode(Form::FormItem *item)
{
    return item->getOptions().contains(::HIDEHEADERONUNIQUEEPISODE, Qt::CaseInsensitive);
}

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
            l = new QLabel(item->formWidget());
        }
    }
    return l;
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
    case ::Type_FrenchNSS : return new FrenchSocialNumberFormWidget(formItem,parent);
//    case ::Type_AustrianSVNR : return new AustrianSocialNumberFormWidget(formItem,parent);
    default: return 0;
    }
    return 0;
}



// TODO: Verify usage of clear() in all itemData() --> originalValue ?


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
 * \class BaseForm
 * \brief Creates a form
 *  Manages some Form::FormItem::extraData() (that is a QHash<QString, QString>) :
 *  - "col=" ; "numberOfColumns"
*/
BaseForm::BaseForm(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent),
    m_ContainerLayout(0),
    i(0), row(0), col(0), numberColumns(1),
    m_Header(0),
    aScreenshot(0)
{
    setObjectName("BaseForm");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *header = new QWidget(this);
    m_Header = new Ui::BaseFormWidget;
    m_Header->setupUi(header);

    m_EpisodeDate = m_Header->dateTimeEdit;
    m_EpisodeDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));

    m_EpisodeDate->setEnabled(false);
    m_EpisodeDate->setCalendarPopup(true);

    m_EpisodeLabel = m_Header->lineEdit;
    m_EpisodeLabel->setEnabled(false);
    m_Header->label->setText(m_FormItem->spec()->label());

    aScreenshot = new QAction(this);
    aScreenshot->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));
    m_Header->toolButton->addAction(aScreenshot);
    connect(m_Header->toolButton, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
    m_Header->toolButton->setDefaultAction(aScreenshot);
    m_Header->toolButton->setFocusPolicy(Qt::ClickFocus);

    // create main widget
    QWidget *mainWidget = 0;
    const QString &uiContent = formItem->spec()->value(Form::FormItemSpec::Spec_UiFileContent).toString();
    if (!uiContent.isEmpty()) {
        // Create the Ui using the QtUi file
        QUiLoader loader;
        QBuffer buf;
        buf.setData(uiContent.toAscii());
        mainWidget = loader.load(&buf, this);

        // Manage options
        Form::FormMain *form = qobject_cast<Form::FormMain*>(formItem);
        if (form) {
            if (!(form->episodePossibilities()==Form::FormMain::UniqueEpisode && hideHeaderOnUniqueEpisode(formItem))) {
                mainLayout->addWidget(header);
            } else {
                header->hide();
            }
        }
    } else {
        mainLayout->setSpacing(0);
        mainLayout->setMargin(0);
        mainWidget = new QWidget(this);
        // create container layout
        m_ContainerLayout = new QGridLayout(mainWidget);
        // Retrieve the number of columns for the gridlayout (lays in extraData() of linked FormItem)
        numberColumns = getNumberOfColumns(m_FormItem);
        if (isCompactView(m_FormItem)) {
            mainLayout->setMargin(5);
            mainLayout->setSpacing(5);
            m_ContainerLayout->setMargin(5);
            m_ContainerLayout->setSpacing(5);
        }

        // Manage options
        Form::FormMain *form = qobject_cast<Form::FormMain*>(formItem);
        if (form) {
            if (!(form->episodePossibilities()==Form::FormMain::UniqueEpisode && hideHeaderOnUniqueEpisode(formItem))) {
                m_ContainerLayout->addWidget(header, 0, 0, 1, numberColumns);
                i = numberColumns * 2;
            } else {
                header->hide();
            }
        }
        row = 0;
        col = 0;
    }

    mainLayout->addWidget(mainWidget);
    mainLayout->addStretch();

    // create itemdata
    BaseFormData *baseFormData = new BaseFormData(formItem);
    baseFormData->setForm(this);
    formItem->setItemData(baseFormData);

    retranslate();
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
//    qWarning() << "ADDING to" << m_FormItem->uuid() << "widget" << widget->formItem()->uuid()
//               << "i" << i << "col" << col << "row" << row;
    m_ContainerLayout->addWidget(widget , row, col);
    i++;
}

QString BaseForm::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

    QStringList html;
    QList<Form::FormItem*> items = m_FormItem->formItemChildren();
    for(int i = 0; i < items.count(); ++i) {
        Form::IFormWidget *w = items.at(i)->formWidget();
        if (w)
            html << w->printableHtml(withValues);
    }
    html.removeAll("");

//    qWarning() << m_FormItem->spec()->label() << dontPrintEmptyValues(m_FormItem) << html.count();
//    qWarning() << m_FormItem->getOptions();

    if (html.isEmpty() && dontPrintEmptyValues(m_FormItem))
        return QString();

//    qWarning() << html;

    int i = 0;
//    int c = 0;
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
//        c = (i % numberColumns);
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
    if (m_Header)
        m_Header->label->setText(m_FormItem->spec()->label());
    if (aScreenshot) {
        aScreenshot->setText(tkTr(Trans::Constants::TAKE_SCREENSHOT));
        aScreenshot->setToolTip(tkTr(Trans::Constants::TAKE_SCREENSHOT));
    }
}

void BaseForm::triggered(QAction *action)
{
    if (action==aScreenshot) {
        QPixmap pix = QPixmap::grabWidget(this);
        QString fileName = QFileDialog::getSaveFileName(this, tkTr(Trans::Constants::SAVE_FILE),
                                                        settings()->path(Core::ISettings::UserDocumentsPath),
                                                        tr("Images (*.png)"));
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            if (info.completeSuffix().isEmpty())
                fileName.append(".png");
            pix.save(fileName);
        }
    }
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseFormData::BaseFormData(Form::FormItem *item) :
    m_FormItem(item), m_Form(0), m_Modified(false)
{}

BaseFormData::~BaseFormData()
{}

void BaseFormData::clear()
{
    m_Data.clear();
    m_Form->m_EpisodeDate->setDateTime(QDateTime::currentDateTime());
    m_Form->m_EpisodeLabel->clear();
    m_Form->m_EpisodeLabel->setEnabled(false);
    m_Form->m_EpisodeDate->setEnabled(false);
}

bool BaseFormData::isModified() const
{
    if (m_Modified)
        return true;
    foreach(int id, m_OriginalData.keys()) {
        if (data(id) != m_OriginalData.value(id))
            return true;
    }
    return false;
}

bool BaseFormData::setData(const int ref, const QVariant &data, const int role)
{
//    qWarning() << "Form::setData" << ref << data << role;
    if (role!=Qt::EditRole)
        return false;

    m_Data.insert(ref, data);
    m_Modified = true;
    switch (ref) {
    case ID_EpisodeDate:
        m_Form->m_EpisodeDate->setDateTime(m_Data.value(ref).toDateTime());
        m_Form->m_EpisodeDate->setEnabled(true);
        break;
    case ID_EpisodeLabel:
        m_Form->m_EpisodeLabel->setText(m_Data.value(ref).toString());
        m_Form->m_EpisodeLabel->setEnabled(true);
        break;
    }
    m_Form->m_EpisodeDate->setToolTip(QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                                      .arg(QLocale().toString(m_Data.value(ID_EpisodeDate).toDateTime(), QLocale::LongFormat).replace(" ","&nbsp;"))
                                       .arg(m_Data.value(ID_EpisodeLabel).toString().replace(" ", "&nbsp;"))
                                       .arg(m_Data.value(ID_UserName).toString().replace(" ", "&nbsp;")));
    m_Form->m_EpisodeLabel->setToolTip(QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                                       .arg(QLocale().toString(m_Data.value(ID_EpisodeDate).toDateTime(), QLocale::LongFormat).replace(" ","&nbsp;"))
                                       .arg(m_Data.value(ID_EpisodeLabel).toString().replace(" ", "&nbsp;"))
                                       .arg(m_Data.value(ID_UserName).toString().replace(" ", "&nbsp;")));
    return true;
}

QVariant BaseFormData::data(const int ref, const int role) const
{
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
        switch (ref) {
        case ID_EpisodeDate: return m_Form->m_EpisodeDate->dateTime();
        case ID_EpisodeLabel: return m_Form->m_EpisodeLabel->text();
        case ID_UserName: return m_Data.value(ID_UserName);
        }
    }
    return QVariant();
}

/** Use as setStorableData(bool) == setModified(bool) */
void BaseFormData::setStorableData(const QVariant &modified)
{
    m_Modified = modified.toBool();
    m_OriginalData = m_Data;
}

/** Use as bool storableData() == bool isModified() */
QVariant BaseFormData::storableData() const
{
    return QVariant(m_Modified);
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- BaseGroup implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseGroup::BaseGroup(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_Group(0),
    m_ItemData(0),
    m_ContainerLayout(0),
    i(0), row(0), col(0), numberColumns(1)
{
    setObjectName("BaseGroup");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QGroupBox *grp = qFindChild<QGroupBox*>(formItem->parentFormMain()->formWidget(), widget);
        if (grp) {
            m_Group = grp;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Group = new QGroupBox(this);
        }
    } else {
        QVBoxLayout *vblayout = new QVBoxLayout(this);
        m_Group = new QGroupBox(this);
        vblayout->addWidget(m_Group);
        setLayout(vblayout);
        vblayout->setMargin(0);

        // Retrieve the number of columns for the gridlayout (lays in extraData() of linked FormItem)
        numberColumns = getNumberOfColumns(m_FormItem, 2);

        // Create the gridlayout with all the widgets
        m_ContainerLayout = new QGridLayout(m_Group);
        if (isCompactView(m_FormItem)) {
            m_ContainerLayout->setMargin(0);
            m_ContainerLayout->setSpacing(2);
        }
        m_Group->setLayout(m_ContainerLayout);
    }
    m_Group->setTitle(m_FormItem->spec()->label());

    // Check country specific options
    const QStringList &countries = getCountries(formItem);
    if (!countries.isEmpty()) {
        if (!countries.contains(QLocale().name().right(2), Qt::CaseInsensitive))
            this->hide();
    }

    getCheckAndCollapsibleState();
    if (isGroupCollapsible(m_FormItem, false))
        connect(m_Group, SIGNAL(toggled(bool)), this, SLOT(expandGroup(bool)));

    // create itemdata
    m_ItemData = new BaseGroupData(formItem);
    m_ItemData->setBaseGroup(this);
    formItem->setItemData(m_ItemData);
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
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
        if (items.at(i)->formWidget()) {
            html << items.at(i)->formWidget()->printableHtml(withValues);
        }
    }
    // remove empty values
    html.removeAll("");

    // group is empty ?
    if (withValues) {
        if (html.isEmpty() && dontPrintEmptyValues(m_FormItem))
            return QString();
    }

    // recreate the grid as an html table
    int i = 0;
//    int c = 0;
    int r = 0;
    int previousrow = 0;
    foreach(const QString &s, html) {
//        c = (i % numberColumns);
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
    QString title = QString("<thead>"
                            "<tr>"
                            "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\" colspan=%1>"
                            "%2"
                            "</td>"
                            "</tr>"
                            "</thead>")
            .arg(numberColumns)
            .arg(m_FormItem->spec()->label());
    if (m_FormItem->getOptions().contains("DontPrintTitle", Qt::CaseInsensitive)) {
        title.clear();
    }

    return QString("<table width=100% border=1 cellpadding=0 cellspacing=0 style=\"margin: 5px 0px 0px 0px\">"
                   "%1"
                   "<tbody>"
                   "<tr>"
                   "<td>"
                   "<table width=100% border=0 cellpadding=0 cellspacing=0 style=\"margin: 0px\">"
                   "<tbody>"
                   "<tr>"
                   "%2"
                   "</tr>"
                   "</tbody>"
                   "</table>"
                   "</td>"
                   "</tr>"
                   "</tbody>"
                   "</table>")
            .arg(title)
            .arg(content);
}

void BaseGroup::retranslate()
{
    m_Group->setTitle(m_FormItem->spec()->label());
}

void BaseGroup::getCheckAndCollapsibleState()
{
    if (isGroupCheckable(m_FormItem, false)) {
        m_Group->setCheckable(true);
        m_Group->setChecked(isGroupChecked(m_FormItem,false));
    }

    if (isGroupCollapsible(m_FormItem, false)) {
        m_Group->setCheckable(true);
        if (isGroupExpanded(m_FormItem, false)) {
            m_Group->setChecked(true);
            expandGroup(true);
        } else {
            m_Group->setChecked(false);
            expandGroup(false);
        }
    }
}

void BaseGroup::expandGroup(bool expanded)
{
    // show/hide direct children
    foreach(QObject* child, m_Group->children()) {
        if (child->isWidgetType())
            static_cast<QWidget*>(child)->setVisible(expanded);
    }
    m_Group->setFlat(!expanded);
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseGroupData::BaseGroupData(Form::FormItem *item) :
        m_FormItem(item), m_BaseGroup(0)
{}

BaseGroupData::~BaseGroupData()
{}

void BaseGroupData::setBaseGroup(BaseGroup *gr)
{
    m_BaseGroup = gr;
    m_OriginalValue_isChecked = gr->m_Group->isChecked();
    clear();
}

void BaseGroupData::clear()
{
    if (isGroupCollapsible(m_FormItem, false))
        m_BaseGroup->getCheckAndCollapsibleState();
}

bool BaseGroupData::isModified() const
{
    if (isGroupCollapsible(m_FormItem, false) || isGroupCheckable(m_FormItem, false))
        return m_OriginalValue_isChecked != m_BaseGroup->m_Group->isChecked();
    return false;
}

bool BaseGroupData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (!m_BaseGroup)
        return false;
    if (role==Qt::CheckStateRole) {
        if (isGroupCollapsible(m_FormItem, false)) {
            m_BaseGroup->m_Group->setChecked(data.toBool());
            m_BaseGroup->expandGroup(data.toBool());
            onValueChanged();
        }
        else if (isGroupCheckable(m_FormItem, false)) {
            m_BaseGroup->m_Group->setChecked(data.toBool());
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseGroupData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    if (role==Qt::CheckStateRole)
        if (isGroupCollapsible(m_FormItem, false) || isGroupCheckable(m_FormItem, false))
            return m_BaseGroup->m_Group->isChecked();
    return QVariant();
}

void BaseGroupData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    m_OriginalValue_isChecked = data.toBool();
    setData(0, m_OriginalValue_isChecked, Qt::CheckStateRole);
}

QVariant BaseGroupData::storableData() const
{
    return data(0, Qt::CheckStateRole);
}

void BaseGroupData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseCheck ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCheck::BaseCheck(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Check(0)
{
    setObjectName("BaseCheck");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QCheckBox *chk = qFindChild<QCheckBox*>(formItem->parentFormMain()->formWidget(), widget);
        if (chk) {
            m_Check = chk;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Check = new QCheckBox(this);
        }
    } else {
        QHBoxLayout * hb = new QHBoxLayout(this);
        // Add Buttons
        m_Check = new QCheckBox(this);
        m_Check->setObjectName("Checkbox_" + m_FormItem->uuid());
        hb->addWidget(m_Check);
    }

    // Check options
    if (formItem->getOptions().contains("onright", Qt::CaseInsensitive)) {
        m_Check->setLayoutDirection(Qt::RightToLeft);
    }

    retranslate();

    // create itemdata
    m_ItemData = new BaseCheckData(formItem);
    m_ItemData->setCheckBox(m_Check);
    formItem->setItemData(m_ItemData);
    connect(m_Check, SIGNAL(clicked()), m_ItemData, SLOT(onValueChanged()));
}

BaseCheck::~BaseCheck()
{
}

QString BaseCheck::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
    if (m_Check) {
        // Get tooltip
        QString tip;
        if (m_FormItem->getOptions().contains("labelastooltip", Qt::CaseInsensitive)) {
            tip = m_FormItem->spec()->label();
        } else {
            tip = m_FormItem->spec()->value(Form::FormItemSpec::Spec_Tooltip).toString();
        }
        m_Check->setToolTip(tip);

        // Get label
        m_Check->setText(m_FormItem->spec()->label());
    }
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
    Q_UNUSED(ref);
//    qWarning() << "BaseCheckData::setData" << data << role;
    if (!m_Check)
        return false;
    if (role==Qt::EditRole || role==Qt::CheckStateRole) {
        if (data.canConvert(QVariant::Int))  {
            switch (data.toInt()) {
            case 0:
                m_Check->setCheckState(Qt::Unchecked);
                break;
            case 1:
                m_Check->setCheckState(Qt::PartiallyChecked);
                break;
            case 2:
                m_Check->setCheckState(Qt::Checked);
                break;
            }
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseCheckData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    if (role==Qt::CheckStateRole)
        return m_Check->checkState();
    if (role==Form::IFormItemData::CalculationsRole) {
        const QStringList &vals = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Numerical);
        if (m_Check->isChecked() && vals.count() >= 1)
            return vals.at(0);
        if (vals.count()>=2)
            return vals.at(1);
    }
    return Qt::Unchecked;
}

void BaseCheckData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    Qt::CheckState state = Qt::CheckState(data.toInt());
    m_Check->setCheckState(state);
    m_OriginalValue = state;
//    onValueChanged();
}

QVariant BaseCheckData::storableData() const
{
    return m_Check->checkState();
}

void BaseCheckData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseRadio ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseRadio::BaseRadio(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_ButGroup(0)
{
    setObjectName("BaseRadio");
    QLayout *radioLayout = 0;
    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find widget
        radioLayout = qFindChild<QLayout*>(formItem->parentFormMain()->formWidget(), layout);
        if (!radioLayout) {
            radioLayout = new QHBoxLayout(this);
        }
        m_Label = findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout *hb = getBoxLayout(labelAlignement(formItem, Label_OnTop), m_FormItem->spec()->label(), this);

        // Add QLabel
        //    m_Label->setSizePolicy(QSizePolicy::Preferred , QSizePolicy::Preferred);
        hb->addWidget(m_Label);

        // Add Buttons
        QGroupBox *gb = new QGroupBox(this);
        //     gb->setFlat(true);
        //     QSizePolicy sizePolicy = gb->sizePolicy();
        //     sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
        //     gb->setSizePolicy(sizePolicy);
        if (isRadioHorizontalAlign(m_FormItem)) {
            radioLayout = new QBoxLayout(QBoxLayout::LeftToRight, gb);
        } else {
            radioLayout = new QBoxLayout(QBoxLayout::TopToBottom, gb);
        }
    //    qWarning() << isRadioHorizontalAlign(m_FormItem);
        radioLayout->setContentsMargins(1, 0, 1, 0);
        hb->addWidget(gb);
    }

    m_ButGroup = new QButtonGroup(this);
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

    // create the FormItemData
    BaseRadioData *data = new BaseRadioData(m_FormItem);
    data->setBaseRadio(this);
    m_FormItem->setItemData(data);

    connect(m_ButGroup,SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(m_ButGroup, SIGNAL(buttonClicked(QAbstractButton*)), data, SLOT(onValueChanged()));
}

BaseRadio::~BaseRadio()
{}

QString BaseRadio::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
    if (m_Label)
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
    formItem()->itemData()->setData(0, radio->property("id"), Form::IFormItemData::CalculationsRole);
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
    Q_UNUSED(ref);
    Q_UNUSED(data);
    // receive ref=0; data=uid of activated radio; role=IFormItemData::RoleRepresentation
//    qWarning() << "BaseRadioData::setData" << data << role;
    if (role==Form::IFormItemData::CalculationsRole) {
        Q_EMIT dataChanged(ref); // just emit the dataChanged signal
        onValueChanged();
    }
    return true;
}

QVariant BaseRadioData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    if (role==Form::IFormItemData::CalculationsRole) {
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
    } else if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
        foreach(QRadioButton *but, m_Radio->m_RadioList) {
            if (but->isChecked()) {
                return but->text();
            }
        }
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
//    onValueChanged(); ?
}

QVariant BaseRadioData::storableData() const
{
    foreach(QRadioButton *but, m_Radio->m_RadioList) {
        if (but->isChecked())
            return but->property("id");
    }
    // TODO: return the default value ?
    return QVariant();
}

void BaseRadioData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- BaseSimpleText -------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSimpleText::BaseSimpleText(Form::FormItem *formItem, QWidget *parent, bool shortText) :
    Form::IFormWidget(formItem,parent), m_Line(0), m_Text(0)
{
    setObjectName("BaseSimpleText");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        if (shortText) {
            QLineEdit *le = qFindChild<QLineEdit*>(formItem->parentFormMain()->formWidget(), widget);
            if (le) {
                m_Line = le;
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
                // To avoid segfaulting create a fake combo
                m_Line = new QLineEdit(this);
            }
        } else {
            QTextEdit *te = qFindChild<QTextEdit*>(formItem->parentFormMain()->formWidget(), widget);
            if (te) {
                m_Text = te;
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
                // To avoid segfaulting create a fake combo
                m_Text = new QTextEdit(this);
            }
        }
        // Find Label
        m_Label = findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        if (shortText) {
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
    }

    // Create the FormItemData
    BaseSimpleTextData *data = new BaseSimpleTextData(m_FormItem);
    data->setBaseSimpleText(this);
    m_FormItem->setItemData(data);
    if (m_Line)
        connect(m_Line, SIGNAL(textChanged(QString)), data, SLOT(onValueChanged()));
    if (m_Text)
        connect(m_Text, SIGNAL(textChanged()), data, SLOT(onValueChanged()));
}

BaseSimpleText::~BaseSimpleText()
{
}

QString BaseSimpleText::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

    if (withValues) {
        if (dontPrintEmptyValues(m_FormItem)) {
            if (m_Line && m_Line->text().isEmpty())
                return QString();
            else if (m_Text && m_Text->toPlainText().isEmpty())
                return QString();
        }
        QString text;
        if (m_Line)
            text = m_Line->text();
        if (m_Text)
            text = m_Text->toHtml();
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
                .arg(m_FormItem->spec()->label()).arg(text.remove("</body>").remove("</html>"));
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
    if (m_Label)
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
    Q_UNUSED(ref);
//    WARN_FUNC << ref << role << data;
    if (role==Qt::EditRole) {
        if (m_Text->m_Line) {
            m_Text->m_Line->setText(data.toString());
            onValueChanged();
        } else if (m_Text->m_Text) {
            m_Text->m_Text->setText(data.toString());
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseSimpleTextData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
        if (m_Text->m_Line)
            return m_Text->m_Line->text();
        else if (m_Text->m_Text)
            return m_Text->m_Text->toPlainText();
    }
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

void BaseSimpleTextData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseHelpText -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseHelpText::BaseHelpText(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent)
{
    setObjectName("BaseHelpText");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    const QString &label = formItem->spec()->value(Form::FormItemSpec::Spec_UiLabel).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QLabel *le = qFindChild<QLabel*>(formItem->parentFormMain()->formWidget(), widget);
        if (le) {
            m_Label = le;
        } else {
            m_Label = new QLabel(this);
        }
        m_Label->setText(m_FormItem->spec()->label());
    } else if (!label.isEmpty()) {
        m_Label = findLabel(formItem);
    } else {
        QHBoxLayout *hb = new QHBoxLayout(this);
        // Add QLabel
        createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
        // Setting objectName to hide/show via a simple option button
        m_Label->setObjectName("HelpText_" + m_FormItem->uuid());
        hb->addWidget(m_Label);
    }
}

BaseHelpText::~BaseHelpText()
{
}

QString BaseHelpText::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();
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

void BaseHelpText::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseLists --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseList::BaseList(Form::FormItem *formItem, QWidget *parent, bool uniqueList) :
    Form::IFormWidget(formItem,parent), m_List(0)
{
    setObjectName("BaseList");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QListView *le = qFindChild<QListView*>(formItem->parentFormMain()->formWidget(), widget);
        if (le) {
            m_List = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_List = new QListView(this);
        }
        // Find Label
        m_Label = findLabel(formItem);
    } else {
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
    }

    // create FormItemData
    BaseListData *data = new BaseListData(m_FormItem);
    data->setBaseList(this);
    m_FormItem->setItemData(data);

    connect(m_List->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), data, SLOT(onValueChanged()));
}

BaseList::~BaseList()
{
}

QString BaseList::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
    if (m_Label)
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
    onValueChanged();
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
    Q_UNUSED(ref);
    if (role!=Qt::EditRole) {
        setSelectedItems(data.toStringList().join("`@`"));
    }
    return true;
}

QVariant BaseListData::data(const int ref, const int role) const
{
//    if (role==Form::IFormItemData::CalculationsRole) {
//        // return selected value::numerical (if exists)
//        QString selectedUid;
//        foreach(QRadioButton *but, m_Radio->m_RadioList) {
//            if (but->isChecked()) {
//                selectedUid = but->property("id").toString();
//                break;
//            }
//        }
//        int id = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(selectedUid);
//        const QStringList &vals = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Numerical);
//        if (id < vals.count() && id >= 0)
//            return vals.at(id);
//    } else
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
        QStringList selected;
        QItemSelectionModel *selModel = m_List->m_List->selectionModel();
        if (!selModel->hasSelection())
            return QVariant();
        if (ref==Form::IFormItemData::ID_CurrentUuid) {
            const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
            foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
                selected.append(uuids.at(idx.row()));
            }
        } else {
            foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
                selected.append(idx.data().toString());
            }
        }
        return selected;
    }
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

void BaseListData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseCombo --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCombo::BaseCombo(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Combo(0)
{
    setObjectName("BaseCombo");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QComboBox *cbx = qFindChild<QComboBox*>(formItem->parentFormMain()->formWidget(), widget);
        if (cbx) {
            m_Combo = cbx;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Combo = new QComboBox(this);
        }
        // Find label
        m_Label = findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout *hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        m_Combo = new QComboBox(this);
        m_Combo->setObjectName("Combo_" + m_FormItem->uuid());
        hb->addWidget(m_Combo);
    }
    m_Combo->addItems(m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible));

    // create FormItemData
    BaseComboData *data = new BaseComboData(m_FormItem);
    data->setBaseCombo(this);
    m_FormItem->setItemData(data);

    connect(m_Combo, SIGNAL(currentIndexChanged(int)), data, SLOT(onValueChanged()));
}

BaseCombo::~BaseCombo()
{
}

QString BaseCombo::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
    if (m_Label)
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
    if (role!=Qt::EditRole)
        return false;
    if (ref==Form::IFormItemData::ID_CurrentUuid) {
        int id = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(data.toString());
        m_Combo->m_Combo->setCurrentIndex(id);
        onValueChanged();
    }
    return true;
}

QVariant BaseComboData::data(const int ref, const int role) const
{
    int id = m_Combo->m_Combo->currentIndex();
    if (ref==Form::IFormItemData::ID_CurrentUuid) {
        if (id>=0)
            return parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(id);

    }
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
        return m_Combo->m_Combo->currentText();
    }
    if (role==Form::IFormItemData::CalculationsRole) {
        const QStringList &vals = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Numerical);
        if (id < vals.count() && id >= 0)
            return vals.at(id);
    }
    return QVariant();
}

void BaseComboData::setStorableData(const QVariant &data)
{
    m_OriginalValue = selectedItem(data.toString());
}

QVariant BaseComboData::storableData() const
{
    int row = m_Combo->m_Combo->currentIndex();
    if (row < 0 || row >= m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).count())
        return QVariant();
    return m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(row);
}

void BaseComboData::onValueChanged()
{
//    WARN_FUNC;
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(Form::IFormItemData::ID_CurrentUuid);
}
//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseDate ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseDate::BaseDate(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Date(0)
{
    setObjectName("BaseDate");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QDateTimeEdit *le = qFindChild<QDateTimeEdit*>(formItem->parentFormMain()->formWidget(), widget);
        if (le) {
            m_Date = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Date = new QDateTimeEdit(this);
        }
        // Find Label
        m_Label = findLabel(formItem);
    } else {    // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add Date selector and manage date format
        m_Date = new QDateTimeEdit(this);
        m_Date->setObjectName("Date_" + m_FormItem->uuid());
        m_Date->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        m_Date->setCalendarPopup(true);
        hb->addWidget(m_Date);
    }
    m_Date->setDisplayFormat(getDateFormat(m_FormItem));

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
    m_FormItem->setItemData(data);

    connect(m_Date, SIGNAL(dateChanged(QDate)), data, SLOT(onValueChanged()));
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
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
                .arg(m_FormItem->spec()->label()).arg(QLocale().toString(m_Date->date(), getDateFormat(m_FormItem)).replace(" ","&nbsp;"));
    }
    return content;
}

void BaseDate::retranslate()
{
    if (m_Label)
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
    onValueChanged();
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
    Q_UNUSED(ref);
    if (role==Qt::EditRole) {
        if (data.canConvert<QDate>()) {
            m_Date->m_Date->setDate(data.toDate());
            onValueChanged();
        } else if (data.canConvert<QDateTime>()) {
            m_Date->m_Date->setDateTime(data.toDateTime());
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseDateData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_Date->m_Date->dateTime();
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

void BaseDateData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
//--------------------------------------------------------------------------------------------------------
//------------------------------------------ BaseSpin --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSpin::BaseSpin(Form::FormItem *formItem, QWidget *parent, bool doubleSpin) :
    Form::IFormWidget(formItem,parent), m_Spin(0)
{
    setObjectName("BaseSpin");
    // create FormItemData
    BaseSpinData *data = new BaseSpinData(m_FormItem);

    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        if (doubleSpin) {
            QDoubleSpinBox *dbl = qFindChild<QDoubleSpinBox*>(formItem->parentFormMain()->formWidget(), widget);
            if (dbl) {
                m_Spin = dbl;
                connect(dbl, SIGNAL(valueChanged(double)), data, SLOT(onValueChanged()));
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
                // To avoid segfaulting create a fake spinbox
                m_Spin = new QDoubleSpinBox(this);
            }
        } else {
            QSpinBox *dbl = qFindChild<QSpinBox*>(formItem->parentFormMain()->formWidget(), widget);
            if (dbl) {
                m_Spin = dbl;
                connect(dbl, SIGNAL(valueChanged(double)), data, SLOT(onValueChanged()));
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
                // To avoid segfaulting create a fake spinbox
                m_Spin = new QSpinBox(this);
            }
        }
        m_Spin->setToolTip(m_FormItem->spec()->label());
        // Find label
        m_Label = findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add spin
        if (doubleSpin) {
            QDoubleSpinBox *spin = new QDoubleSpinBox(this);
            spin->setObjectName("DoubleSpin_" + m_FormItem->uuid());
            spin->setMinimum(formItem->extraData().value(::SPIN_EXTRAS_KEY_MIN, "0").toDouble());
            spin->setMaximum(formItem->extraData().value(::SPIN_EXTRAS_KEY_MAX, "10000").toDouble());
            spin->setSingleStep(formItem->extraData().value(::SPIN_EXTRAS_KEY_STEP, "0.1").toDouble());
            connect(spin, SIGNAL(valueChanged(double)), data, SLOT(onValueChanged()));
            m_Spin = spin;
        } else {
            QSpinBox *spin = new QSpinBox(this);
            spin->setObjectName("Spin_" + m_FormItem->uuid());
            spin->setMinimum(formItem->extraData().value(::SPIN_EXTRAS_KEY_MIN, "0").toInt());
            spin->setMaximum(formItem->extraData().value(::SPIN_EXTRAS_KEY_MAX, "10000").toInt());
            spin->setSingleStep(formItem->extraData().value(::SPIN_EXTRAS_KEY_STEP, "1").toInt());
            m_Spin = spin;
            connect(spin, SIGNAL(valueChanged(int)), data, SLOT(onValueChanged()));
        }
        m_Spin->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        hb->addWidget(m_Spin);
    }

    // manage options
    data->setBaseSpin(this);
    m_FormItem->setItemData(data);
}

BaseSpin::~BaseSpin()
{}

QString BaseSpin::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(::NOT_PRINTABLE))
        return QString();

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
    if (m_Spin)
        m_Spin->setToolTip(m_FormItem->spec()->label());
    if (m_Label)
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
    Q_UNUSED(ref);
    if (role==Qt::EditRole) {
        QSpinBox *spin = qobject_cast<QSpinBox*>(m_Spin->m_Spin);
        if (spin) {
            spin->setValue(data.toInt());
            onValueChanged();
            return true;
        }
        QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin->m_Spin);
        if (dspin) {
            dspin->setValue(data.toDouble());
            onValueChanged();
            return true;
        }
    }
    return false;
}

QVariant BaseSpinData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
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

void BaseSpinData::onValueChanged()
{
    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
//--------------------------------------------------------------------------------------------------------
//------------------------------------------ BaseButton ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseButton::BaseButton(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Button(0)
{
    setObjectName("BaseButton");

    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QPushButton *le = qFindChild<QPushButton*>(formItem->parentFormMain()->formWidget(), widget);
        if (le) {
            m_Button = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Button = new QPushButton(this);
        }
    } else {
        QHBoxLayout * hb = new QHBoxLayout(this);
        hb->addStretch();

        m_Button = new QPushButton(this);
        m_Button->setObjectName("Button_" + m_FormItem->uuid());
        m_Button->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        hb->addWidget(m_Button);
    }
    QString icon = formItem->spec()->value(Form::FormItemSpec::Spec_IconFileName).toString();
    if (!icon.isEmpty()) {
        if (icon.startsWith(Core::Constants::TAG_APPLICATION_THEME_PATH, Qt::CaseInsensitive)) {
            icon = icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
        }
        m_Button->setIcon(QIcon(icon));
    }
    m_Button->setText(m_FormItem->spec()->label());
    connect(m_Button, SIGNAL(clicked()) , this , SLOT(buttonClicked()));
}

BaseButton::~BaseButton()
{}

void BaseButton::buttonClicked()
{
    if (!m_FormItem->scripts()->onClicked().isEmpty())
        scriptManager()->evaluate(m_FormItem->scripts()->onClicked());
}

void BaseButton::retranslate()
{
    if (m_Button)
        m_Button->setText(m_FormItem->spec()->label());
}
