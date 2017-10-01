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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       Jerome Pinguet <jerome@jerome.cc>                                 *
 ***************************************************************************/
#include "baseformwidgets.h"
#include "baselistwidgets.h"
#include "frenchsocialnumber.h"
#include "basedetailswidget.h"
#include "basedatecompleterwidget.h"
#include "measurementwidget.h"
//#include "austriansocialnumber.h"
#include "constants.h"
#include "buttonmenupathitemview.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/emailvalidator.h>

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_current.h>

#include <QStringList>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QTime>
#include <QTimeEdit>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QStringListModel>
#include <QFileDialog>

#include <QtUiTools/QUiLoader>
#include <QBuffer>
#include <QDataWidgetMapper>

#include "ui_baseformwidget.h"

using namespace BaseWidgets;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}


namespace {
// TypeEnum must be sync with the widgetsName QStringList
    enum TypeEnum {
        Type_Undefined = 0,
        Type_Form,
        Type_Radio,
        Type_Check,
        Type_MultiCheck,
        Type_Combo,
        Type_UniqueList,
        Type_MultiList,
        Type_EditableList,
        Type_Spin,
        Type_DoubleSpin,
        Type_ShortText,
        Type_LongText,
        Type_HelpText,
        Type_File,
        Type_Group,
        Type_Date,
        Type_DateTime,
        Type_ModernDate,
        Type_Button,
        Type_DetailsWidget,
        Type_Measurement,
        Type_FrenchNSS,
        Type_AustrianSVNR,
        Type_ButtonMenuPathItemView,
        Type_MaxType
    };

    // names must be sync with the type enum
    static const QStringList widgetsName =
            QStringList() << "undef" << "form" << "radio" << "check" << "multicheck"
                          << "combo" << "uniquelist" << "multilist" << "editablelist"
                          << "spin" << "doublespin"
                          << "shorttext" << "longtext" << "helptext" << "file" << "group"
                          << "date" << "datetime" << "moderndate" << "button" << "detailswidget"
                          << "measurement" << "frenchnss" << "austriansvnr"
                          << "buttonmenupath";
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
    case ::Type_EditableList : return new BaseEditableStringList(formItem,parent);
    case ::Type_Combo : return new BaseCombo(formItem,parent);
    case ::Type_Date : return new BaseDate(formItem,parent);
    case ::Type_DateTime : return new BaseDateTime(formItem,parent);
    case ::Type_ModernDate : return new BaseDateCompleterWidget(formItem,parent);
    case ::Type_Spin : return new BaseSpin(formItem,parent);
    case ::Type_DoubleSpin : return new BaseSpin(formItem,parent,true);
    case ::Type_Button : return new BaseButton(formItem,parent);
    case ::Type_DetailsWidget: return new BaseDetailsWidget(formItem, parent);
    case ::Type_Measurement: return new MeasurementWidget(formItem, parent);
    case ::Type_FrenchNSS : return new FrenchSocialNumberFormWidget(formItem,parent);
//    case ::Type_AustrianSVNR : return new AustrianSocialNumberFormWidget(formItem,parent);
    case ::Type_ButtonMenuPathItemView : return new ButtonMenuPathItemView(formItem,parent);
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
    m_EpisodeDateTime(0),
    m_EpisodeLabel(0),
    m_PriorityButton(new QToolButton(this)),
    m_ContainerLayout(0),
    i(0), row(0), col(0), numberColumns(1),
    ui(0),
    aScreenshot(0),
    aHigh(0), aMedium(0), aLow(0)
{
    setObjectName("BaseForm");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // add a header with date, episode name and screenshot button - every episode has this
    QWidget *header = new QWidget(this);
    ui = new Ui::BaseFormWidget;
    ui->setupUi(header);

    m_EpisodeDateTime = ui->dateTimeEdit;
    m_EpisodeDateTime->setDisplayFormat(tkTr(Trans::Constants::DATETIMEFORMAT_FOR_EDITOR));

    m_EpisodeDateTime->setEnabled(false);
    m_EpisodeDateTime->setCalendarPopup(true);

    m_EpisodeLabel = ui->lineEdit;
    m_EpisodeLabel->setMaxLength(Constants::EPISODELABELMAXCHAR);
    m_EpisodeLabel->setEnabled(false);
    m_PriorityButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_PriorityButton->setPopupMode(QToolButton::InstantPopup);
    ui->lineEdit->setLeftButton(m_PriorityButton);
    ui->label->setText(m_FormItem->spec()->label());

    ui->toolButton->hide();

    // create main widget
    QWidget *mainWidget = 0;
    const QString &uiContent = formItem->spec()->value(Form::FormItemSpec::Spec_UiFileContent).toString();
    if (!uiContent.isEmpty()) {
        // Create the Ui using the QtUi file
        QUiLoader loader;
        QBuffer buf;
        buf.setData(uiContent.toUtf8());
        mainWidget = loader.load(&buf, this);

        // Manage options
        Form::FormMain *form = qobject_cast<Form::FormMain*>(formItem);
        if (form) {
            mainLayout->addWidget(header);
            if (!(form->episodePossibilities() == Form::FormMain::UniqueEpisode && Constants::hideHeaderOnUniqueEpisode(formItem))) {
                header->show();
            } else {
                header->hide();
            }
        }
    } else {
        mainLayout->setSpacing(0);
        mainLayout->setMargin(0);
        mainWidget = new QWidget(this);
        mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // create container layout
        m_ContainerLayout = new QGridLayout(mainWidget);
        // Retrieve the number of columns for the gridlayout (lays in extraData() of linked FormItem)
        numberColumns = Constants::getNumberOfColumns(m_FormItem);
        if (Constants::isCompactView(m_FormItem)) {
            mainLayout->setMargin(5);
            mainLayout->setSpacing(5);
            m_ContainerLayout->setMargin(5);
            m_ContainerLayout->setSpacing(5);
        }

        // Manage options
        Form::FormMain *form = qobject_cast<Form::FormMain*>(formItem);
        if (form) {
            m_ContainerLayout->addWidget(header, 0, 0, 1, numberColumns);
            if (!(form->episodePossibilities() == Form::FormMain::UniqueEpisode && Constants::hideHeaderOnUniqueEpisode(formItem))) {
                i = numberColumns * 2;
                header->show();
            } else {
                header->hide();
            }
        }
        row = 0;
        col = 0;
    }

    mainLayout->addWidget(mainWidget);
    createActions();
    hideAndClearValidationMessage();

    //Form::FormMain *form = qobject_cast<Form::FormMain*>(formItem);
    //Form::EpisodeManager *manager = new Form::EpisodeManager;
    //Form::EpisodeModel *model = manager->episodeModel(form);
    //QDataWidgetMapper *mapper = new QDataWidgetMapper;
    //mapper->setModel(model);
    //mapper->addMapping(m_EpisodeDateTime, Form::EpisodeModel::UserDateTime);
    //mapper->toFirst();

    // create itemdata
    BaseFormData *baseFormData = new BaseFormData(formItem);
    baseFormData->setForm(this);
    formItem->setItemData(baseFormData);

    retranslate();
}

BaseForm::~BaseForm()
{
    if (ui) {
        delete ui;
        ui = 0;
    }
}

/** Create the priority actions and populate the priority toolbutton */
void BaseForm::createActions()
{
    QAction *a;
    a = aHigh = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONPRIORITY_HIGH));
    a = aMedium = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONPRIORITY_MEDIUM));
    a = aLow = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONPRIORITY_LOW));
    m_PriorityButton->addAction(aHigh);
    m_PriorityButton->addAction(aMedium);
    m_PriorityButton->addAction(aLow);
    m_PriorityButton->setDefaultAction(aMedium);
