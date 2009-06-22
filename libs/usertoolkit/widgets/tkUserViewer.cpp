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
/**
  \class tkUserViewer
  \brief This class is a data wrapper for users.
  When you instanciate this widget, it retreive and show the tkUserModel::currentUserIndex().\n
  Rights are managed via the tkUserModel.\n
  Changes are automaticaly saved into the tkUserModel.
  \todo limit memory usage.
  \ingroup usertoolkit widget_usertoolkit usermanager
*/

#include "tkUserViewer.h"
#include "tkUserViewer_p.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkTranslators.h>
#include <tkLog.h>
#include <tkSerializer.h>
#include <tkStringListView.h>

// include usertoolkit
#include <tkUserModel.h>
#include <tkUserPasswordDialog.h>

// include Qt headers
#include <QByteArray>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QStringListModel>
#include <QMessageBox>
#include <QTextEdit>

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Constructors / Destructors --------------------------------------
//--------------------------------------------------------------------------------------------------------
tkUserViewer::tkUserViewer( QWidget * parent )
    : QWidget( parent )
{
    setObjectName("tkUserViewer");
    d = new tkUserViewerPrivate(this);
    d->initialize();
    if (!parent)
        tkGlobal::centerWidget(this);
}

tkUserViewerPrivate::tkUserViewerPrivate( QObject * parent )
    : QObject( parent ), m_Parent( 0 ), m_Mapper( 0 ),
      genericPreview(0), adminPreview(0), prescriptionPreview(0)
{
    Q_ASSERT_X( static_cast<QDialog*>( parent ), "tkUserViewerPrivate", "*parent is not a QDialog" );
    setObjectName( "tkUserViewerPrivate" );
    m_Parent = static_cast<QWidget*>( parent );
    m_Row = tkUserModel::instance()->currentUserIndex().row();
}

/** \brief Change current viewing user to \e modelRow from tkUserModel */
void tkUserViewer::changeUserTo( const int modelRow )
{
    d->changeUserIndex( modelRow );
}

/** \brief Change current viewing user to \e modelRow from tkUserModel */
void tkUserViewerPrivate::changeUserIndex( const int modelRow )
{
    int oldRow = m_Row;
    m_Row = modelRow;
    checkUserRights();
    if ( m_CanRead )
        m_Mapper->setCurrentModelIndex( tkUserModel::instance()->index( modelRow, 0 ) );
    else {
        m_Row = oldRow;
        tkGlobal::informativeMessageBox( tr("You can not access to these datas."), tr( "You don't have these rights."), "");
    }
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- PRIVATE PART -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Ui initializer. */
void tkUserViewerPrivate::initialize()
{
    setupUi( m_Parent );
    // add previewer
    genericPreview = tkPrinter::previewer(m_Parent);
    adminPreview = tkPrinter::previewer(m_Parent);
    prescriptionPreview = tkPrinter::previewer(m_Parent);
    editorsGenericLayout->addWidget(genericPreview, 0,0);
    editorsAdminLayout->addWidget(adminPreview, 0,0);
    editorsPrescriptionLayout->addWidget(prescriptionPreview, 0,0);

    // populate combos
    titleCombo->addItems( tkUserModel::instance()->titles() );
    genderCombo->addItems( tkUserModel::instance()->genders() );
    languageCombo->addItems( tkTranslators::availableLocales() );
    // QListView need to be managed by hand
    QStringListModel * modelspe = new QStringListModel( this );
    specialtyListView->setModel( modelspe );
    specialtyListView->setButtons( tkListView::AllButtons );
    specialtyListView->setActions( tkListView::AllActions );
    QStringListModel * modelqual = new QStringListModel( this );
    qualificationsListView->setModel( modelqual );
    QStringListModel * modelids = new QStringListModel( this );
    practIdsListView->setModel( modelids );

    checkUserRights();
    prepareMapper();

    // make connections
    connect( languageCombo, SIGNAL( activated( int ) ), this, SLOT( on_languageCombo_activated( int ) ) );
    connect( but_changePassword, SIGNAL(clicked() ), this, SLOT( on_but_changePassword_clicked() ) );
    connect( but_viewHistory, SIGNAL(clicked()), this, SLOT( on_but_viewHistory_clicked()) );

    tabWidget->setCurrentWidget( tabIdentity );
    tabHeadersFooters->setCurrentWidget( genericTab );
}

/** \brief Mapper preparer */
void tkUserViewerPrivate::prepareMapper()
{
    m_Mapper = new QDataWidgetMapper( m_Parent );
    m_Mapper->setModel( tkUserModel::instance() );
    m_Mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );
    m_Mapper->addMapping( uuidLineEdit, User::UserUuid );
    m_Mapper->addMapping( titleCombo, User::UserTitleIndex, "currentIndex" );
    m_Mapper->addMapping( genderCombo, User::UserGenderIndex, "currentIndex" );
    m_Mapper->addMapping( nameLineEdit, User::UserName );
    m_Mapper->addMapping( loginLineEdit, User::UserDecryptedLogin );
    m_Mapper->addMapping( secNameLineEdit, User::UserSecondName );
    m_Mapper->addMapping( surnameLineEdit, User::UserSurname );
    m_Mapper->addMapping( lastLoginDateTimeEdit, User::UserLastLogin );
    m_Mapper->addMapping( languageCombo, User::UserLanguageIndex, "currentIndex" );
    m_Mapper->addMapping( specialtyListView, User::UserSpecialities );
    m_Mapper->addMapping( adressTextEdit, User::UserAdress, "plainText" );
    m_Mapper->addMapping( countryLineEdit, User::UserCountry );
    m_Mapper->addMapping( zipcodeLineEdit, User::UserZipcode );
    m_Mapper->addMapping( cityLineEdit, User::UserCity );
    m_Mapper->addMapping( tel1LineEdit, User::UserTel1 );
    m_Mapper->addMapping( tel2LineEdit, User::UserTel2 );
    m_Mapper->addMapping( tel3LineEdit, User::UserTel3 );
    m_Mapper->addMapping( faxLineEdit, User::UserFax );
    m_Mapper->addMapping( mailLineEdit, User::UserMail );
    m_Mapper->addMapping( specialtyListView, User::UserSpecialities, "stringList" );
    m_Mapper->addMapping( qualificationsListView, User::UserQualifications, "stringList" );
    m_Mapper->addMapping( practIdsListView, User::UserPractitionerId, "stringList" );

    // Got lots of problems with datawidgetmapper and tkrichtexteditor
    // So save by hand
    m_Mapper->addMapping( genericPreview->headerEditor() , User::UserGenericHeader, "html" );
    m_Mapper->addMapping( genericPreview->footerEditor() , User::UserGenericFooter, "html" );

    m_Mapper->addMapping( adminPreview->headerEditor(), User::UserAdministrativeHeader, "html" );
    m_Mapper->addMapping( adminPreview->footerEditor(), User::UserAdministrativeFooter, "html" );

    m_Mapper->addMapping( prescriptionPreview->headerEditor(), User::UserPrescriptionHeader, "html" );
    m_Mapper->addMapping( prescriptionPreview->footerEditor(), User::UserPrescriptionFooter, "html" );

    m_Mapper->addMapping( userManagerRightsListWidget, User::UserManagerRights, "rights" );
    m_Mapper->addMapping( drugsRightsListWidget, User::UserDrugsRights, "rights" );
    m_Mapper->addMapping( medicalRightsListWidget, User::UserMedicalRights, "rights" );
    m_Mapper->addMapping( paramedicalRightsWidget, User::UserParamedicalRights, "rights" );
    m_Mapper->addMapping( administrativeRightsWidget, User::UserAdministrativeRights, "rights" );

    m_Mapper->setCurrentModelIndex( tkUserModel::instance()->currentUserIndex() );
    // make connections
    connect ( m_Mapper->model(), SIGNAL( modelReset() ), this, SLOT( on_modelReseted() ) );
}

