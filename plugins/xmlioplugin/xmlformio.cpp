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
#include "xmlformio.h"
#include "xmlformioconstants.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/global.h>
#include <coreplugin/icore.h>
#include <coreplugin/formmanager.h>
#include <coreplugin/iformitem.h>
#include <coreplugin/iformwidgetfactory.h>

//#include <coreplugin/mfObject.h>

#include <translationutils/constanttranslations.h>

// including Qt headers
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCodec>
#include <QFileInfo>


//class XmlIOPrivate {
//public:
//    XmlIOPrivate() {}
//    ~XmlIOPrivate() {}
//public:
//};


using namespace XmlForms;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
    static QHash<QString, Core::IFormWidgetFactory *> m_PlugsFactories;
    static QHash<QString, int> m_ScriptsTypes;
    static QHash<QString, int> m_ValuesTypes;
    static QHash<QString, int> m_SpecsTypes;
}

inline static void warnXmlReadError(const QString &file, const QString &msg, const int line, const int col)
{
    Core::tkGlobal::warningMessageBox(
            Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file),
            Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
            .arg(msg).arg(line).arg(col),"",qApp->applicationName());
}
inline static Core::FormManager *fm() {return Core::ICore::instance()->formManager();}
inline static ExtensionSystem::PluginManager *pm() {return ExtensionSystem::PluginManager::instance();}
inline static void refreshPlugsFactories()
{
    ::m_PlugsFactories.clear();
    foreach(Core::IFormWidgetFactory *fact, pm()->getObjects<Core::IFormWidgetFactory>()) {
        foreach(const QString &widgetname, fact->providedWidgets()) {
            ::m_PlugsFactories.insert(widgetname,fact);
        }
    }
}

inline static Core::FormMain *createNewForm(const QDomElement &element, Core::FormItem *item = 0)
{
    QString name = element.firstChildElement(Constants::TAG_NAME).text();
    Core::FormMain *parent = fm()->getParent<Core::FormMain>(item);
    return fm()->createForm(name, parent);
}

inline static bool populateValues(Core::FormItem *item, const QDomElement &root)
{
    QDomElement element = root.firstChildElement();
//    qWarning() << "Values" << root.tagName() << element.tagName();
    QString lang = root.attribute(Constants::ATTRIB_LANGUAGE,Core::Constants::ALL_LANGUAGE);
    while (!element.isNull()) {
        int id = element.attribute(Constants::ATTRIB_ID,0).toInt();
        QVariant val = element.text();
        int type = ::m_ValuesTypes.value(element.tagName(),-1);
        if (type != -1) {
            item->valueReferences()->setValue(type,id,val,lang);
        } else {
            if (element.tagName().compare(Constants::TAG_VALUE_DEFAULT, Qt::CaseInsensitive)==0) {
                item->valueReferences()->setDefaultValue(val,lang);
            }
        }
        element = element.nextSiblingElement();
    }
    return true;
}

inline static bool populateScripts(Core::FormItem *item, const QDomElement &root)
{
    QDomElement element = root.firstChildElement();
    QString lang = root.attribute(Constants::ATTRIB_LANGUAGE,Core::Constants::ALL_LANGUAGE).left(2);
    while (!element.isNull()) {
        QString script = element.text();
        int type = ::m_ScriptsTypes.value(element.tagName(),Core::FormItemScripts::Script_OnDemand);
        item->scripts()->setScript(type,script,lang);
        element = element.nextSiblingElement();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  XmlFormIO  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
XmlFormIO::XmlFormIO(const QString &absFileName, QObject *parent) :
        IFormIO(absFileName, parent), m_AbsFileName(absFileName)
{
    setObjectName("XmlFormIO");
    ::m_ScriptsTypes.clear();
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONLOAD, Core::FormItemScripts::Script_OnLoad);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_POSTLOAD, Core::FormItemScripts::Script_PostLoad);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEMAND, Core::FormItemScripts::Script_OnDemand);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUECHANGED, Core::FormItemScripts::Script_OnValueChanged);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUEREQUIERED, Core::FormItemScripts::Script_OnValueRequiered);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEPENDENCIESCHANGED, Core::FormItemScripts::Script_OnDependentValueChanged);
    ::m_ValuesTypes.clear();
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_NUMERICAL, Core::FormItemValues::Value_Numerical);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_SCRIPT, Core::FormItemValues::Value_Script);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_POSSIBLE, Core::FormItemValues::Value_Possible);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_DEPENDENCIES, Core::FormItemValues::Value_Dependency);
    ::m_SpecsTypes.clear();
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_PLUGINNAME, Core::FormItemSpec::Spec_Plugin);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_AUTHORS, Core::FormItemSpec::Spec_Author);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_CATEGORY, Core::FormItemSpec::Spec_Category);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_LICENSE, Core::FormItemSpec::Spec_License);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_CREATIONDATE, Core::FormItemSpec::Spec_CreationDate);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_BIBLIOGRAPHY, Core::FormItemSpec::Spec_Bibliography);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_DESCRIPTION, Core::FormItemSpec::Spec_Description);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_LABEL, Core::FormItemSpec::Spec_Label);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_VERSION, Core::FormItemSpec::Spec_Version);
}

