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
#include "mfXmlIO.h"

// including toolkit headers
#include <tkGlobal.h>

// including Qt headers
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCodec>

mfXmlIO::mfXmlIO() : mfIOInterface()
{
     // define plugins informations
     mPluginInfos.Name = PLUGIN_NAME;
     mPluginInfos.Caption = tr( "XML I/O plugin" );
     mPluginInfos.Description = tr( "Plugin for XML save and load operations of mfObjects" );
     mPluginInfos.Author = "Eric Maeker,MD <eric.maeker@gmail.com>, Guillaume Denry<guillaume.denry@gmail.com>";
     mPluginInfos.Type = mfBaseInterface::iIO;
     mPluginInfos.Version = "1.0.0";
     mPluginInfos.Enabled = false;
}

mfObject *mfXmlIO::loadForm(const mfIOPlace &place)
{
     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::loadForm()", "<place> is not a local file!");

     QFile file( QDir::cleanPath( place.path() ));
     if ( !file.open( QFile::ReadOnly | QFile::Text ) ) {
          QMessageBox::warning( 0, qApp->applicationName(),
                                QObject::tr( "Cannot read file %1:\n%2." ).arg( place.path(), file.errorString() ) );
          return 0;
     }

     QDomDocument document;

     QString errorMsg;
     int errorLine, errorColumn;
     if ( !document.setContent( &file, &errorMsg, &errorLine, &errorColumn ) ) {
          QMessageBox::warning( 0, qApp->applicationName(),
                                QObject::tr( "%1 at line %2, column %3" )
                                .arg( errorMsg ).arg( errorLine ).arg( errorColumn ) );
          return 0;
     }

     file.close();

     mfObject *object = new mfObject;
     object->setAbsoluteFileName( QDir::cleanPath( place.path() ) );

     loadObjectFromDom( *object, document.documentElement() );

     return object;
}

bool mfXmlIO::saveForm(const mfIOPlace &place, const mfObject &object)
{
     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::saveForm()", "<place> is not a local file!");

     QFile file( place.path() );

     // Try to create/open save file
     if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
          QMessageBox::warning( 0, qApp->applicationName(),
                                QObject::tr( "Cannot write to file %1:\n%2." )
                                .arg( place.path(), file.errorString() ) );
          return false;
     }

     // Fill file
     QTextStream out( &file );
     out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
     out.setCodec( QTextCodec::codecForName( "UTF-8" ) );

     QDomDocument doc( qApp->applicationName() );
     saveObjectInDom( object, doc );
     out << doc.toString( 3 );

     file.close();

     // Manage temporary file
     mfCore::renewTemporaryFile();

     return true;
}

void mfXmlIO::saveObjectInDom( const mfObject &object, QDomNode &root ) const
{
     QDomDocument doc = root.ownerDocument();

     QDomElement medFormElem = doc.createElement( "MedForm" );
     root.appendChild( medFormElem );

     QStringList languages = object.translatedParams().keys();

     // save parameters in order of their enumeration in the type
     for ( int keyParam = 0; keyParam < mfObject::Param_MaxType; ++keyParam )
     {
          const QString &key = mfObject::paramList[keyParam];

          // type is a special case; not in parameters and no translations
          if ( keyParam == mfObject::Param_Type )
               tkGlobal::xmlWrite( medFormElem, key, mfObject::typeNames[object.type()] );

          foreach ( const QString &lang, languages )
          {
               const QMapParams &params = object.translatedParams()[lang];
               QMapParams::const_iterator it = params.constFind( keyParam );
               if ( it != params.constEnd() )
               {
                    const QVariant &value = *it;
                    if ( value.canConvert( QVariant::String ) )
                    {
                         QDomElement valueElem = doc.createElement( key );
                         medFormElem.appendChild( valueElem );
                         if ( !key.compare( mfObject::paramList[mfObject::Param_Name] ) && !object.isRequired() )
                              valueElem.setAttribute( "completion", "optional" );

                         if ( lang.compare( "default", Qt::CaseInsensitive ) )
                              valueElem.setAttribute( "lang", lang );
                         valueElem.appendChild( doc.createTextNode( value.toString() ) );
                    }
                    else
                    {
                         int i = 0;
                         foreach ( const QString &str, value.toStringList() )
                         {
                              QDomElement valueElem = doc.createElement( key );
                              medFormElem.appendChild( valueElem );
                              valueElem.setAttribute( "id", i++ );
                              if ( lang.compare( "default", Qt::CaseInsensitive ) )
                                   valueElem.setAttribute( "lang", lang );

                              QDomText t = doc.createTextNode( str );
                              valueElem.appendChild( t );
                         }
                    }
               }
          }
     }

     // save children
     foreach ( mfObject *child, object.mfChildren() )
     saveObjectInDom( *child, medFormElem );
}

