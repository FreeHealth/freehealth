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
#include "mfPrinter.h"
#include <tkLog.h>

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QTextDocument>
#include <QTextFrame>
#include <QPrinter>
#include <QPrintDialog>

mfPrinter::mfPrinter( mfObject * mfo )
          : m_Object ( mfo )
{
     Q_ASSERT_X( mfo, "Passing null pointer to mfPrinter::mfPrinter", "Cannot work without mfObject" );
     setObjectName( "mfPrinter" );
     tkLog::addMessage( this, tr( "Print : preparing to print %1" ).arg( mfo->name() ) );
}


mfPrinter::~mfPrinter()
{
}


bool mfPrinter::print()
{
     // Show PrintDialog with ediable preview of the form
     QPrinter printer;
     printer.setCreator( qApp->applicationName() );
     QPrintDialog printDialog( &printer, 0 );
     printDialog.addEnabledOption( QAbstractPrintDialog::PrintToFile );
     if ( printDialog.exec() )
     {
          printer.setDocName( m_Object->name() + QDateTime::currentDateTime().toString() );
          printer.setPageMargins( 10, 10, 10, 10, QPrinter::Millimeter );
          QTextDocument doc;
          doc.setHtml( toPrintableHtml() );
          doc.print( &printer );
          tkLog::addMessage( this, tr( "Print : printing doc %1 to %2" ).arg( printer.docName(), printer.printerName() ) );
     }

     return true;
}

QString mfPrinter::retrieveFile( const QString & fileName ) const
{
     QFile file( fileName );
     Q_ASSERT_X( file.open( QIODevice::ReadOnly | QIODevice::Text ),
                 "mfPrinter::objectToPrintableHtml() : resource file cannot be opened",
                 qPrintable( QString( "resource file = %1" ).arg( fileName ) ) );
     return file.readAll();
}


QString mfPrinter::toPrintableHtml()
{
     // HTML HEADER
     QString header = retrieveFile( ":/htmlTranformers/header.html" );
     header.replace( "$LANG$", QLocale().name() );
     header.replace( "$TITLE$", qApp->applicationName() + " " + m_Object->label() );
     header.replace( "$KEYWORDS$", "" );
     header.replace( "$DESCRIPTION$", "" );
     header.replace( "$AUTHORS$", m_Object->param( mfObject::Param_Authors ).toString() );
     header.replace( "$FMFVERSION$", m_Object->param( mfObject::Param_Version ).toString() );
     header.replace( "$COPYRIGHT$", "" );
     header.replace( "$DATE$", m_Object->param( mfObject::Param_CreationDate ).toString() );

     // HTML FOOTER
     QString footer = retrieveFile( ":/htmlTranformers/footer.html" );

     // Prepare the form
     m_ToHtml.clear();
     addHtmlObject( m_Object, m_ToHtml );

     m_ToHtml.prepend( header );
     m_ToHtml.append( footer );

     return m_ToHtml.join( "" );
}

QString mfPrinter::prepareLine( mfObject * mfo, QString text )
{
     return QString( "       <table width=100% border=0 cellpadding=5 cellspacing=5>\n"
                     "         <tr>\n"
                     "           <td style=\"vertical-align: middle;\" border=0 width=35%>\n"
                     "             <p align=right>%1</p>\n"
                     "           </td>\n"
                     "           <td border=0>\n"
                     "              %2\n"
                     "           </td>\n"
                     "         </tr>\n"
                     "       </table>\n" ).arg( mfo->label(), text );

}

QString mfPrinter::prepareTable( QStringList & list, int nbCol, int border )
{
     // Create table columns
     QString formHtml = "  <table width=100% border=" + QString::number( border );
     formHtml += " cellpadding=2 cellspacing=2>\n";
     QString tmp = "";
     int i = 0;
     foreach( QString str, list )
     {
          if ( ( i % nbCol ) == 0 )  tmp = "  <tr>\n" ;

          tmp += QString( "   <td width=%1%>\n%2\n   </td>\n" ).arg( 100 / nbCol ).arg( str );

          i++;
          if ( ( i % nbCol ) == 0 )  tmp += "  </tr>\n" ;
          formHtml.append( tmp );
          tmp.clear();
     }
     formHtml.append( "  </table>\n\n" );
     return formHtml;
}

