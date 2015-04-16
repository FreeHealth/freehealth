/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                     **
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
#include <mfCrashRecovererDialog.h>

#include <mfCore.h>
#include <mfSettings.h>

#include <tkLog.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

mfCrashRecovererDialog::mfCrashRecovererDialog( QWidget * parent )
          : QDialog( parent )
{
     setupUi( this );
     setObjectName( "mfCrashRecovereDialog" );
     setWindowTitle( qApp->applicationName() );
     appNameLabel->setText( qApp->applicationName() );

     populateListWidget();

}


mfCrashRecovererDialog::~mfCrashRecovererDialog()
{
}

void mfCrashRecovererDialog::populateListWidget()
{
     // ForEach files in tmp dir create an item
     QDir tmpDir( mfCore::settings()->temporaryPath() );
     tmpDir.setNameFilters( QStringList() << "*.tmp" );

     listWidget->clear();

     foreach( QFileInfo fileInfo, tmpDir.entryInfoList( QDir::Files ) )
     {
          QFile file( fileInfo.absoluteFilePath() );
          if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
               {     QMessageBox::warning( this, qApp->applicationName(),
                               tr( "Recovery file %1 cannot be open.\nError : %2" )
                               .arg( fileInfo.absoluteFilePath() ).arg( file.error() ) );
                     tkLog::addError( objectName() , tr( "Recovery file %1 cannot be open.\nError : %2" )
                                      .arg( fileInfo.absoluteFilePath() ).arg( file.error() ) );
                     return ;
               }

          QTextStream in( &file );
          // ingore first line of this file
          file.readLine();
          // absolute form filename
          QString formName = file.readLine();
          formName = formName.mid( formName.lastIndexOf( QDir::separator() ) + 1 );
          formName.replace( "\n", "" );
          formName.append( "\n" );
          QListWidgetItem * item = new QListWidgetItem( listWidget );
          item->setText( tr( "Root Form : %1Last save : %2" )
                         .arg ( formName , fileInfo.lastModified().toString( "ddd dd MMM yyyy hh:mm:ss" ) ) );
          item->setData( Qt::UserRole, fileInfo.absoluteFilePath() );
     }
}

void mfCrashRecovererDialog::on_recoverSelectionBut_clicked()
{
     QMessageBox::information( this, qApp->applicationName(),
                               tr( "This function is not fully implemented for now.\n Work in progress." ) );
}

void mfCrashRecovererDialog::on_deleteSelectionBut_clicked()
{
     foreach( QListWidgetItem* item, listWidget->selectedItems() )
     QFile::remove( item->data( Qt::UserRole ).toString() );
     populateListWidget();
}
