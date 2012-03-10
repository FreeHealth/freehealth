/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mfDrugsPreferences.h"

#include <mfDrugsConstants.h>

#include <tkTheme.h>
#include <tkPrinter.h>
#include <tkSettings.h>
#include <tkLog.h>
#include <tkConstantTranslations.h>

#include <QPixmap>

using namespace mfDrugsConstants;
Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

mfDrugsPreferences::mfDrugsPreferences(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    // configure tkRichTextEditors
    previewer = tkPrinter::previewer(this);
    userLayout->addWidget(previewer, 0,0);
    ALDBefore->setTypes(tkRichTextEditor::Full|tkRichTextEditor::Full);
    ALDAfter->setTypes(tkRichTextEditor::Full|tkRichTextEditor::Full);
    // set icons
    drugBoldButton->setIcon( tkTheme::icon(ICONBOLD) );
    drugUnderlineButton->setIcon( tkTheme::icon(ICONUNDERLINE) );
    drugItalicButton->setIcon( tkTheme::icon(ICONITALIC) );
    prescrBoldButton->setIcon( tkTheme::icon(ICONBOLD) );
    prescrUnderlineButton->setIcon( tkTheme::icon(ICONUNDERLINE) );
    prescrItalicButton->setIcon( tkTheme::icon(ICONITALIC) );

    // feed with actual values
    tkSettings *s = tkSettings::instance();
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

    hideLabCheck->setChecked( s->value( MFDRUGS_SETTING_HIDELABORATORY ).toBool() );

    viewFontCombo->setCurrentFont( s->value( MFDRUGS_SETTING_VIEWFONT ).toString() );
    viewFontSizeSpin->setValue( s->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt() );

    previewer->setHeader( s->value( MFDRUGS_SETTING_USERHEADER ).toString() );
    previewer->setFooter( s->value( MFDRUGS_SETTING_USERFOOTER ).toString() );
    previewer->setWatermark( s->value( MFDRUGS_SETTING_WATERMARK_HTML ).toString(),
                             tkPrinter::Presence(s->value( MFDRUGS_SETTING_WATERMARKPRESENCE ).toInt()) );

    ALDBefore->textEdit()->setHtml( s->value( MFDRUGS_SETTING_ALD_PRE_HTML ).toString() );
    ALDAfter->textEdit()->setHtml( s->value( MFDRUGS_SETTING_ALD_POST_HTML ).toString() );
}

void mfDrugsPreferences::saveToSettings( tkSettings *settings )
{
    tkSettings *s;
    if (!settings)
        s = tkSettings::instance();
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

    s->setValue( MFDRUGS_SETTING_HIDELABORATORY, hideLabCheck->isChecked() );

    s->setValue( MFDRUGS_SETTING_USERHEADER, previewer->headerToHtml() );
    s->setValue( MFDRUGS_SETTING_USERFOOTER, previewer->footerToHtml() );

    s->setValue( MFDRUGS_SETTING_VIEWFONT , viewFontCombo->currentFont() );
    s->setValue( MFDRUGS_SETTING_VIEWFONTSIZE, viewFontSizeSpin->value() );

    s->setValue( MFDRUGS_SETTING_WATERMARKPRESENCE, previewer->watermarkPresence() );
    s->setValue( MFDRUGS_SETTING_WATERMARK_HTML, previewer->watermarkToHtml() );
    s->setValue( MFDRUGS_SETTING_ALD_PRE_HTML, ALDBefore->textEdit()->toHtml() );
    s->setValue( MFDRUGS_SETTING_ALD_POST_HTML, ALDAfter->textEdit()->toHtml() );
}

void mfDrugsPreferences::writeDefaultSettings( tkSettings *s )
{
//    qWarning() << "---------> writedefaults";
    tkLog::addMessage("mfDrugsPreferences", tkTr(CREATING_DEFAULT_SETTINGS_FOR_1).arg("mfDrugsWidget") );
    s->setValue( MFDRUGS_SETTING_CONFIGURED, true );
    s->setValue( MFDRUGS_SETTING_VIEWFONT , QFont() );
    s->setValue( MFDRUGS_SETTING_VIEWFONTSIZE, QFont().pointSize() );
    s->setValue( MFDRUGS_SETTING_HISTORYSIZE, 20 );
    s->setValue( MFDRUGS_SETTING_DRUGHISTORY, QVariant() );
    s->setValue( MFDRUGS_SETTING_LEVELOFWARNING , 0 );

    s->setValue( MFDRUGS_SETTING_DRUGFONT , QFont().toString() );
    s->setValue( MFDRUGS_SETTING_PRESCRIPTIONFONT , QFont().toString() );

    s->setValue( MFDRUGS_SETTING_HIDELABORATORY, false );

    s->setValue( MFDRUGS_SETTING_WATERMARKPRESENCE, tkPrinter::DuplicataOnly );
    s->setValue( MFDRUGS_SETTING_WATERMARKALIGNEMENT, Qt::AlignCenter );
    s->setValue( MFDRUGS_SETTING_WATERMARK_HTML, MFDRUGS_DEFAULT_WATEMARKHTML );

    s->setValue( MFDRUGS_SETTING_USERHEADER, MFDRUGS_DEFAULT_USERHEADER );
    s->setValue( MFDRUGS_SETTING_USERFOOTER, "" );

    s->setValue( MFDRUGS_SETTING_ALD_PRE_HTML, MFDRUGS_DEFAULT_ALD_PRE_HTML );
    s->setValue( MFDRUGS_SETTING_ALD_POST_HTML, MFDRUGS_DEFAULT_ALD_POST_HTML );
    s->sync();
}

void mfDrugsPreferences::changeEvent(QEvent *e)
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