//    connect(m_PriorityButton, SIGNAL(triggered(QAction*)), this, SLOT(priorityActionTriggered(QAction*)));
}

/** Return current priority according to the Form::EpisodeModel::Priority enum */
int BaseForm::currentPriority() const
{
    QAction *a = m_PriorityButton->defaultAction();
    if (a == aHigh)
        return 0;
    if (a == aMedium)
        return 1;
    if (a == aLow)
        return 2;
    return 2;
}

/** Set the priority according to the Form::EpisodeModel::Priority enum */
void BaseForm::setCurrentPriority(const int priority)
{
    switch (priority) {
    case 0: // HIGH
        m_PriorityButton->setDefaultAction(aHigh);
        break;
    case 1: // MEDIUM
        m_PriorityButton->setDefaultAction(aMedium);
        break;
    case 2: // LOW
        m_PriorityButton->setDefaultAction(aLow);
        break;
    }
}

void BaseForm::addWidgetToContainer(IFormWidget *widget)
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

void BaseForm::showValidationMessage(const QString &message)
{
    ui->validatedEpisodeLabel->setText(message);
    ui->validatedEpisodeLabel->setVisible(true);
}

void BaseForm::hideAndClearValidationMessage()
{
    ui->validatedEpisodeLabel->setText(QString::null);
    ui->validatedEpisodeLabel->setVisible(false);
}

QString BaseForm::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    QStringList html;
    QList<Form::FormItem*> items = m_FormItem->formItemChildren();
    for(int i = 0; i < items.count(); ++i) {
        Form::IFormWidget *w = items.at(i)->formWidget();
        if (w)
            html << Utils::htmlBodyContent(w->printableHtml(withValues));
    }
    html.removeAll("");
    if (html.isEmpty() && Constants::dontPrintEmptyValues(m_FormItem)){
        return QString();
    }

    int i = 0;
//    int c = 0;
    int previousrow = 0;
    QString header, content;
    // Start with the header of the form
    header += QString("<table width=100% border=2 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<td style=\"vertical-align: top;padding: 5px\">\n"
                    "<center><span style=\"font-weight: 600;\">%1</span><br />\n"
                    "%2"
                    "</center>\n"
                    "</td>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n")
            .arg(m_FormItem->spec()->label())
            .arg(m_EpisodeLabel->toolTip().replace("right", "center").replace("<p ", "<span ").replace("</p>", "</span>"));

    // recreate the grid as an html table
    foreach(const QString &s, html) {
//        c = (i % numberColumns);
        int r = (i / numberColumns);
        if (r>previousrow) {
            previousrow = r;
            content += "</tr><tr>";
        }
        content += QString("<td style=\"vertical-align: top; align: left\">\n"
                           "%1\n"
                           "</td>\n").arg(s);
        ++i;
    }

    return QString("%1\n"
                   "<table width=100% border=0 cellpadding=0 cellspacing=0 style=\"margin:0px\">\n"
                   "<tbody>\n"
                   "<tr>\n"
                   "%2\n"
                   "</tr>\n"
                   "</tbody>\n"
                   "</table>\n\n")
            .arg(header).arg(content);
}