void mfXmlIO::loadObjectFromDom( mfObject &object, const QDomElement &rootElement )
{
     QDomElement paramElem = rootElement.firstChildElement();
     while ( !paramElem.isNull() )
     {
          if ( !paramElem.tagName().compare( "MedForm", Qt::CaseInsensitive ) ) // a child object
               loadObjectFromDom( *( new mfObject( &object ) ), paramElem );
          else if ( !paramElem.tagName().compare( "file", Qt::CaseInsensitive ) ) // a file inclusion
          {
               QString fileName = paramElem.text().trimmed();
               QDir rootDir = QFileInfo( object.absoluteFileName() ).dir();  /* get object parent directory */
               if ( QFileInfo( fileName ).isRelative() )
                    fileName = rootDir.filePath( fileName );

               // Read the file
               mfObject * childObj = loadForm(mfIOPlace::fromLocalFile(fileName));
               if ( childObj )
                    childObj->setParent( &object );
          }
          else
          {
               // optional?
               if ( !paramElem.attribute( "completion" ).compare( "optional", Qt::CaseInsensitive ) )
                    object.setRequired( false );

               object.setParam( mfObject::paramList.indexOf( paramElem.tagName().trimmed() ),
                                paramElem.text().trimmed(),
                                paramElem.attribute( "lang" ),
                                mfObject::AppendParam );
          }

          paramElem = paramElem.nextSiblingElement();
     }
}

bool mfXmlIO::loadFormData(const mfIOPlace &place, mfObject &object)
{
     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::loadFormData()", "<place> is not a local file!");

     QFile file( QDir::cleanPath( place.path() ));
     if ( !file.open( QFile::ReadOnly | QFile::Text ) )
     {
          QMessageBox::warning( 0, qApp->applicationName(),
                                QObject::tr( "Cannot read file %1:\n%2." ).arg( place.path(), file.errorString() ) );
          return 0;
     }

     QDomDocument document;

     QString errorMsg;
     int errorLine, errorColumn;
     if ( !document.setContent( &file, &errorMsg, &errorLine, &errorColumn ) )
     {
          QMessageBox::warning( 0, qApp->applicationName(),
                                QObject::tr( "%1 at line %2, column %3" )
                                .arg( errorMsg ).arg( errorLine ).arg( errorColumn ) );
          return 0;
     }

     file.close();

     return loadObjectDataFromDom( object, document.documentElement() );
}

bool mfXmlIO::saveFormData(const mfIOPlace &place, const mfObject &object)
{
     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::saveFormData()", "<place> is not a local file!");

     QFile file( place.path() );

     // Try to create/open save file
     if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
     {
          QMessageBox::warning( 0, qApp->applicationName(),
                                QObject::tr( "Cannot write to file %1:\n%2." )
                                .arg( place.path(), file.errorString() ) );
          return false;
     }

     // Fill file
     QTextStream out( &file );
     out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
     out.setCodec( QTextCodec::codecForName( "UTF-8" ) );

     QDomDocument doc( "freemedformdata" );
     saveObjectDataInDom( object, doc );
     out << doc.toString( 3 );

     file.close();

     // Manage temporary file
     mfCore::renewTemporaryFile();

     return true;
}

void mfXmlIO::saveObjectDataInDom( const mfObject &object, QDomNode &root ) const
{
     QDomDocument doc = root.ownerDocument();

     if (!object.isModified())
          return;

     QDomElement medFormDataElem = doc.createElement( "MedFormData" );
     root.appendChild( medFormDataElem );

     // Save name
     QDomElement nameElem = doc.createElement( "name" );
     medFormDataElem.appendChild(nameElem);
     nameElem.appendChild( doc.createTextNode( object.name() ) );

     // Save version (only for forms)
     if (object.type() == mfObjectFundamental::Type_Form)
     {
          QDomElement nameElem = doc.createElement( "form_version" );
          medFormDataElem.appendChild(nameElem);
          nameElem.appendChild( doc.createTextNode( object.param( mfObjectFundamental::Param_Version ).toString() ) );
     }

     // Save selected values
     QStringList values = object.values();
     foreach (const QString &value, object.values())
     {
          QDomElement valueElem = doc.createElement( "selected_value" );
          medFormDataElem.appendChild(valueElem);
          valueElem.appendChild( doc.createTextNode( value ) );
     }

     // Save children
     foreach ( mfObject *child, object.mfChildren() )
          saveObjectDataInDom( *child, medFormDataElem );
}

bool mfXmlIO::loadObjectDataFromDom(mfObject &object, const QDomElement &rootElement)
{
     // Save form name?
     QDomElement nameElem = rootElement.firstChildElement("name");
     if (nameElem.isNull()) // No name element => break
          return false;

     QString dataFormName = nameElem.text();
     if (dataFormName.compare(object.name(), Qt::CaseInsensitive)) // names are not equals => break
          return false;

     // Get selected values
     QStringList values;
     QDomElement elem = rootElement.firstChildElement("selected_value");
     while (!elem.isNull())
     {
          values << elem.text().trimmed();
          elem = elem.nextSiblingElement("selected_value");
     }

     if (!object.setValues(values)) // Values are not of the excepted form => break
          return false;

     // Children
     QDomElement childElem = rootElement.firstChildElement("MedFormData");
     while (!childElem.isNull()) {
          QDomElement nameElem = childElem.firstChildElement("name");
          if (!nameElem.isNull()) {
               mfObject *child = object.childByName(nameElem.text());
               if (child)
                    if (!loadObjectDataFromDom(*child, childElem))
                         return false;
          }
          childElem = childElem.nextSiblingElement("MedFormData");
     }

     return true;
}

Q_EXPORT_PLUGIN2( mf_xmlio, mfXmlIO )