/** \brief Change current user view. No save are done into the database from the model. */
void tkUserViewerPrivate::on_modelReseted()
{
    changeUserIndex( tkUserModel::instance()->currentUserIndex().row() );
}

/** \brief Verify rights of user */
void tkUserViewerPrivate::checkUserRights()
{
    tkUserModel *m = tkUserModel::instance();
    int currentUserRow = m->currentUserIndex().row();
    if ( currentUserRow == m_Row ) {
        // showing currentuser
        User::UserRights r = User::UserRights( m->index( currentUserRow, User::UserManagerRights).data().toInt() );
        m_CanModify = ( r ^ User::WriteOwn );
        m_CanRead = ( r ^ User::ReadOwn );
    } else {
        // not showing currentuser
        User::UserRights r = User::UserRights( m->index( currentUserRow, User::UserManagerRights).data().toInt() );
        m_CanModify = ( r & User::WriteAll );
        m_CanRead = ( r & User::ReadAll );
    }
    // enable editors
    foreach( QLineEdit *l, findChildren<QLineEdit *>() )
        l->setEnabled( m_CanModify );
    foreach( QComboBox *c, findChildren<QComboBox *>() )
        c->setEnabled( m_CanModify );
    foreach( QPushButton *c, findChildren<QPushButton *>() )
        c->setEnabled( m_CanModify );
}

/** \brief Lange change requested by user from the ui */
void tkUserViewerPrivate::on_languageCombo_activated( int )
{
    tkTranslators::instance()->changeLanguage( languageCombo->currentText() );
    retranslateUi( m_Parent );
}

void tkUserViewerPrivate::on_but_changePassword_clicked()
{
    tkUserModel * m = tkUserModel::instance();
    tkUserPasswordDialog d( m->index( m_Row, User::UserPassword).data().toString(), m_Parent );
    if ( d.exec() == QDialog::Accepted ) {
        if ( ! d.canGetNewPassword() )
            return;
        QModelIndex idx = m->index( m_Row, User::UserPassword );
        m->setData( idx, d.cryptedPassword() );
    }
}

void tkUserViewerPrivate::on_but_viewHistory_clicked()
{
    tkUserModel *m = tkUserModel::instance();
    QMessageBox mb( qApp->activeWindow() );
    mb.setIcon( QMessageBox::Information );
    mb.setWindowTitle( qApp->activeWindow()->windowTitle() );
    mb.setText( tr( "Login history." ) );
    mb.setInformativeText( tr( "User %1\nLast connection : %2" )
                           .arg( m->index(m_Row,User::UserName).data().toString() )
                           .arg( m->index(m_Row,User::UserLastLogin).data().toDateTime().toString() ) );
    mb.setDetailedText( m->index(m_Row,User::UserLoginHistory).data().toString() );
    mb.setStandardButtons(QMessageBox::Ok);
    mb.setDefaultButton(QMessageBox::Ok);
    mb.exec();
}