XmlFormIO::~XmlFormIO()
{
}

QStringList XmlFormIO::fileFilters() const
{
    return QStringList() << tr("FreeMedForms Form File (*.%1)").arg(Constants::DOCTYPE_EXTENSION);
}

bool XmlFormIO::setFileName(const QString &absFileName)
{
    if (!QFile(absFileName).exists())
        return false;
    if (QFileInfo(absFileName).suffix()==managedFileExtension()) {
        m_AbsFileName = absFileName;
        return true;
    }
    return false;
}

bool XmlFormIO::canReadFile() const
{
    if (m_AbsFileName.isEmpty())
        return false;

    if (QFileInfo(m_AbsFileName).suffix()!=managedFileExtension()) {
        return false;
    }
    // Check contents
    QString contents = Core::tkGlobal::readTextFile(m_AbsFileName, Core::tkGlobal::DontWarnUser);
    if (contents.count("<"+QString(Constants::TAG_NEW_FORM)+">") == contents.count("</"+QString(Constants::TAG_NEW_FORM)+">")) {
        return true;
    } else {
        Utils::Log::addError(this, Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(m_AbsFileName));
    }
    return false;
}

bool XmlFormIO::loadForm()
{
    Q_ASSERT(!m_AbsFileName.isEmpty());
    refreshPlugsFactories();
    return loadForm(m_AbsFileName,0);
}

bool XmlFormIO::loadForm(const QString &file, Core::FormMain *rootForm)
{
    // Read contents if necessary
    QString contents;
    contents = Core::tkGlobal::readTextFile(file, Core::tkGlobal::DontWarnUser);
    if (contents.isEmpty()) {
        warnXmlReadError(file, tr("File is empty."), 0, 0);
        /** \todo return a FormObject with a helptext that explains the error ? */
        return false;
    }

    QDomDocument document;
    QString errorMsg;
    int errorLine, errorColumn;
    if (!document.setContent(contents, &errorMsg, &errorLine, &errorColumn)) {
        warnXmlReadError(file, errorMsg, errorLine, errorColumn);
        return false;
    }

    // Check doctype name
    if (document.doctype().name().compare(Constants::DOCTYPE_NAME,Qt::CaseInsensitive)!=0) {
        warnXmlReadError(file, tr("This file is not a FreeMedForms XML file. Document type name mismatch."), 0, 0);
        return false;
    }

    // Check root element --> must be a form type
    QDomElement root = document.documentElement();
    // in case of no rootForm is passed --> XML must start with a file inclusion or a newform tag
    if (!rootForm) {
        if ((root.tagName().compare(Constants::TAG_NEW_FORM)!=0) && (root.tagName().compare(Constants::TAG_ADDFILE)!=0)) {
            warnXmlReadError(file, tr("Wrong root tag %1 %2.").arg(root.tagName()).arg(Constants::TAG_NEW_FORM), 0, 0);
            return false;
        }
        rootForm = createNewForm(root,0);
    }

    if (!loadElement(rootForm,root))
        return false;
    rootForm->createDebugPage();
    createWidgets();
    return true;
}

