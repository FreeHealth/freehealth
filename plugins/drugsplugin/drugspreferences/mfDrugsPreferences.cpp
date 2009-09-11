/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfDrugsPreferences.h"

#include <mfDrugsConstants.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>

#include <QPixmap>

using namespace mfDrugsConstants;
using namespace Drugs;
using namespace Drugs::Internal;
using namespace Trans::ConstantTranslations;


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  DrugsViewOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsViewOptionsPage::DrugsViewOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsViewOptionsPage"); }

DrugsViewOptionsPage::~DrugsViewOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsViewOptionsPage::id() const { return objectName(); }
QString DrugsViewOptionsPage::name() const { return tr("Drugs View"); }
QString DrugsViewOptionsPage::category() const { return tkTr(Trans::Constants::PLUGINS_CATEGORY); }

void DrugsViewOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(Core::ICore::instance()->settings());
}

void DrugsViewOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    /** \todo here */
}

void DrugsViewOptionsPage::finish() { delete m_Widget; }

QWidget *DrugsViewOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsViewWidget(parent);
    return m_Widget;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  DrugsUserOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsUserOptionsPage::DrugsUserOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsUserOptionsPage"); }

DrugsUserOptionsPage::~DrugsUserOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsUserOptionsPage::id() const { return objectName(); }
QString DrugsUserOptionsPage::name() const { return tr("Drugs User"); }
QString DrugsUserOptionsPage::category() const { return tkTr(Trans::Constants::PLUGINS_CATEGORY); }

void DrugsUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(Core::ICore::instance()->settings());
}

void DrugsUserOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    /** \todo here */
}

void DrugsUserOptionsPage::finish() { delete m_Widget; }

QWidget *DrugsUserOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsUserWidget(parent);
    return m_Widget;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  DrugsExtraOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsExtraOptionsPage::DrugsExtraOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsExtraOptionsPage"); }

DrugsExtraOptionsPage::~DrugsExtraOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsExtraOptionsPage::id() const { return objectName(); }
QString DrugsExtraOptionsPage::name() const { return tr("Drug Extras"); }
QString DrugsExtraOptionsPage::category() const { return tkTr(Trans::Constants::PLUGINS_CATEGORY); }

void DrugsExtraOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(Core::ICore::instance()->settings());
}

void DrugsExtraOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    /** \todo here */
}

void DrugsExtraOptionsPage::finish() { delete m_Widget; }

QWidget *DrugsExtraOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugsExtraWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsViewWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsViewWidget::DrugsViewWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    Core::ITheme *th = Core::ICore::instance()->theme();
    // set icons
    drugBoldButton->setIcon( th->icon(Core::Constants::ICONBOLD) );
    drugUnderlineButton->setIcon( th->icon(Core::Constants::ICONUNDERLINE) );
    drugItalicButton->setIcon( th->icon(Core::Constants::ICONITALIC) );
    prescrBoldButton->setIcon( th->icon(Core::Constants::ICONBOLD) );
    prescrUnderlineButton->setIcon( th->icon(Core::Constants::ICONUNDERLINE) );
    prescrItalicButton->setIcon( th->icon(Core::Constants::ICONITALIC) );

    // feed with actual values
    Core::ISettings *s = Core::ICore::instance()->settings();
    //    fontSizeSpin->setValue( m_fontSize );
    QString userName = s->value(MFDRUGS_SETTING_USER).toString();
    if ( ( !userName.isEmpty() ) && ( userName.startsWith( "test_" ) ) ) {
        testerBox->setChecked( true );
        userNameEdit->setText( userName );
        userPasswordEdit->setText( s->value(MFDRUGS_SETTING_PASSWORD).toString() );
        userPasswordEdit->setEnabled( false );
    }
    else
        testerBox->setChecked( false );

    viewFontSizeSpin->setValue( s->value( MFDRUGS_SETTING_VIEWFONTSIZE, 12).toInt() );
    historicSizeSpin->setValue( s->value( MFDRUGS_SETTING_HISTORYSIZE ).toInt() );
    levelOfWarningCombo->setCurrentIndex( s->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt() );

    viewFontCombo->setCurrentFont( s->value( MFDRUGS_SETTING_VIEWFONT ).toString() );
    viewFontSizeSpin->setValue( s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt() );

    QFont drugsFont;
    drugsFont.fromString(s->value( MFDRUGS_SETTING_DRUGFONT ).toString());
    QFont prescrFont;
    prescrFont.fromString(s->value( MFDRUGS_SETTING_PRESCRIPTIONFONT ).toString());

    drugFontCombo->setCurrentFont( drugsFont );
    prescriptionFontCombo->setCurrentFont( prescrFont );

    prescriptionFontSizeSpin->setValue( prescrFont.pointSize() );
    prescrBoldButton->setChecked( prescrFont.bold() );
    prescrItalicButton->setChecked( prescrFont.italic() );
    prescrUnderlineButton->setChecked( prescrFont.underline() );

    drugFontSizeSpin->setValue( drugsFont.pointSize() );
    drugBoldButton->setChecked( drugsFont.bold() );
    drugUnderlineButton->setChecked( drugsFont.italic() );
    drugItalicButton->setChecked( drugsFont.underline() );
}

