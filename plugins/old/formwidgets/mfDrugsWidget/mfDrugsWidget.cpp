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
#include "mfDrugsWidget.h"

// include drugswidget headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/mfDrugInfo.h>
#include <drugspreferences/mfDrugsPreferences.h>
#include <drugswidget/mfDrugsCentralWidget.h>

// include FreeMedForms headers
#include <mfCore.h>
#include <mfSettings.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkTranslators.h>
#include <tkActionManager.h>
#include <tkConstantTranslations.h>
#include <tkTheme.h>

// include Qt headers
#include <QStringList>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QTextBrowser>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextEdit>
#include <QGridLayout>
#include <QModelIndex>


namespace mfDrugsWidgetPluginsPrivateConstants {
    const char* const OPTION_WITHPRESCRIBING  = "withprescribing";
    const char* const OPTION_WITHPRINTING     = "withprinting";
}

using namespace mfDrugsWidgetPluginsPrivateConstants;

Q_TK_USING_CONSTANTS;
Q_TK_USING_TRANSLATIONS;

//using namespace mfDrugsConstants;
//using namespace mfDosagesConstants;
//using namespace mfInteractionsConstants;

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfDrugsWidget plugin interface ------------------------------------
//--------------------------------------------------------------------------------------------------------
mfDrugsWidgetPlugin::mfDrugsWidgetPlugin()
{
    // define plugins informations
    mPluginInfos.Caption = tr( "Form's Drugs Manager Widgets" );
    mPluginInfos.Description = tr( "Plugin for form's widget : "
                                   "drug database widget, lets you manage prescribing." );
    mPluginInfos.Author = "Eric Maeker,MD <eric.maeker@gmail.com>";
    mPluginInfos.Type = mfBaseInterface::iFormWidget;
    mPluginInfos.Name = PLUGIN_NAME;
    mPluginInfos.Version = "0.0.2";
    mPluginInfos.Enabled = false;
}


QStringList mfDrugsWidgetPlugin::widgets() const
{
    return QStringList() << "drugs";
}

bool mfDrugsWidgetPlugin::isContainer( const int ) const
{
    return false;
}

mfAbstractWidget * mfDrugsWidgetPlugin::getWidget( mfObject * mfo, mfAbstractWidget * parent ) const
{
    return new mfDrugsWidget( mfo, parent );
}


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfDrugsWidget implementation ------------------------------------
//--------------------------------------------------------------------------------------------------------
mfDrugsWidget::mfDrugsWidget( mfObject * mfo, mfAbstractWidget * parent )
          : mfAbstractWidget( mfo, parent ),
          m_PrescriptionModel(0)
{
    // Add Translator to the Application
    mfCore::translators()->addNewTranslator( "mfDrugsWidget" );

    // Prepare Widget Layout and label
    mfo->setParam( mfObject::Param_Options, "labelontop" );
    QBoxLayout * hb = getBoxLayout( mfo, this );
    // Add QLabel
    hb->addWidget( m_Label );
    if ( !( mfo->options() & mfObjectFundamental::LabelOnTop ) )
    {
        Qt::Alignment alignment = m_Label->alignment();
        alignment &= ~( Qt::AlignVertical_Mask );
        alignment |= Qt::AlignVCenter;
        m_Label->setAlignment( alignment );
    }

    // Get options
    const QStringList &options = mfo->param( mfObject::Param_Options ).toStringList();
    if ( options.contains( OPTION_WITHPRESCRIBING, Qt::CaseInsensitive ) )
        m_WithPrescribing = true;
    if ( options.contains( OPTION_WITHPRINTING, Qt::CaseInsensitive ) )
        m_WithPrinting = true;

    // intialize drugs database
    mfDrugsBase::instance();

    // check settings
    if (!mfCore::settings()->value(MFDRUGS_SETTING_CONFIGURED, false).toBool())
        mfDrugsPreferences::writeDefaultSettings(mfCore::settings());

    // create main widget
    m_PrescriptionModel = new mfDrugsModel(this);
    mfDrugsCentralWidget *centralWidget = new mfDrugsCentralWidget(this);
    centralWidget->initialize();
    hb->addWidget(centralWidget);

    createConnections();

    centralWidget->changeFontTo( QFont(mfCore::settings()->value( MFDRUGS_SETTING_VIEWFONT ).toString(),
                                 mfCore::settings()->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );

    // Connect list selection changed with mfObject value changed
    //      connect( m_Combo, SIGNAL( activated ( int ) ),
    //               this ,   SLOT  ( updateObject( int ) ) );
    //      connect( mfo,     SIGNAL( valueChanged() ),
    //               this,    SLOT( updateWidget() ) );

    // if selected data exists fill the widget with
    //      if ( mfo->value() != QVariant() )
    //           updateWidget();
}

mfDrugsWidget::~mfDrugsWidget()
{
}

void mfDrugsWidget::createConnections()
{
    //    tkActionManager *am = tkActionManager::instance();
    //    connect( am->action(A_FILE_OPEN), SIGNAL( triggered() ), this, SLOT( openPrescription() ) );
    //    connect( am->action(A_FILE_SAVE), SIGNAL( triggered() ), this, SLOT( savePrescription() ) );
    //    connect( am->action(A_FILE_PRINT), SIGNAL( triggered() ), this, SLOT( printPrescription() ) );
    //    connect( am->action(A_FILE_EXIT), SIGNAL( triggered() ), this, SLOT( close() ) );
    //
    //    connect( am->action(A_PREFERENCES), SIGNAL( triggered() ), this, SLOT( preferences() ) );
    //    connect( am->action(A_ABOUT), SIGNAL( triggered() ), this, SLOT( about() ) );
    //    connect( am->action(A_DEBUGHELPER), SIGNAL( triggered() ), this, SLOT( debugDialog() ) );
    //    connect( am->action(A_CONFIG_MEDINTUX), SIGNAL( triggered() ), this, SLOT( configureMedinTux() ) );
    //
    //    connect( m_PrescriptionView, SIGNAL(printTriggered()), this, SLOT(printPrescription()));
}

void mfDrugsWidget::createDefaultSettings( tkSettings *settings )
{
    mfDrugsPreferences::writeDefaultSettings(settings);
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfDrugsWidget private slots -------------------------------------
//----------------------------------------- for plugin interface -----------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfDrugsWidget::updateWidget()
{
    //      if ( !m_Combo ) return;
    //      m_Combo->disconnect();
    //
    //      connect( m_Combo,  SIGNAL( activated ( int ) ),
    //               this ,    SLOT  ( updateObject( int ) ) );
}

void mfDrugsWidget::updateObject( int )
{
    //      m_Object->selectedValueChangedTo( id );
}

void mfDrugsWidget::retranslateUi( const QString & )
{
    //     if ( m_DrugsModel )
    //     {
    //          m_DrugsModel->setHeaderData( 1, Qt::Horizontal, tr( "Short Name" ) );
    //          m_DrugsModel->setHeaderData( 2, Qt::Horizontal, tr( "Form" ) );
    //          m_DrugsModel->setHeaderData( 3, Qt::Horizontal, tr( "Administration" ) );
    //     }
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Export class as plugin ----------------------------------------
//--------------------------------------------------------------------------------------------------------
Q_EXPORT_PLUGIN2( mf_drugswidgetplugin, mfDrugsWidgetPlugin )