bool XmlFormIO::loadElement(Core::FormItem *item, QDomElement &rootElement)
{
    QDomElement element = rootElement.firstChildElement();
    while (!element.isNull()) {
        int i=0;

        // Create a nem FormItem ?
        i = Constants::createTags.indexOf(element.tagName());
        if (i != -1) {
            createElement(item,element);
            element = element.nextSiblingElement();
            continue;
        }

        // Values ?
        if (element.tagName() == Constants::TAG_VALUE) {
            populateValues(item,element);
            element = element.nextSiblingElement();
            continue;
        }

        // Script ?
        if (element.tagName() == Constants::TAG_SCRIPT) {
            populateScripts(item,element);
            element = element.nextSiblingElement();
            continue;
        }

        // Spec ?
        i = ::m_SpecsTypes.value(element.tagName(),-1);
        if (i != -1) {
            QString lang = element.attribute(Constants::ATTRIB_LANGUAGE, Core::Constants::ALL_LANGUAGE);
            item->spec()->setValue(::m_SpecsTypes.value(element.tagName()),element.text(), lang);
            element = element.nextSiblingElement();
            continue;
        }
    //             // optional?
//            if (element.hasAttribute(Constants::ATTRIB_OPTIONNAL)) {
//                if (element.attribute(Constants::ATTRIB_OPTIONNAL).compare("true",Qt::CaseInsensitive)==0)
//                    item->valueReferences()->setOptionnal(true);
//                else
//                    item->valueReferences()->setOptionnal(false);

        // Add a file ?
        if (element.tagName().compare(Constants::TAG_ADDFILE,Qt::CaseInsensitive)==0) {
            QDomDocument doc;
            QString fileName = element.text();
            if (QFileInfo(fileName).isRelative())
                fileName.prepend(QFileInfo(m_AbsFileName).absoluteDir().absolutePath() + QDir::separator());
            fileName = QDir::cleanPath(fileName);
            loadForm(fileName,0);
        }

        // Options
        item->addExtraData(element.tagName(), element.text());
        element = element.nextSiblingElement();
    }
    return true;
}

bool XmlFormIO::createElement(Core::FormItem *item, QDomElement &element)
{
    qWarning() << "XmlFormIO create element" << element.text();
    // new item
    if (element.tagName().compare(Constants::TAG_NEW_ITEM, Qt::CaseInsensitive)==0) {
        if (item) {
            loadElement(item->createChildItem(), element);
            return true;
        }
        else
            return false; // XML ERROR --> an item MUST have a parent
    }

    // new form
    if (element.tagName().compare(Constants::TAG_NEW_FORM, Qt::CaseInsensitive)==0) {
        // create a new form
        item = fm()->createForm(element.firstChildElement(Constants::TAG_NAME).text());
        if (item) {
            loadElement(item, element);
            // read specific form's datas
            return true;
        }
        else
            return false;
    }

    if (element.tagName().compare(Constants::TAG_NEW_PAGE, Qt::CaseInsensitive)==0) {
        // create a new page
        item = item->createPage(element.firstChildElement(Constants::TAG_NAME).text());
        /** \todo add page to a form */
        if (item) {
            loadElement(item, element);
            // read specific page's datas
            return true;
        }
        else
            return false;
    }

    return false;
}

bool XmlFormIO::createItemWidget(Core::FormItem *item, QWidget *parent)
{
    // does plugin was inform in the xml file ?
    if (item->spec()->pluginName().isEmpty()) {
        item->setFormWidget(0);
        return false;
    }

    // does asked widget exists in the plugins ?
    QString askedWidget = item->spec()->pluginName().toLower();
    if (!m_PlugsFactories.keys().contains(askedWidget)) {
        item->setFormWidget(0);
        qWarning() << "asked widget does not exists in plugins' widgets' list" << askedWidget << m_PlugsFactories;
        return false;
    }

    // get the widget
    Core::IFormWidgetFactory *factory = m_PlugsFactories.value(askedWidget);
    Core::IFormWidget *w = 0;
    w = factory->createWidget(askedWidget,item);
    if (w->isContainer()) {
        foreach(Core::FormItem *child, item->formItemChildren()) {
//            Core::IFormWidget *wchild = factory->createWidget(child->spec()->pluginName(),child,w);
//            w->addWidgetToContainer(wchild);
//            child->setFormWidget(wchild);
            createItemWidget(child,w);
        }
    }
    item->setFormWidget(w);
    Core::IFormWidget *p = qobject_cast<Core::IFormWidget*>(parent);
    if (p)
        p->addWidgetToContainer(w);

//    toReturn = plug->getWidget( mfo, parent );
//
//    if ( plug->isContainer( widgetId ) )
//    {
//        foreach( mfObject * chd, mfo->mfChildren() )
//        {
//            mfAbstractWidget * chdWgt = getWidget( chd, toReturn );
//            if ( chdWgt )
//                toReturn->addWidgetToContainer( chdWgt );
//        }
//    }
//
    return true;
}

