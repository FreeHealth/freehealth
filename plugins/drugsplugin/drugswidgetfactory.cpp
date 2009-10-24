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
#include "drugswidgetfactory.h"

// include drugswidget headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/druginfo.h>
#include <drugspreferences/mfDrugsPreferences.h>
#include <drugswidget/mfDrugsCentralWidget.h>

// include FreeMedForms headers
//#include <mfCore.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/iformitem.h>

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
using namespace Drugs;
using namespace Drugs::Internal;

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfDrugsWidget plugin interface ------------------------------------
//--------------------------------------------------------------------------------------------------------
DrugsWidgetsFactory::DrugsWidgetsFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

DrugsWidgetsFactory::~DrugsWidgetsFactory()
{
}

bool DrugsWidgetsFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool DrugsWidgetsFactory::extensionInitialized()
{
    return true;
}


QStringList DrugsWidgetsFactory::providedWidgets() const
{
     return QStringList() << "drugs";  /** \todo add more type of plugs ? */
}

bool DrugsWidgetsFactory::isContainer( const int idInStringList ) const
{
    return false;
}

Core::IFormWidget *DrugsWidgetsFactory::createWidget(const QString &name, Core::FormItem *linkedObject, QWidget *parent)
{
    return new DrugsWidget(linkedObject,parent);
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfDrugsWidget implementation ------------------------------------
//--------------------------------------------------------------------------------------------------------
DrugsWidget::DrugsWidget(Core::FormItem *linkedObject, QWidget *parent)
          : IFormWidget(linkedObject,parent),
          m_PrescriptionModel(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnTop, m_LinkedObject->spec()->label(), this );
    // Add QLabel
    hb->addWidget( m_Label );
//    if ( !( mfo->options() & mfObjectFundamental::LabelOnTop ) )
//    {
//        Qt::Alignment alignment = m_Label->alignment();
//        alignment &= ~( Qt::AlignVertical_Mask );
//        alignment |= Qt::AlignVCenter;
//        m_Label->setAlignment( alignment );
//    }

//    // Get options
//    const QStringList &options = mfo->param( mfObject::Param_Options ).toStringList();
//    if ( options.contains( OPTION_WITHPRESCRIBING, Qt::CaseInsensitive ) )
//        m_WithPrescribing = true;
//    if ( options.contains( OPTION_WITHPRINTING, Qt::CaseInsensitive ) )
//        m_WithPrinting = true;

    // intialize drugs database
    DrugsBase::instance();

    // create main widget
    DrugsCentralWidget *centralWidget = new DrugsCentralWidget(this);
    centralWidget->initialize();
    hb->addWidget(centralWidget);

    createConnections();

    centralWidget->changeFontTo( QFont(Core::ICore::instance()->settings()->value( MFDRUGS_SETTING_VIEWFONT ).toString(),
                                 Core::ICore::instance()->settings()->value( MFDRUGS_SETTING_VIEWFONTSIZE ).toInt()) );

    // Connect list selection changed with mfObject value changed
    //      connect( m_Combo, SIGNAL( activated ( int ) ),
    //               this ,   SLOT  ( updateObject( int ) ) );
    //      connect( mfo,     SIGNAL( valueChanged() ),
    //               this,    SLOT( updateWidget() ) );

    // if selected data exists fill the widget with
    //      if ( mfo->value() != QVariant() )
    //           updateWidget();
}

DrugsWidget::~DrugsWidget()
{
}

void DrugsWidget::createConnections()
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

//void DrugsWidget::createDefaultSettings( Core::ISettings *settings )
//{
//    DrugsPreferences::writeDefaultSettings(settings);
//}

