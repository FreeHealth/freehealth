#include "baseformwidgetsoptionspage.h"
#include "ui_baseformwidgetsoptionspage.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <translationutils/constanttranslations.h>

using namespace BaseWidgets;
using namespace BaseWidgets::Internal;

namespace {
    const char *const SETTINGS_COMPACTVIEW_SPACING = "BaseFormWidgets/CompactView/Spacing";
    const char *const SETTINGS_COMPACTVIEW_MARGIN = "BaseFormWidgets/CompactView/Margin";
}

BaseFormSettingsWidget::BaseFormSettingsWidget(QWidget *parent): QWidget(parent)
{
    m_ui = new Ui::BaseFormWidgetsOptionsPage();
    m_ui->setupUi(this);
    Core::ISettings *s = Core::ICore::instance()->settings();
    m_ui->marginSpin->setValue( s->value(::SETTINGS_COMPACTVIEW_MARGIN, 0).toInt() );
    m_ui->spacingSpin->setValue( s->value(::SETTINGS_COMPACTVIEW_SPACING, 2).toInt() );
}

BaseFormSettingsWidget::~BaseFormSettingsWidget()
{
    if (m_ui)
        delete m_ui;
    m_ui=0;
}

void BaseFormSettingsWidget::applyChanges()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
    s->setValue(::SETTINGS_COMPACTVIEW_MARGIN, m_ui->marginSpin->value());
    s->value(::SETTINGS_COMPACTVIEW_SPACING, m_ui->spacingSpin->value());
}

void BaseFormSettingsWidget::resetToDefaults()
{
    m_ui->marginSpin->setValue(0);
    m_ui->spacingSpin->setValue(2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
BaseFormWidgetsOptionsPage::BaseFormWidgetsOptionsPage(QObject *parent) :
    Core::IOptionsPage(parent)
{
    setObjectName("BaseFormWidgetsOptionsPage");
}

BaseFormWidgetsOptionsPage::~BaseFormWidgetsOptionsPage()
{
}

QString BaseFormWidgetsOptionsPage::id() const
{
    return objectName();
}

QString BaseFormWidgetsOptionsPage::name() const
{
    return tr("Base Forms Widgets");
}

QString BaseFormWidgetsOptionsPage::category() const
{
    return Trans::ConstantTranslations::tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

void BaseFormWidgetsOptionsPage::resetToDefaults()
{
    if (m_Widget)
        m_Widget->resetToDefaults();
}

void BaseFormWidgetsOptionsPage::applyChanges()
{
    if (m_Widget)
        m_Widget->applyChanges();
}

void BaseFormWidgetsOptionsPage::finish()
{
}

QWidget *BaseFormWidgetsOptionsPage::createPage(QWidget *parent)
{
    m_Widget = new BaseFormSettingsWidget(parent);
    return m_Widget;
}