bool XmlFormIO::createFormWidget(Core::FormMain *form)
{
    // Create a new Widget with a vbox and put the label
    QWidget *w = new QWidget();
    if (!createItemWidget(form, 0))
        return false;
//    m_WidgetToStack->setParent( this );

//    // All this goes into a scrollarea
//    QScrollArea * area = new QScrollArea( m_Stack );
//    area->setWidget( m_WidgetToStack );
//    area->setWidgetResizable( true );
//
//    // Add widget to stack
//    int id = m_Stack->addWidget( area );
//    m_FormList.append( mfo );
//    // save id in treeitem with Qt::UserRole
//    item->setData( LabelColumn, Qt::UserRole, id );

    // Analysing all children, take care to the tree !!!
//    foreach( mfObject* chd, mfo->mfChildren() )
//        createForm( chd, item );
    //      else                 createForm(mfo->children().first(), m_Item->parent());
    return true;
}

bool XmlFormIO::createWidgets()
{
    // foreach Forms in FormManager
    foreach(Core::FormMain *form, fm()->forms()) {
        // create the form
        createFormWidget(form);
    }
    return true;
}

//bool mfXmlIO::saveForm(const mfIOPlace &place, const mfObject &object)
//{
//     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::saveForm()", "<place> is not a local file!");
//
//     QFile file( place.path() );
//
//     // Try to create/open save file
//     if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
//          QMessageBox::warning( 0, qApp->applicationName(),
//                                QObject::tr( "Cannot write to file %1:\n%2." )
//                                .arg( place.path(), file.errorString() ) );
//          return false;
//     }
//
//     // Fill file
//     QTextStream out( &file );
//     out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
//     out.setCodec( QTextCodec::codecForName( "UTF-8" ) );
//
//     QDomDocument doc( qApp->applicationName() );
//     saveObjectInDom( object, doc );
//     out << doc.toString( 3 );
//
//     file.close();
//
//     // Manage temporary file
//     mfCore::renewTemporaryFile();
//
//     return true;
//}
//
//void mfXmlIO::saveObjectInDom( const mfObject &object, QDomNode &root ) const
//{
//     QDomDocument doc = root.ownerDocument();
//
//     QDomElement medFormElem = doc.createElement( "MedForm" );
//     root.appendChild( medFormElem );
//
//     QStringList languages = object.translatedParams().keys();
//
//     // save parameters in order of their enumeration in the type
//     for ( int keyParam = 0; keyParam < mfObject::Param_MaxType; ++keyParam )
//     {
//          const QString &key = mfObject::paramList[keyParam];
//
//          // type is a special case; not in parameters and no translations
//          if ( keyParam == mfObject::Param_Type )
//               tkGlobal::xmlWrite( medFormElem, key, mfObject::typeNames[object.type()] );
//
//          foreach ( const QString &lang, languages )
//          {
//               const QMapParams &params = object.translatedParams()[lang];
//               QMapParams::const_iterator it = params.constFind( keyParam );
//               if ( it != params.constEnd() )
//               {
//                    const QVariant &value = *it;
//                    if ( value.canConvert( QVariant::String ) )
//                    {
//                         QDomElement valueElem = doc.createElement( key );
//                         medFormElem.appendChild( valueElem );
//                         if ( !key.compare( mfObject::paramList[mfObject::Param_Name] ) && !object.isRequired() )
//                              valueElem.setAttribute( "completion", "optional" );
//
//                         if ( lang.compare( "default", Qt::CaseInsensitive ) )
//                              valueElem.setAttribute( "lang", lang );
//                         valueElem.appendChild( doc.createTextNode( value.toString() ) );
//                    }
//                    else
//                    {
//                         int i = 0;
//                         foreach ( const QString &str, value.toStringList() )
//                         {
//                              QDomElement valueElem = doc.createElement( key );
//                              medFormElem.appendChild( valueElem );
//                              valueElem.setAttribute( "id", i++ );
//                              if ( lang.compare( "default", Qt::CaseInsensitive ) )
//                                   valueElem.setAttribute( "lang", lang );
//
//                              QDomText t = doc.createTextNode( str );
//                              valueElem.appendChild( t );
//                         }
//                    }
//               }
//          }
//     }
//
//     // save children
//     foreach ( mfObject *child, object.mfChildren() )
//     saveObjectInDom( *child, medFormElem );
//}

