#include "baseformwidgetsoptionspage.h"
#include "ui_baseformwidgetsoptionspage.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <translationutils/constanttranslations.h>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

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
    m_ui->marginSpin->setValue( settings()->value(::SETTINGS_COMPACTVIEW_MARGIN, 0).toInt() );
    m_ui->spacingSpin->setValue( settings()->value(::SETTINGS_COMPACTVIEW_SPACING, 2).toInt() );
}

BaseFormSettingsWidget::~BaseFormSettingsWidget()
{
    if (m_ui)
        delete m_ui;
    m_ui=0;
}

void BaseFormSettingsWidget::applyChanges()
{
    settings()->setValue(::SETTINGS_COMPACTVIEW_MARGIN, m_ui->marginSpin->value());
    settings()->value(::SETTINGS_COMPACTVIEW_SPACING, m_ui->spacingSpin->value());
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

void BaseFormWidgetsOptionsPage::checkSettingsValidity()
{
    if (settings()->value(::SETTINGS_COMPACTVIEW_MARGIN,QVariant())==QVariant()) {
        settings()->setValue(::SETTINGS_COMPACTVIEW_MARGIN,0);
    }

    if (settings()->value(::SETTINGS_COMPACTVIEW_SPACING,QVariant())==QVariant()) {
        settings()->setValue(::SETTINGS_COMPACTVIEW_SPACING,2);
    }
}

void BaseFormWidgetsOptionsPage::finish()
{
    if (m_Widget)
        delete m_Widget;
}

QWidget *BaseFormWidgetsOptionsPage::createPage(QWidget *parent)
{
    m_Widget = new BaseFormSettingsWidget(parent);
    return m_Widget;
}