void mfPrinter::addHtmlObject( mfObject * mfo, QStringList & list )
{
     int t = mfo->type();
     QString tmp = "";
     switch ( t )
     {
          case mfObject::Type_Form :
               createForm( mfo );
               break;
          case mfObject::Type_Group :
               list.append( createGroup( mfo ) );
               break;
          case mfObject::Type_Date :
               tmp.fill( '_' , 20 );
               list.append( prepareLine( mfo,  tmp ) );
               break;
          case mfObject::Type_Check :
               list.append( QString( QChar( CheckChar ) ) + " " + mfo->label() );
               break;
          case mfObject::Type_Radio :
               list.append( createMultiPossibleValues( mfo, QChar( RadioChar ) )  );
               break;
          case mfObject::Type_MultiList :
               list.append( createMultiPossibleValues( mfo, QChar(  0x25BB ) ) );
               break;
          case mfObject::Type_UniqueList :
          case mfObject::Type_Combo :
               list.append( createMultiPossibleValues( mfo, QChar( RadioChar ) ) );
               break;
          case mfObject::Type_ShortText :
               tmp.fill( '_' , 30 );
               list.append( prepareLine( mfo, tmp ) );
               break;
          case mfObject::Type_LongText :
               int i = 0;
               for ( i = 0; i < 9; ++i ) tmp.append( "|<br />" );
               list.append( prepareLine( mfo, tmp ) );
               break;


//        case mfObject::Type_HelpText :
//             createHelpText( mfo );
//             break;


     }

}

void mfPrinter::createForm( mfObject * formRoot )
{
     // Add form header
     int nbCol = formRoot->getNumberOfColumn();
     QString header = retrieveFile( ":/htmlTranformers/formHeader.html" );
     header.replace( "$FORMNAME$", m_Object->param( mfObject::Param_Label ).toString().toUpper() );
     header.replace( "$AUTHORS$", m_Object->param( mfObject::Param_Authors ).toString() );
     header.replace( "$VERSION$", m_Object->param( mfObject::Param_Version ).toString() );
     header.replace( "$DATE$", m_Object->param( mfObject::Param_CreationDate ).toString() );
     header.replace( "$NUMCOLS$", QString::number( nbCol ) );

     m_ToHtml.append( header );

     // Prepare stringlist for the form
     QStringList formList;
     foreach( mfObject* chd, formRoot->mfChildren() )
     {  addHtmlObject( chd, formList ) ;  }

     // Add columns and table footer
     m_ToHtml.append( prepareTable( formList, nbCol ) );
     m_ToHtml.append( retrieveFile( ":/htmlTranformers/formFooter.html" ) );

}

QString mfPrinter::createGroup( mfObject * formRoot )
{
     // Add form header
     int nbCol = formRoot->getNumberOfColumn();

     // Prepare stringlist for the form
     QStringList formList;
     foreach( mfObject* chd, formRoot->mfChildren() )
     {  addHtmlObject( chd, formList ) ;  }

     // Add columns and table footer
     QString tmp = "     <!-- *********** GROUP *********** -->\n";
     tmp.append( QString( "    <div class=\"groupTitle\">%1</div>\n" ).arg( formRoot->label() ) );
     tmp.append( "    <div class=\"groupForm\">\n" );
     tmp.append( prepareTable( formList, nbCol, 1 ) );
     tmp.append( "    </div>" );
     return tmp;
}

QString mfPrinter::createMultiPossibleValues( mfObject * mfo, QChar car )
{
     // Foreach possibleValues add a radio button to the group
     QString posVal = "";
     QString begin = "";
     QString end = "";
     foreach( QString str, mfo->possibleValues() )
     {
          if ( mfo->param( mfObject::Param_Options ).toStringList().contains( "horizontal" ) )
          {
               posVal.append( QString( "          %1 %2&nbsp;&nbsp;&nbsp;\n" )
                              .arg( car ).arg( Qt::escape( str ) ) );
               if ( begin.isEmpty() ) begin = "          <div class=\"item\">";
               if ( end.isEmpty() ) end = "</div>\n";
          }
          else
               posVal.append( QString( "          <div class=\"item\">%1 %2</div>\n" )
                              .arg( car ).arg( Qt::escape( str ) ) );
     }
     posVal.append( end );
     posVal.prepend( begin );
// qWarning() << posVal << mfo->param( mfObject::Param_Options ).toStringList();
     posVal.chop( 7 );
     posVal.append( "\n" );
     QString tmp = QString( "       <table style=\"text-align: rigth; \" width=100%"
                            "              border=1 cellpadding=2 cellspacing=2> \n"
                            "         <tr>\n"
                            "           <td style=\"vertical-align: middle;\" border=0 width=35%>%1</td> \n"
                            "           <td border=1 >\n"
                            "               %2\n"
                            "           </td>\n"
                            "         </tr>\n"
                            "       </table> \n" ).arg( mfo->label(), posVal );
     return tmp;
}