//bool mfXmlIO::loadFormData(const mfIOPlace &place, mfObject &object)
//{
//     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::loadFormData()", "<place> is not a local file!");
//
//     QFile file( QDir::cleanPath( place.path() ));
//     if ( !file.open( QFile::ReadOnly | QFile::Text ) )
//     {
//          QMessageBox::warning( 0, qApp->applicationName(),
//                                QObject::tr( "Cannot read file %1:\n%2." ).arg( place.path(), file.errorString() ) );
//          return 0;
//     }
//
//     QDomDocument document;
//
//     QString errorMsg;
//     int errorLine, errorColumn;
//     if ( !document.setContent( &file, &errorMsg, &errorLine, &errorColumn ) )
//     {
//          QMessageBox::warning( 0, qApp->applicationName(),
//                                QObject::tr( "%1 at line %2, column %3" )
//                                .arg( errorMsg ).arg( errorLine ).arg( errorColumn ) );
//          return 0;
//     }
//
//     file.close();
//
//     return loadObjectDataFromDom( object, document.documentElement() );
//}
//
//bool mfXmlIO::saveFormData(const mfIOPlace &place, const mfObject &object)
//{
//     Q_ASSERT_X(place.type() == mfIOPlace::LocalFile, "mfXmlIO::saveFormData()", "<place> is not a local file!");
//
//     QFile file( place.path() );
//
//     // Try to create/open save file
//     if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
//     {
//          QMessageBox::warning( 0, qApp->applicationName(),
//                                QObject::tr( "Cannot write to file %1:\n%2." )
//                                .arg( place.path(), file.errorString() ) );
//          return false;
//     }
//
//     // Fill file
//     QTextStream out( &file );
//     out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
//     out.setCodec( QTextCodec::codecForName( "UTF-8" ) );
//
//     QDomDocument doc( "freemedformdata" );
//     saveObjectDataInDom( object, doc );
//     out << doc.toString( 3 );
//
//     file.close();
//
//     // Manage temporary file
//     mfCore::renewTemporaryFile();
//
//     return true;
//}

//void mfXmlIO::saveObjectDataInDom( const mfObject &object, QDomNode &root ) const
//{
//     QDomDocument doc = root.ownerDocument();
//
//     if (!object.isModified())
//          return;
//
//     QDomElement medFormDataElem = doc.createElement( "MedFormData" );
//     root.appendChild( medFormDataElem );
//
//     // Save name
//     QDomElement nameElem = doc.createElement( "name" );
//     medFormDataElem.appendChild(nameElem);
//     nameElem.appendChild( doc.createTextNode( object.name() ) );
//
//     // Save version (only for forms)
//     if (object.type() == mfObjectFundamental::Type_Form)
//     {
//          QDomElement nameElem = doc.createElement( "form_version" );
//          medFormDataElem.appendChild(nameElem);
//          nameElem.appendChild( doc.createTextNode( object.param( mfObjectFundamental::Param_Version ).toString() ) );
//     }
//
//     // Save selected values
//     QStringList values = object.values();
//     foreach (const QString &value, object.values())
//     {
//          QDomElement valueElem = doc.createElement( "selected_value" );
//          medFormDataElem.appendChild(valueElem);
//          valueElem.appendChild( doc.createTextNode( value ) );
//     }
//
//     // Save children
//     foreach ( mfObject *child, object.mfChildren() )
//          saveObjectDataInDom( *child, medFormDataElem );
//}

//bool mfXmlIO::loadObjectDataFromDom(mfObject &object, const QDomElement &rootElement)
//{
//     // Save form name?
//     QDomElement nameElem = rootElement.firstChildElement("name");
//     if (nameElem.isNull()) // No name element => break
//          return false;
//
//     QString dataFormName = nameElem.text();
//     if (dataFormName.compare(object.name(), Qt::CaseInsensitive)) // names are not equals => break
//          return false;
//
//     // Get selected values
//     QStringList values;
//     QDomElement elem = rootElement.firstChildElement("selected_value");
//     while (!elem.isNull())
//     {
//          values << elem.text().trimmed();
//          elem = elem.nextSiblingElement("selected_value");
//     }
//
//     if (!object.setValues(values)) // Values are not of the excepted form => break
//          return false;
//
//     // Children
//     QDomElement childElem = rootElement.firstChildElement("MedFormData");
//     while (!childElem.isNull()) {
//          QDomElement nameElem = childElem.firstChildElement("name");
//          if (!nameElem.isNull()) {
//               mfObject *child = object.childByName(nameElem.text());
//               if (child)
//                    if (!loadObjectDataFromDom(*child, childElem))
//                         return false;
//          }
//          childElem = childElem.nextSiblingElement("MedFormData");
//     }
//
//     return true;
//}

