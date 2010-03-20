/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfGlobal.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <tkLog.h>


//#include <QuaZip>

namespace mfGlobal
{

bool unzipFile( const QString & fileName, const QString & pathToUnZippedFiles )
{
//     Q_ASSERT_X( QFile( fileName ).exists() , "MfCore global function unzipFile()",
//                 qPrintable( QString( "File %1 does not exists" ).arg( fileName ) ) );
//
//     Q_ASSERT_X( QDir( pathToUnZippedFiles ).exists() , "MfCore global function unzipFile()",
//                 qPrintable( QString( "Dir %1 does not exists" ).arg( pathToUnZippedFiles ) ) );
//
//     QuaZip zip( fileName );
//     if ( !zip.open( QuaZip::mdUnzip ) )
//     {
//          tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
//          return false;
//     }
//
//     QuaZipFileInfo info;
//     QuaZipFile file( &zip );
//     QFile out;
//     QString name;
//     char c;
//
//     for ( bool more = zip.goToFirstFile(); more; more = zip.goToNextFile() )
//     {
//          if ( !file.open( QIODevice::ReadOnly ) )
//          {
//               tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
//               return false;
//          }
//
//          name = file.getActualFileName();
//
//          if ( file.getZipError() != UNZ_OK )
//          {
//               tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
//               return false;
//          }
//          out.setFileName( pathToUnZippedFiles + QDir::separator() + name );
//
//          // inform user
//          tkLog::addMessage( "mfGlobal", QCoreApplication::translate( "mfGlobal", "Zip : extracting : " ) + out.fileName() );
//
//          if ( !out.open( QIODevice::WriteOnly | QIODevice::Text ) )
//          {
//               tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( out.fileName() ).arg( out.error() ) );
//               return false;
//          }
//          while ( file.getChar( &c ) ) out.putChar( c );
//          out.close();
//
//          if ( file.getZipError() != UNZ_OK )
//          {
//               tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
//               return false;
//          }
//
//          if ( !file.atEnd() )
//          {
//               tkLog::addError( "mfGlobal", QCoreApplication::translate( "mfGlobal", "Zip : read all but not EOF : " ) + fileName );
//               return false;
//          }
//          file.close();
//
//          if ( file.getZipError() != UNZ_OK )
//          {
//               tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
//               return false;
//          }
//     }
//     zip.close();
//     if ( zip.getZipError() != UNZ_OK )
//     {
//          tkLog::addError( "mfGlobal", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
//          return false;
//     }

     return true;
}


void unzipAllFilesIntoDirs( const QStringList & paths )
{
//     foreach( QString p, paths )
//     {
//          QDir d( p );
//          if ( !d.exists() ) continue;
//
//          // get all zip files in dir
//          d.setNameFilters( QStringList() << "*.zip" );
//          d.setFilter( QDir::Files | QDir::NoSymLinks | QDir::Readable );
//          QStringList zipFiles = d.entryList();
//
//          foreach( QString f, zipFiles )
//          {
//               // if file if already unzipped dir with its baseName exists and is populated with files
//               QDir dz( p );
//               dz.setFilter( QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::NoDotAndDotDot );
//               if ( ( dz.cd( QFileInfo( f ).baseName() ) ) &&
//                         ( dz.entryList().count() ) )
//                    continue;
//
//               // d must not change here +++
//               // file was not unzipped by me, so do it
//               // in first create the output directory
//               if ( !d.cd( QFileInfo( f ).baseName()  ) )
//               {
//                    d.mkdir( QFileInfo( f ).baseName() );
//                    dz.cd( QFileInfo( f ).baseName() );
//               }
//               else d.cdUp();
//
//               // in second unzip file to the output directory
//               unzipFile( d.absolutePath() + QDir::separator() + f, dz.absolutePath() );
//          }
//     }
}



}