void BaseForm::retranslate()
{
    if (ui)
        ui->label->setText(m_FormItem->spec()->label());
    if (aScreenshot) {
        aScreenshot->setText(tkTr(Trans::Constants::TAKE_SCREENSHOT));
        aScreenshot->setToolTip(tkTr(Trans::Constants::TAKE_SCREENSHOT));
    }
    if (aHigh) {
        aHigh->setText(Utils::firstLetterUpperCase(tkTr(Trans::Constants::HIGH)));
        aMedium->setText(Utils::firstLetterUpperCase(tkTr(Trans::Constants::MEDIUM)));
        aLow->setText(Utils::firstLetterUpperCase(tkTr(Trans::Constants::LOW)));
        m_PriorityButton->setToolTip(tkTr(Trans::Constants::PRIORITY));
    }
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseFormData::BaseFormData(Form::FormItem *item) :
    m_FormItem(item),
    m_Form(0),
    m_Modified(false)
{}

BaseFormData::~BaseFormData()
{}

void BaseFormData::setForm(BaseForm *form)
{
    m_Form = form;
    clear();
}

void BaseFormData::clear()
{
    m_Data.clear();
    m_Form->m_EpisodeDateTime->setDateTime(QDateTime());
    m_Form->m_EpisodeLabel->clear();
    m_Form->m_EpisodeLabel->setEnabled(false);
    m_Form->m_EpisodeDateTime->setEnabled(false);
    m_Form->hideAndClearValidationMessage();
}

bool BaseFormData::isModified() const
{
    // Force modified state
    if (m_Modified)
        return true;
    // Check values
    QList<int> keys;
    keys << ID_UserName << ID_EpisodeLabel << ID_EpisodeDateTime << ID_Priority;
    foreach(int id, keys) {
        if (data(id) != m_OriginalData.value(id)) {
            return true;
        }
    }
    return false;
}

void BaseFormData::setModified(bool modified)
{
    m_Modified = modified;
    if (!modified) {
        QList<int> keys;
        keys << ID_UserName << ID_EpisodeLabel << ID_EpisodeDateTime << ID_Priority;
        foreach(int id, keys) {
            m_OriginalData.insert(id, data(id));
        }
    }
}

/**
 * Switch the formitem widget and data to a readonly state (when the episode was validated).
 * Setting the readonly property to true will show a label informing user that the episode
 * was validated and can not be modified.\n
 * Warning this does not include all the children.
 */
void BaseFormData::setReadOnly(bool readOnly)
{
    m_Form->m_EpisodeLabel->setEnabled(!readOnly);
    m_Form->m_EpisodeDateTime->setEnabled(!readOnly);
    m_Form->m_PriorityButton->setEnabled(!readOnly);
    if (readOnly)
        m_Form->showValidationMessage(
                    QString("<span style='color: maroon;'><span style='font-weight: bold'>%1</span><br />%2</span>")
                    .arg(QApplication::translate("BaseFormData", "This episode is validated."))
                    .arg(QApplication::translate("BaseFormData", "You cannot edit its content, you cannot remove it."))
                    );
    else
        m_Form->hideAndClearValidationMessage();
}

bool BaseFormData::isReadOnly() const
{
    return (!m_Form->m_EpisodeLabel->isEnabled());
}

bool BaseFormData::setData(const int ref, const QVariant &data, const int role)
{
    if (role!=Qt::EditRole)
        return false;

    m_Data.insert(ref, data);
    m_Modified = true;
    switch (ref) {
    case ID_EpisodeDateTime:
        // If datetime record is stored as UTC in ISO format, it contains "Z"
        // and should be converted back to local time. Else, keep the datetime unchanged
        if (m_Data.value(ref).toString().contains("Z", Qt::CaseSensitive)){
            m_Form->m_EpisodeDateTime->setDateTime(m_Data.value(ref).toDateTime().toLocalTime());
        } else {
            m_Form->m_EpisodeDateTime->setDateTime(m_Data.value(ref).toDateTime());
        }
        m_Form->m_EpisodeDateTime->setEnabled(true);
        break;
    case ID_EpisodeLabel:
        m_Form->m_EpisodeLabel->setText(m_Data.value(ref).toString());
        m_Form->m_EpisodeLabel->setEnabled(true);
        break;
    case ID_Priority:
        m_Form->setCurrentPriority(data.toInt());
        break;
    }
    m_Form->m_EpisodeDateTime->setToolTip(QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                                      .arg(QLocale().toString(m_Data.value(ID_EpisodeDateTime).toDateTime(), QLocale::LongFormat).replace(" ","&nbsp;"))
                                       .arg(m_Data.value(ID_EpisodeLabel).toString().replace(" ", "&nbsp;"))
                                       .arg(m_Data.value(ID_UserName).toString().replace(" ", "&nbsp;")));
    m_Form->m_EpisodeLabel->setToolTip(QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                                       .arg(QLocale().toString(m_Data.value(ID_EpisodeDateTime).toDateTime(), QLocale::LongFormat).replace(" ","&nbsp;"))
                                       .arg(m_Data.value(ID_EpisodeLabel).toString().replace(" ", "&nbsp;"))
                                       .arg(m_Data.value(ID_UserName).toString().replace(" ", "&nbsp;")));
    return true;
}

/**
 * @brief BaseFormData::data return data of baseform widget
 * text() of QLineEdit m_EpisodeLabel being stored as varchar(200)in the database,
 * we need to make sure that only the first 200 characters of the string are returned.
 * @param ref
 * @param role
 * @return
 */
QVariant BaseFormData::data(const int ref, const int role) const
{
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
        switch (ref) {
        case ID_EpisodeDateTime: return m_Form->m_EpisodeDateTime->dateTime().toUTC().toString(Qt::ISODate);
        case ID_EpisodeLabel: return m_Form->m_EpisodeLabel->text().left(Constants::EPISODELABELMAXCHAR);
        case ID_UserName: return m_Data.value(ID_UserName);
        case ID_Priority: return m_Form->currentPriority();
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
        QGroupBox *grp = formItem->parentFormMain()->formWidget()->findChild<QGroupBox*>(widget);
        if (grp) {
            m_Group = grp;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake group
            m_Group = new QGroupBox(this);
        }
    } else {
        QVBoxLayout *vblayout = new QVBoxLayout(this);
        m_Group = new QGroupBox(this);
        vblayout->addWidget(m_Group);
        setLayout(vblayout);
        vblayout->setMargin(0);

        // Retrieve the number of columns for the gridlayout (lays in extraData() of linked FormItem)
        numberColumns = Constants::getNumberOfColumns(m_FormItem, 2);

        // Create the gridlayout with all the widgets
        m_ContainerLayout = new QGridLayout(m_Group);
        if (Constants::isCompactView(m_FormItem)) {
            m_ContainerLayout->setMargin(0);
            m_ContainerLayout->setSpacing(2);
        }
        m_Group->setLayout(m_ContainerLayout);
    }
    m_Group->setTitle(m_FormItem->spec()->label());

    // Check country specific options
    const QStringList &countries = Constants::getCountries(formItem);
    if (!countries.isEmpty()) {
        if (!countries.contains(QLocale().name().right(2), Qt::CaseInsensitive))
            this->hide();
    }

    getCheckAndCollapsibleState();
    if (Constants::isGroupCollapsible(m_FormItem, false)) {
        connect(m_Group, SIGNAL(toggled(bool)), this, SLOT(expandGroup(bool)));
    }

    // create itemdata
    m_ItemData = new BaseGroupData(formItem);
    m_ItemData->setBaseGroup(this);
    formItem->setItemData(m_ItemData);

    if (Constants::isGroupCollapsible(m_FormItem, false) || Constants::isGroupCheckable(m_FormItem, false)) {
        connect(m_Group, SIGNAL(toggled(bool)), this, SLOT(onToggled()));
    }
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
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    // Check country specific options
    const QStringList &countries = Constants::getCountries(m_FormItem);
    if (!countries.isEmpty()) {
        if (!countries.contains(QLocale().name().right(2), Qt::CaseInsensitive))
            return QString();
    }

    QStringList html;
    QString content;
    QList<Form::FormItem*> items = m_FormItem->formItemChildren();
    for(int i = 0; i < items.count(); ++i) {
        if (items.at(i)->formWidget()) {
            html << Utils::htmlBodyContent(items.at(i)->formWidget()->printableHtml(withValues));
        }
    }
    // remove empty values
    html.removeAll("");

    // group is empty ?
    if (withValues) {
        if (html.isEmpty() && Constants::dontPrintEmptyValues(m_FormItem))
            return QString();
    }

    // recreate the grid as an html table
    int i = 0;
//    int c = 0;
    int previousrow = 0;
    foreach(const QString &s, html) {
//        c = (i % numberColumns);
        int r = (i / numberColumns);
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
    const QSignalBlocker blocker(m_Group);

    if (Constants::isGroupCheckable(m_FormItem, false)) {
        m_Group->setCheckable(true);
        m_Group->setChecked(Constants::isGroupChecked(m_FormItem,false));
    }

    if (Constants::isGroupCollapsible(m_FormItem, false)) {
        m_Group->setCheckable(true);
        if (Constants::isGroupExpanded(m_FormItem, false)) {
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

void BaseGroup::onToggled()
{
    Constants::executeOnToggledScript(m_FormItem);
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
    if (Constants::isGroupCollapsible(m_FormItem, false))
        m_BaseGroup->getCheckAndCollapsibleState();
}

bool BaseGroupData::isModified() const
{
    if (Constants::isGroupCollapsible(m_FormItem, false) || Constants::isGroupCheckable(m_FormItem, false))
        return m_OriginalValue_isChecked != m_BaseGroup->m_Group->isChecked();
    return false;
}

void BaseGroupData::setModified(bool modified)
{
    if (!modified) {
        if (Constants::isGroupCollapsible(m_FormItem, false) || Constants::isGroupCheckable(m_FormItem, false))
            m_OriginalValue_isChecked = m_BaseGroup->m_Group->isChecked();
    }
}

void BaseGroupData::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
    // TODO: improve this (we only need to avoid checked state to change)
    //m_BaseGroup->m_Group->setEnabled(!readOnly);
}

bool BaseGroupData::isReadOnly() const
{
    return false;
    //return (!m_BaseGroup->m_Group->isEnabled());
}

bool BaseGroupData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (!m_BaseGroup)
        return false;
    if (role==Qt::CheckStateRole) {
        if (Constants::isGroupCollapsible(m_FormItem, false)) {
            m_BaseGroup->m_Group->setChecked(data.toBool());
            m_BaseGroup->expandGroup(data.toBool());
            onValueChanged();
        }
        else if (Constants::isGroupCheckable(m_FormItem, false)) {
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
        if (Constants::isGroupCollapsible(m_FormItem, false) || Constants::isGroupCheckable(m_FormItem, false))
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
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseCheck ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCheck::BaseCheck(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_Check(0)
{
    setObjectName("BaseCheck");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QCheckBox *chk = formItem->parentFormMain()->formWidget()->findChild<QCheckBox*>(widget);
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

    setFocusedWidget(m_Check);

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
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    // ⍌⎕☒☑
    if (withValues) {
        if (m_Check->isChecked()) {
            return QString("%1&nbsp;%2").arg("&#10003;").arg(m_FormItem->spec()->label());
        } else if (!m_FormItem->getOptions().contains("printonlychecked", Qt::CaseInsensitive)) {
            return QString("%1&nbsp;%2").arg("⎕").arg(m_FormItem->spec()->label());
        }
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
            tip = m_FormItem->spec()->tooltip();
        }
        m_Check->setToolTip(tip);
        m_Check->setText(m_FormItem->spec()->label());
    }
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseCheckData::BaseCheckData(Form::FormItem *item) :
    m_FormItem(item),
    m_Check(0),
    m_OriginalValue(Qt::Unchecked)
{
}

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
}

bool BaseCheckData::isModified() const
{
    return m_OriginalValue != m_Check->checkState();
}

void BaseCheckData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_Check->checkState();
}

void BaseCheckData::setReadOnly(bool readOnly)
{
    m_Check->setEnabled(!readOnly);
}

bool BaseCheckData::isReadOnly() const
{
    return (!m_Check->isEnabled());
}

/**
 * To keep compatibility with FreeMedForms:
 *   - Qt::Checked is recorded as 2 in the database
 *   - Qt::Unchecked is recored as 0 in the database
 *   - Qt::PartiallyChecked is not used in FreeHealth
 * Tristate was never implemented in FreeMedForms as the default property
 * isTristate() for a QCheckBox is false.
 */
bool BaseCheckData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (!m_Check)
        return false;
    if (role==Qt::EditRole || role==Qt::CheckStateRole) {
        if (data.canConvert(QVariant::Int))  {
            switch (data.toInt()) {
            case 0:
                m_Check->setCheckState(Qt::Unchecked);
                break;
            case 1:
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
    if (role==Qt::CheckStateRole) {
        return m_Check->checkState();
    } else if (role==Form::IFormItemData::CalculationsRole) {
        const QStringList &vals = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Numerical);
        if (m_Check->isChecked() && vals.count() >= 1)
            return vals.at(0);
        if (vals.count()>=2)
            return vals.at(1);
    } else if (role == Form::IFormItemData::PrintRole) {
        // Check if a Print value is defined for this item
        QString toPrint = m_FormItem->spec()->label();
        const QStringList &vals = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Printing);
        if (vals.count() >= 1 && !vals.at(0).isEmpty()) {
            toPrint = vals.at(0);
        }
        if (vals.count() >= 2 && !vals.at(1).isEmpty())
            toPrint = vals.at(1);

        // ⍌⎕☑
        if (m_Check->isChecked()) {
            return QString("%1&nbsp;%2").arg("&#10003;").arg(toPrint);
        } else {
            if (!m_FormItem->getOptions().contains("printonlychecked", Qt::CaseInsensitive))
                return QString("%1&nbsp;%2").arg("⎕").arg(toPrint);
        }
        return QVariant();
    }
    return QVariant();
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
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseRadio ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseRadio::BaseRadio(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_ButGroup(0)
{
    setObjectName("BaseRadio");
    QBoxLayout *radioLayout = 0;
    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find widget
        radioLayout = formItem->parentFormMain()->formWidget()->findChild<QBoxLayout*>(layout);
        if (!radioLayout) {
            radioLayout = new QHBoxLayout(this);
        }
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout *hb = getBoxLayout(Constants::labelAlignement(formItem, OnTop), m_FormItem->spec()->label(), this);

        // Add QLabel
        hb->addWidget(m_Label);

        // Add Buttons
        QGroupBox *gb = new QGroupBox(this);
        if (Constants::isRadioHorizontalAlign(m_FormItem)) {
            radioLayout = new QBoxLayout(QBoxLayout::LeftToRight, gb);
        } else {
            radioLayout = new QBoxLayout(QBoxLayout::TopToBottom, gb);
        }
        radioLayout->setContentsMargins(1, 0, 1, 0);
        hb->addWidget(gb);
    }

    m_ButGroup = new QButtonGroup(this);
    int i = 0;
    const QStringList &uids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    foreach (const QString &v, m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible)) {
        QRadioButton *rb = new QRadioButton(this);
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

    // For compact view, add a spaceritem inside the layout
    if (Constants::isCompactView(formItem)) {
        // TODO: manage a new option for right ou left compacting
        radioLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    }

    if (m_RadioList.count() >= 1)
        setFocusedWidget(m_RadioList.at(0));

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
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    if (Constants::dontPrintEmptyValues(m_FormItem) && m_ButGroup->checkedButton()==0)
        return QString();

    // ⚪⚫ characters encoded as hexadecimal reference for multi OS compatibility
    QStringList html;
    bool horiz = Constants::isRadioHorizontalAlign(m_FormItem);
    foreach (QRadioButton *button, m_RadioList) {
        if (withValues) {
            if (button->isChecked()) {
                html << QString("%1&nbsp;%2").arg(QString::fromUtf8("\u26AB")).arg(button->text());
                continue;
            }
        }
        html << QString("%1&nbsp;%2").arg(QString::fromUtf8("\u26AC")).arg(button->text());
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
    // TODO manage formitem spec tooltip for each button
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
            return m_OriginalValue != but->property("id").toString();
        }
    }
    return true;
}

void BaseRadioData::setModified(bool modified)
{
    if (!modified) {
        foreach(QRadioButton *but, m_Radio->m_RadioList) {
            if (but->isChecked()) {
                m_OriginalValue = but->property("id").toString();
                return;
            }
        }
    }
}

void BaseRadioData::setReadOnly(bool readOnly)
{
    foreach(QRadioButton *but, m_Radio->m_RadioList)
        but->setEnabled(!readOnly);
}

bool BaseRadioData::isReadOnly() const
{
    if (m_Radio->m_RadioList.isEmpty())
        return false;
    return (!m_Radio->m_RadioList.at(0)->isEnabled());
}

bool BaseRadioData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
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
        if (IN_RANGE_STRICT_MAX(id, 0, vals.count()))
            return vals.at(id);
    } else if (role==Qt::DisplayRole
               || role==Form::IFormItemData::PatientModelRole
               || role==Form::IFormItemData::PrintRole) {
        foreach(QRadioButton *but, m_Radio->m_RadioList) {
            if (but->isChecked()) {
                if (role == Form::IFormItemData::PrintRole) {
                    // Check if a Print value is defined for this radio item
                    QString selectedUid = but->property("id").toString();
                    QString toPrint = but->text();
                    int id = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(selectedUid);
                    const QStringList &vals = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Printing);
                    if (IN_RANGE_STRICT_MAX(id, 0, vals.count()))
                        toPrint = vals.at(id);
                    return toPrint;
                }
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
    Constants::executeOnValueChangedScript(m_FormItem);
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- BaseSimpleText -------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSimpleText::BaseSimpleText(Form::FormItem *formItem, QWidget *parent, bool shortText) :
    Form::IFormWidget(formItem,parent),
    m_Line(0),
    m_Text(0)
{
    setObjectName("BaseSimpleText");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        if (shortText) {
            QLineEdit *le = formItem->parentFormMain()->formWidget()->findChild<QLineEdit*>(widget);
            if (le) {
                m_Line = le;
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
                // To avoid segfaulting create a fake combo
                m_Line = new QLineEdit(this);
            }
        } else {
            QTextEdit *te = formItem->parentFormMain()->formWidget()->findChild<QTextEdit*>(widget);
            if (te) {
                m_Text = te;
            } else {
                LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
                // To avoid segfaulting create a fake combo
                m_Text = new QTextEdit(this);
            }
        }
        // Find Label
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        if (shortText) {
            m_Line = new QLineEdit(this);
            m_Line->setObjectName("Line_" + m_FormItem->uuid());
            m_Line->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
            hb->addWidget(m_Line);
        } else {
            m_Text = new QTextEdit(this);
            m_Text->setObjectName("Text_" + m_FormItem->uuid());
            m_Text->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
            hb->addWidget(m_Text);
        }
    }

    if (m_Text) {
        setFocusedWidget(m_Text);
    } else if (m_Line) {
        setFocusedWidget(m_Line);
        // Read options
        // validator
        if (formItem->extraData().contains(Constants::EXTRAS_LINEEDIT_VALIDATOR)) {
            const QString &value = formItem->extraData().value(Constants::EXTRAS_LINEEDIT_VALIDATOR);
            if (value.compare("email", Qt::CaseInsensitive)==0
                    || value.compare("mail", Qt::CaseInsensitive)==0
                    || value.compare("e-mail", Qt::CaseInsensitive)==0) {
                m_Line->setValidator(new Utils::EmailValidator(m_Line));
            } else if (!value.isEmpty()) {
                m_Line->setValidator(new QRegExpValidator(QRegExp(value), this));
            }
        }
        // inputmask
        if (formItem->extraData().contains(Constants::EXTRAS_LINEEDIT_MASK)) {
            m_Line->setInputMask(formItem->extraData().value(Constants::EXTRAS_LINEEDIT_MASK));
        }
        // manage placeholder
        if (!formItem->spec()->placeHolder().isEmpty())
            m_Line->setPlaceholderText(formItem->spec()->placeHolder());
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
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    if (withValues) {
        if (Constants::dontPrintEmptyValues(m_FormItem)) {
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
    if (m_Line)
        m_Line->setToolTip(m_FormItem->spec()->tooltip());
    if (m_Text)
        m_Text->setToolTip(m_FormItem->spec()->tooltip());
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

void BaseSimpleTextData::setModified(bool modified)
{
    if (!modified) {
        if (m_Text->m_Line)
            m_OriginalValue = m_Text->m_Line->text();
        else if (m_Text->m_Text)
            m_OriginalValue = m_Text->m_Text->toPlainText();
    }
}

void BaseSimpleTextData::setReadOnly(bool readOnly)
{
    if (m_Text->m_Line)
        m_Text->m_Line->setReadOnly(readOnly);
    if (m_Text->m_Text)
        m_Text->m_Text->setReadOnly(readOnly);
}

bool BaseSimpleTextData::isReadOnly() const
{
    if (m_Text->m_Line)
        return m_Text->m_Line->isReadOnly();
    if (m_Text->m_Text)
        return m_Text->m_Text->isReadOnly();
    return false;
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
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole || role==Form::IFormItemData::PrintRole) {
        if (m_Text->m_Line) {
            return m_Text->m_Line->text();
        } else if (m_Text->m_Text) {
            if (m_FormItem->getOptions().contains("html", Qt::CaseInsensitive)) {
                return Utils::htmlBodyContent(m_Text->m_Text->toHtml());
            } else {
                return m_Text->m_Text->toPlainText();
            }
        }
    }
    return QVariant();
}

void BaseSimpleTextData::setStorableData(const QVariant &data)
{
    m_OriginalValue = data.toString();
    if (m_Text->m_Line)
        m_Text->m_Line->setText(m_OriginalValue);
    else if (m_Text->m_Text) {
        if (m_FormItem->getOptions().contains("html", Qt::CaseInsensitive))
            m_Text->m_Text->setHtml(m_OriginalValue);
        else
            m_Text->m_Text->setPlainText(m_OriginalValue);
    }
}

QVariant BaseSimpleTextData::storableData() const
{
    if (m_Text->m_Line)
        return m_Text->m_Line->text();
    else if (m_Text->m_Text) {
        if (m_FormItem->getOptions().contains("html", Qt::CaseInsensitive))
            return m_Text->m_Text->toHtml();
        else
            return m_Text->m_Text->toPlainText();
    }
    return QVariant();
}

void BaseSimpleTextData::onValueChanged()
{
    Constants::executeOnValueChangedScript(m_FormItem);
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
        QLabel *le = formItem->parentFormMain()->formWidget()->findChild<QLabel*>(widget);
        if (le) {
            m_Label = le;
        } else {
            m_Label = new QLabel(this);
        }
        m_Label->setText(m_FormItem->spec()->label());
    } else if (!label.isEmpty()) {
        m_Label = Constants::findLabel(formItem);
    } else {
        QHBoxLayout *hb = new QHBoxLayout(this);
        // Add QLabel
        createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
        // Setting objectName to hide/show via a simple option button
        m_Label->setObjectName("HelpText_" + m_FormItem->uuid());
        hb->addWidget(m_Label);
    }
    m_Label->setToolTip(m_FormItem->spec()->tooltip());
}

BaseHelpText::~BaseHelpText()
{
}

QString BaseHelpText::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
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
    if (m_Label) {
        m_Label->setText(m_FormItem->spec()->label());
        m_Label->setToolTip(m_FormItem->spec()->tooltip());
    }
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseDate ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//TODO: create a function that returns default time 
BaseDate::BaseDate(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Date(0)
{
    setObjectName("BaseDate");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QDateEdit *le = formItem->parentFormMain()->formWidget()->findChild<QDateEdit*>(widget);
        if (le) {
            m_Date = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Date = new QDateEdit(this);
        }
        // Find Label
        m_Label = Constants::findLabel(formItem);
    } else {    // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add Date selector and manage date format
        m_Date = new QDateEdit(this);
        m_Date->setObjectName("Date_" + m_FormItem->uuid());
        m_Date->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        m_Date->setCalendarPopup(true);
        hb->addWidget(m_Date);
    }
    m_Date->setDisplayFormat(Constants::getDateFormat(m_FormItem));
    setFocusedWidget(m_Date);

    // Manage options
    const QStringList &options = formItem->getOptions();
    if (options.contains(Constants::DATE_NOW, Qt::CaseInsensitive)) {
        m_Date->setDate(QDate::currentDate());
    } else {
        QDate defaultDate = QDate::fromString(Constants::DEFAULT_DATE, Constants::DEFAULT_DATE_FORMAT);
        m_Date->setDate(defaultDate);
    }
    if (options.contains(Constants::DATE_PATIENTLIMITS, Qt::CaseInsensitive)) {
        connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
        onCurrentPatientChanged();
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

void BaseDate::onCurrentPatientChanged()
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
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
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
        QString defaultDateFormat = Constants::DEFAULT_DATE_FORMAT;
        QString defaultDate = Constants::DEFAULT_DATE;
        QString widgetDate = m_Date->date().toString(defaultDateFormat);
        bool dateIsDefault(widgetDate == defaultDate);
        if (Constants::dontPrintEmptyValues(m_FormItem) && dateIsDefault)
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
                .arg(m_FormItem->spec()->label())
                .arg(QLocale().toString(m_Date->date(), Constants::getDateFormat(m_FormItem)).replace(" ","&nbsp;"));
    }
    return content;
}

void BaseDate::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (m_Date)
        m_Date->setToolTip(m_FormItem->spec()->tooltip());
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
    m_Date->m_Date->setDate(QDate::fromString(s, Qt::ISODate));
    onValueChanged();
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseDateData::clear()
{
    QDate defaultDate = QDate::fromString(Constants::DEFAULT_DATE, Constants::DEFAULT_DATE_FORMAT);
    m_OriginalValue = defaultDate.toString(Qt::ISODate);
    setDate(m_OriginalValue);
}

bool BaseDateData::isModified() const
{
    return m_OriginalValue != m_Date->m_Date->date().toString(Qt::ISODate);
}

void BaseDateData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_Date->m_Date->date().toString(Qt::ISODate);
}

void BaseDateData::setReadOnly(bool readOnly)
{
    m_Date->m_Date->setReadOnly(readOnly);
}

bool BaseDateData::isReadOnly() const
{
    return m_Date->m_Date->isReadOnly();
}

bool BaseDateData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (role==Qt::EditRole) {
        if (data.canConvert<QDate>()) {
            m_Date->m_Date->setDate(data.toDate());
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseDateData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_Date->m_Date->date();
}

void BaseDateData::setStorableData(const QVariant &data)
{
    setDate(data.toString());
    m_OriginalValue = data.toString();
}

QVariant BaseDateData::storableData() const
{
    return m_Date->m_Date->date().toString(Qt::ISODate);
}

void BaseDateData::onValueChanged()
{
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseDateTime ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//TODO: create a function that returns default time and date
BaseDateTime::BaseDateTime(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_DateTime(0)
{
    setObjectName("BaseDateTime");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QDateTimeEdit *le = formItem->parentFormMain()->formWidget()->findChild<QDateTimeEdit*>(widget);
        if (le) {
            m_DateTime = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_DateTime = new QDateTimeEdit(this);
        }
        // Find Label
        m_Label = Constants::findLabel(formItem);
    } else {    // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);

        // Add DateTime selector and manage date format
        m_DateTime = new QDateTimeEdit(this);
        m_DateTime->setObjectName("DateTime_" + m_FormItem->uuid());
        m_DateTime->setCalendarPopup(true);
        hb->addWidget(m_Label);
        hb->addWidget(m_DateTime);
    }
    m_DateTime->setDisplayFormat(Constants::getDateTimeFormat(m_FormItem));
    setFocusedWidget(m_DateTime);

    // Manage options
    const QStringList &options = formItem->getOptions();

    // We use DATE_NOW for Date widget and DateTime widget
    if (options.contains(Constants::DATE_NOW, Qt::CaseInsensitive)) {
        m_DateTime->QDateTimeEdit::setDateTime(QDateTime::currentDateTime());
    } else {
        QDateTime defaultDateTime = QDateTime::fromString(Constants::DEFAULT_DATETIME, Constants::DEFAULT_DATETIME_FORMAT);
        m_DateTime->setDateTime(defaultDateTime);
    }
    if (options.contains(Constants::DATE_PATIENTLIMITS, Qt::CaseInsensitive)) {
        connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
        onCurrentPatientChanged();
    }

    // create FormItemData
    BaseDateTimeData *data = new BaseDateTimeData(m_FormItem);
    data->setBaseDateTime(this);
    m_FormItem->setItemData(data);

    connect(m_DateTime, SIGNAL(dateTimeChanged(QDateTime)), data, SLOT(onValueChanged()));
}

BaseDateTime::~BaseDateTime()
{
        delete m_DateTime;
        delete m_FormItem;
}

void BaseDateTime::onCurrentPatientChanged()
{
    if (!patient()->data(Core::IPatient::DateOfBirth).isNull()) {
        m_DateTime->setMinimumDate(patient()->data(Core::IPatient::DateOfBirth).toDate());
    } else {
        m_DateTime->setMinimumDate(QDate::currentDate().addYears(-200));
    }
    if (!patient()->data(Core::IPatient::DateOfDeath).isNull()) {
        m_DateTime->setMaximumDate(patient()->data(Core::IPatient::DateOfDeath).toDate());
    } else {
        m_DateTime->setMaximumDate(QDate::currentDate().addYears(200));
    }
}

QString BaseDateTime::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
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
        QString defaultDateTime = Constants::DEFAULT_DATETIME;
        QString defaultDateTimeFormat = Constants::DEFAULT_DATETIME_FORMAT;
        QString widgetDateTime = m_DateTime->dateTime().toString(defaultDateTimeFormat);
        bool dateTimeIsDefault(widgetDateTime == defaultDateTime);
        if (Constants::dontPrintEmptyValues(m_FormItem) && dateTimeIsDefault)
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
                       "<td style=\"vertical-align: top;\">"
                       "%3"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label())
                .arg(QLocale().toString(m_DateTime->date(), Constants::getDateFormat(m_FormItem)).replace(" ","&nbsp;"))
                .arg(QLocale().toString(m_DateTime->time(), "hh:mm:ss").replace(" ", "&nbsp;"));
    }
    return content;
}

void BaseDateTime::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (m_DateTime)
        m_DateTime->setToolTip(m_FormItem->spec()->tooltip());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseDateTimeData::BaseDateTimeData(Form::FormItem *item) :
        m_FormItem(item), m_DateTime(0)
{
}

BaseDateTimeData::~BaseDateTimeData()
{
    delete m_DateTime;
    delete m_FormItem;
}

void BaseDateTimeData::setDateTime(const QString &s)
{
    m_DateTime->m_DateTime->clear();
    m_DateTime->m_DateTime->setDateTime(QDateTime::fromString(s, Qt::ISODate));
    onValueChanged();
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseDateTimeData::clear()
{
    QDateTime defaultDateTime;
    defaultDateTime = QDateTime::fromString(Constants::DEFAULT_DATETIME, Constants::DEFAULT_DATETIME_FORMAT);
    m_OriginalDateTimeValue = defaultDateTime.toString(Qt::ISODate);
    setDateTime(m_OriginalDateTimeValue);
}

bool BaseDateTimeData::isModified() const
{

    return (m_OriginalDateTimeValue != m_DateTime->m_DateTime->dateTime().toString(Qt::ISODate));
}

void BaseDateTimeData::setModified(bool modified)
{
    if (!modified) {
        m_OriginalDateTimeValue = m_DateTime->m_DateTime->dateTime().toString(Qt::ISODate);
    }
}

void BaseDateTimeData::setReadOnly(bool readOnly)
{
    m_DateTime->m_DateTime->setReadOnly(readOnly);
}

bool BaseDateTimeData::isReadOnly() const
{
    return m_DateTime->m_DateTime->isReadOnly();
}

bool BaseDateTimeData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (role==Qt::EditRole) {
        if (data.canConvert<QDateTime>()) {
            m_DateTime->m_DateTime->setDateTime(data.toDateTime());
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseDateTimeData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_DateTime->m_DateTime->dateTime();
}

void BaseDateTimeData::setStorableData(const QVariant &data)
{
    setDateTime(data.toString());
    m_OriginalDateTimeValue = data.toString();
}

QVariant BaseDateTimeData::storableData() const
{
    return m_DateTime->m_DateTime->dateTime().toString(Qt::ISODate);
}

void BaseDateTimeData::onValueChanged()
{
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

//------------------------------------------------------------------------------
//------------------------------ BaseSpin --------------------------------------
//------------------------------------------------------------------------------
BaseSpin::BaseSpin(Form::FormItem *formItem, QWidget *parent, bool doubleSpin) :
    Form::IFormWidget(formItem,parent), m_Spin(0)
{
    // create FormItemData
    BaseSpinData *data = new BaseSpinData(m_FormItem);

    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        if (doubleSpin) {
            QDoubleSpinBox *dbl = formItem->parentFormMain()->formWidget()->findChild<QDoubleSpinBox*>(widget);
            if (dbl) {
                m_Spin = dbl;
                connect(dbl, SIGNAL(valueChanged(double)), data, SLOT(onValueChanged()));
            } else {
                LOG_ERROR(QString("Using the QtUiLinkage, "
                                  "QDoubleSpinBox not found in the ui: %1 - %2")
                          .arg(widget)
                          .arg(formItem->uuid())
                          );
                // To avoid segfaulting create a fake spinbox
                m_Spin = new QDoubleSpinBox(this);
            }
        } else {
            QSpinBox *dbl = formItem->parentFormMain()->formWidget()->findChild<QSpinBox*>(widget);
            if (dbl) {
                m_Spin = dbl;
                connect(dbl, SIGNAL(valueChanged(int)), data, SLOT(onValueChanged()));
            } else {
                LOG_ERROR(QString("Using the QtUiLinkage, "
                                  "QSpinBox not found in the ui: %1 - %2")
                          .arg(widget)
                          .arg(formItem->uuid())
                          );
                // To avoid segfaulting create a fake spinbox
                m_Spin = new QSpinBox(this);
            }
        }
        m_Spin->setToolTip(m_FormItem->spec()->tooltip());
        // Find label
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add spin
        if (doubleSpin) {
            QDoubleSpinBox *spin = new QDoubleSpinBox(this);
            spin->setObjectName("DoubleSpin_" + m_FormItem->uuid());
            spin->setMinimum(formItem->extraData().value(Constants::SPIN_EXTRAS_KEY_MIN, "0").toDouble());
            spin->setMaximum(formItem->extraData().value(Constants::SPIN_EXTRAS_KEY_MAX, "10000").toDouble());
            spin->setSingleStep(formItem->extraData().value(Constants::SPIN_EXTRAS_KEY_STEP, "0.1").toDouble());
            connect(spin, SIGNAL(valueChanged(double)), data, SLOT(onValueChanged()));
            m_Spin = spin;
        } else {
            QSpinBox *spin = new QSpinBox(this);
            spin->setObjectName("Spin_" + m_FormItem->uuid());
            spin->setMinimum(formItem->extraData().value(Constants::SPIN_EXTRAS_KEY_MIN, "0").toInt());
            spin->setMaximum(formItem->extraData().value(Constants::SPIN_EXTRAS_KEY_MAX, "10000").toInt());
            spin->setSingleStep(formItem->extraData().value(Constants::SPIN_EXTRAS_KEY_STEP, "1").toInt());
            m_Spin = spin;
            connect(spin, SIGNAL(valueChanged(int)), data, SLOT(onValueChanged()));
        }
        m_Spin->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        hb->addWidget(m_Spin);
    }
    setObjectName("BaseSpin_" + m_FormItem->uuid());
    setFocusedWidget(m_Spin);

    // manage options
    data->setBaseSpin(this);
    m_FormItem->setItemData(data);
}

BaseSpin::~BaseSpin()
{}

/*
 * HTML 4 encoded text to print
 * If the specialValueText "∅" has been set in the ui file, return empty string.
 */
QString BaseSpin::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
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
            if (spin->specialValueText() == QString("∅") && spin->value() == spin->minimum()) {
                return QString();
            }
            value = spin->text();
        } else {
            QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin);
            if (dspin) {
                if (dspin->specialValueText() == QString("∅") && dspin->value() == dspin->minimum()) {
                    return QString();
                }
                value = dspin->text();
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
        m_Spin->setToolTip(m_FormItem->spec()->tooltip());
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
    QSpinBox *spin = qobject_cast<QSpinBox*>(m_Spin->m_Spin);
    if (spin) {
        if (!(m_FormItem->valueReferences()->defaultValue() == QVariant())) {
            spin->setValue(m_FormItem->valueReferences()->defaultValue().toInt());
        } else {
            spin->setValue(spin->minimum());
        }
    } else {
        QDoubleSpinBox *dspin = qobject_cast<QDoubleSpinBox*>(m_Spin->m_Spin);
        if (dspin) {
            if (!(m_FormItem->valueReferences()->defaultValue() == QVariant())) {
                dspin->setValue(m_FormItem->valueReferences()->defaultValue().toDouble());
            } else {
                dspin->setValue(dspin->minimum());
            }
        }
    }
}

bool BaseSpinData::isModified() const
{
    return m_OriginalValue != storableData().toDouble();
}

void BaseSpinData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = storableData().toDouble();
}

void BaseSpinData::setReadOnly(bool readOnly)
{
    m_Spin->m_Spin->setReadOnly(readOnly);
}

bool BaseSpinData::isReadOnly() const
{
    return m_Spin->m_Spin->isReadOnly();
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
    Constants::executeOnValueChangedScript(m_FormItem);
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
        QPushButton *le = formItem->parentFormMain()->formWidget()->findChild<QPushButton*>(widget);
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
    setFocusedWidget(m_Button);
    connect(m_Button, SIGNAL(clicked()) , this , SLOT(buttonClicked()));
}

BaseButton::~BaseButton()
{}

void BaseButton::buttonClicked()
{
    // TODO: manage a readonly state using the Form::IFormItemData::setReadOnly()
    if (!m_FormItem->scripts()->onClicked().isEmpty())
        scriptManager()->evaluate(m_FormItem->scripts()->onClicked());
}

void BaseButton::retranslate()
{
    if (m_Button) {
        m_Button->setText(m_FormItem->spec()->label());
        m_Button->setToolTip(m_FormItem->spec()->tooltip());
    }
}