void DrugsViewWidget::saveToSettings( Core::ISettings *settings )
{
    Core::ISettings *s;
    if (!settings)
        s = Core::ICore::instance()->settings();
    else
        s = settings;

    // manage font size
    s->setValue( MFDRUGS_SETTING_VIEWFONTSIZE, viewFontSizeSpin->value() );
    // set testers datas
    if ( ( userNameEdit->text().startsWith( "test_" ) ) )
        s->setValue( MFDRUGS_SETTING_USER, userNameEdit->text() );

    if ( !userPasswordEdit->text().isEmpty() )
        s->setValue( MFDRUGS_SETTING_PASSWORD, userPasswordEdit->text() );

    // manage history size
    s->setValue( MFDRUGS_SETTING_HISTORYSIZE, historicSizeSpin->value() );
    s->setValue( MFDRUGS_SETTING_DRUGHISTORY, QVariant() );
    s->setValue( MFDRUGS_SETTING_LEVELOFWARNING , levelOfWarningCombo->currentIndex() );

    QFont drugsFont = drugFontCombo->currentFont();
    drugsFont.setBold(drugBoldButton->isChecked());
    drugsFont.setItalic(drugItalicButton->isChecked());
    drugsFont.setUnderline(drugUnderlineButton->isChecked());
    drugsFont.setPointSize(drugFontSizeSpin->value());

    QFont prescrFont = prescriptionFontCombo->currentFont();
    prescrFont.setBold(prescrBoldButton->isChecked());
    prescrFont.setItalic(prescrItalicButton->isChecked());
    prescrFont.setUnderline(prescrUnderlineButton->isChecked());
    prescrFont.setPointSize(prescriptionFontSizeSpin->value());

    s->setValue( MFDRUGS_SETTING_DRUGFONT , drugsFont.toString() );
    s->setValue( MFDRUGS_SETTING_PRESCRIPTIONFONT , prescrFont.toString() );

    s->setValue( MFDRUGS_SETTING_VIEWFONT , viewFontCombo->currentFont() );
    s->setValue( MFDRUGS_SETTING_VIEWFONTSIZE, viewFontSizeSpin->value() );
}

void DrugsViewWidget::writeDefaultSettings( Core::ISettings *s )
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsViewWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget") );
    s->setValue( MFDRUGS_SETTING_CONFIGURED, true );
    s->setValue( MFDRUGS_SETTING_VIEWFONT , QFont() );
    s->setValue( MFDRUGS_SETTING_VIEWFONTSIZE, QFont().pointSize() );
    s->setValue( MFDRUGS_SETTING_HISTORYSIZE, 20 );
    s->setValue( MFDRUGS_SETTING_DRUGHISTORY, QVariant() );
    s->setValue( MFDRUGS_SETTING_LEVELOFWARNING , 0 );

    s->setValue( MFDRUGS_SETTING_DRUGFONT , QFont().toString() );
    s->setValue( MFDRUGS_SETTING_PRESCRIPTIONFONT , QFont().toString() );
    s->sync();
}

void DrugsViewWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsUserWidget::DrugsUserWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    Core::ITheme *th = Core::ICore::instance()->theme();
    previewer = Print::Printer::previewer(this);
    userLayout->addWidget(previewer, 0,0);
    // feed with actual values
    Core::ISettings *s = Core::ICore::instance()->settings();

    previewer->setHeader( s->value( MFDRUGS_SETTING_USERHEADER ).toString() );
    previewer->setFooter( s->value( MFDRUGS_SETTING_USERFOOTER ).toString() );
    previewer->setWatermark( s->value( MFDRUGS_SETTING_WATERMARK_HTML ).toString(),
                             Print::Printer::Presence(s->value( MFDRUGS_SETTING_WATERMARKPRESENCE ).toInt()) );
}

void DrugsUserWidget::saveToSettings( Core::ISettings *settings )
{
    Core::ISettings *s;
    if (!settings)
        s = Core::ICore::instance()->settings();
    else
        s = settings;

    s->setValue( MFDRUGS_SETTING_USERHEADER, previewer->headerToHtml() );
    s->setValue( MFDRUGS_SETTING_USERFOOTER, previewer->footerToHtml() );
    s->setValue( MFDRUGS_SETTING_WATERMARKPRESENCE, previewer->watermarkPresence() );
    s->setValue( MFDRUGS_SETTING_WATERMARK_HTML, previewer->watermarkToHtml() );
}

void DrugsUserWidget::writeDefaultSettings( Core::ISettings *s )
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget") );
    s->setValue( MFDRUGS_SETTING_CONFIGURED, true );
    s->setValue( MFDRUGS_SETTING_WATERMARKPRESENCE, Print::Printer::DuplicataOnly );
    s->setValue( MFDRUGS_SETTING_WATERMARKALIGNEMENT, Qt::AlignCenter );
    s->setValue( MFDRUGS_SETTING_WATERMARK_HTML, MFDRUGS_DEFAULT_WATEMARKHTML );
    s->setValue( MFDRUGS_SETTING_USERHEADER, MFDRUGS_DEFAULT_USERHEADER );
    s->setValue( MFDRUGS_SETTING_USERFOOTER, "" );
    s->sync();
}

void DrugsUserWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserWidget  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsExtraWidget::DrugsExtraWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    Core::ITheme *th = Core::ICore::instance()->theme();
    ALDBefore->setTypes(Editor::TextEditor::Full);
    ALDAfter->setTypes(Editor::TextEditor::Full|Editor::TextEditor::Full);
    // feed with actual values
    Core::ISettings *s = Core::ICore::instance()->settings();

    hideLabCheck->setChecked( s->value( MFDRUGS_SETTING_HIDELABORATORY ).toBool() );

    ALDBefore->textEdit()->setHtml( s->value( MFDRUGS_SETTING_ALD_PRE_HTML ).toString() );
    ALDAfter->textEdit()->setHtml( s->value( MFDRUGS_SETTING_ALD_POST_HTML ).toString() );
}

void DrugsExtraWidget::saveToSettings( Core::ISettings *settings )
{
    Core::ISettings *s;
    if (!settings)
        s = Core::ICore::instance()->settings();
    else
        s = settings;
    s->setValue( MFDRUGS_SETTING_HIDELABORATORY, hideLabCheck->isChecked() );

    s->setValue( MFDRUGS_SETTING_ALD_PRE_HTML, ALDBefore->textEdit()->toHtml() );
    s->setValue( MFDRUGS_SETTING_ALD_POST_HTML, ALDAfter->textEdit()->toHtml() );
}

void DrugsExtraWidget::writeDefaultSettings( Core::ISettings *s )
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("DrugsExtraWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugsWidget") );
    s->setValue( MFDRUGS_SETTING_CONFIGURED, true );
    s->setValue( MFDRUGS_SETTING_HIDELABORATORY, false );

    s->setValue( MFDRUGS_SETTING_ALD_PRE_HTML, MFDRUGS_DEFAULT_ALD_PRE_HTML );
    s->setValue( MFDRUGS_SETTING_ALD_POST_HTML, MFDRUGS_DEFAULT_ALD_POST_HTML );
    s->sync();
}

void DrugsExtraWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
