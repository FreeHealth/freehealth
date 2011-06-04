/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "xmlformio.h"
#include "xmlformioconstants.h"
#include "xmlformcontentreader.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

//#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorycore.h>

#include <pmhplugin/pmhcore.h>
#include <pmhplugin/pmhcategorymodel.h>

#include <translationutils/constanttranslations.h>

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCodec>
#include <QFileInfo>

using namespace XmlForms;
using namespace Internal;
using namespace Trans::ConstantTranslations;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Category::CategoryCore *categoryCore() {return  Category::CategoryCore::instance();}
static inline PMH::PmhCore *pmhCore() {return PMH::PmhCore::instance();}
static inline Internal::XmlFormContentReader *reader() {return Internal::XmlFormContentReader::instance();}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  XmlFormIO  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
XmlFormIO::XmlFormIO(const QString &absFileName, QObject *parent) :
        IFormIO(absFileName, parent), m_AbsFileName(absFileName), m_Mute(false)
{
    setObjectName("XmlFormIO");
}

XmlFormIO::~XmlFormIO()
{
}

QString XmlFormIO::lastestXmlVersion()
{
    return "0.3.0";
}

bool XmlFormIO::canReadForms(const QString &absFileName) const
{
    if (m_ReadableForms.keys().contains(absFileName)) {
        return m_ReadableForms.value(absFileName);
    }
    m_Error.clear();
    m_AbsFileName.clear();
    QString fileName = absFileName;
    // replace path TAGs
    fileName.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
    fileName.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
    fileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));
    if (m_ReadableForms.keys().contains(fileName)) {
        return m_ReadableForms.value(fileName);
    }
    // Correct filename ?
    if (QFileInfo(fileName).isDir()) {
        // try to read central.xml
        fileName = fileName + "/central.xml";
    }
    if (!QFileInfo(fileName).exists()) {
        m_Error.append(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        return false;
    }
    if (QFileInfo(fileName).suffix().toLower()=="xml") {
        if (reader()->checkFormFileContent(fileName, Utils::readTextFile(fileName))) {  // use reader
            m_AbsFileName = fileName;
            m_ReadableForms.insert(fileName, true);
            m_ReadableForms.insert(absFileName, true);
            return true;
        } else {
            m_ReadableForms.insert(fileName, false);
            m_ReadableForms.insert(absFileName, false);
            return false;
        }
    } else {
        m_Error.append(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName));
    }
    m_ReadableForms.insert(fileName, false);
    m_ReadableForms.insert(absFileName, false);
    return false;
}

Form::FormIODescription *XmlFormIO::readFileInformations(const QString &uuidOrAbsPath)
{
    return reader()->readFileInformations(uuidOrAbsPath);
}

void XmlFormIO::getAllFormsFromDir(const QString &absPath, QList<Form::FormIODescription *> *list)
{
    QDir start(absPath);
    if (!start.exists()) {
        LOG_ERROR_FOR("XmlFormIO", tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(absPath) +
                  "\n" + tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg("File Form"));
        return;
    }
    // get all forms included in this path
    foreach(const QFileInfo &file, Utils::getFiles(start, "central.xml", true)) {
        const QString &fileName = file.absoluteFilePath();
        if (canReadForms(fileName))
            list->append(reader()->readFileInformations(fileName));
    }
}

QList<Form::FormIODescription *> XmlFormIO::getFormFileDescriptions(const Form::FormIOQuery &query)
{
    QList<Form::FormIODescription *> toReturn;
    QString startPath;
    if (query.typeOfForms() & Form::FormIOQuery::UserForms) {
        /** \todo manage user forms path and default path */
    } else {
        /** \todo manage user forms path and default path */
    }
    if (query.typeOfForms() & Form::FormIOQuery::CompleteForms) {
        startPath = settings()->path(Core::ISettings::CompleteFormsPath);
        getAllFormsFromDir(startPath, &toReturn);
        for(int i = 0; i < toReturn.count(); ++i) {
            toReturn.at(i)->setData(Form::FormIODescription::IsCompleteForm, true);
        }
    }
    if (query.typeOfForms() & Form::FormIOQuery::SubForms) {
        startPath = settings()->path(Core::ISettings::SubFormsPath);
        getAllFormsFromDir(startPath, &toReturn);
        for(int i = 0; i < toReturn.count(); ++i) {
            toReturn.at(i)->setData(Form::FormIODescription::IsSubForm, true);
        }
    }
    /** \todo Add IFormIO to descr && check all forms for params of Query */
    for(int i = 0; i < toReturn.count(); ++i) {
        toReturn.at(i)->setIoFormReader(this);
    }
    return toReturn;
}

QList<Form::FormMain *> XmlFormIO::loadAllRootForms(const QString &uuidOrAbsPath)
{
    /** \todo code here */
//    qWarning() << Q_FUNC_INFO << uuidOrAbsPath;
    QList<Form::FormMain *> toReturn;
    QString file = uuidOrAbsPath;
    if (uuidOrAbsPath.isEmpty()) {
        if (m_AbsFileName.isEmpty()) {
            LOG_ERROR(tr("No form file name"));
            return toReturn;
        } else {
            file = m_AbsFileName;
        }
    }
    if (!canReadForms(file))
        return toReturn;
    // replace path TAGs
    file.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));
    if (!file.endsWith("xml", Qt::CaseInsensitive))
        file += QDir::separator();
    // now we work with m_AbsFileName which is defined by canReadForm()
    QDir dir(QFileInfo(file).absolutePath());
//    qWarning() << "xxxxxxxxx" << dir.entryList(QStringList() << "*.xml", QDir::Files | QDir::Readable);
    reader()->refreshPluginFactories();
    foreach(const QFileInfo &file, dir.entryInfoList(QStringList() << "*.xml", QDir::Files | QDir::Readable)) {
        QString modeName = file.baseName();
        Form::FormMain *root = m_ActualForm = new Form::FormMain;
        root->setModeUniqueName(modeName);
        root->setUuid(uuidOrAbsPath);
        if (!reader()->isInCache(file.absoluteFilePath())) {
            if (!reader()->checkFormFileContent(file.absoluteFilePath(), Utils::readTextFile(file.absoluteFilePath())))
                continue;
        }
        if (!reader()->loadForm(file.filePath(), root)) {
            LOG_ERROR(m_Error.join("\n"));
        } else {
            toReturn.append(root);
        }
        // Emit the formLoaded signal for all subForms loaded
        QList<Form::FormMain *> forms = root->flattenFormMainChildren();
        for(int i=0; i < forms.count(); ++i) {
            forms.at(i)->emitFormLoaded();
        }
//        qWarning() << "     mode" << root << root->modeUniqueName();
    }
    return toReturn;
}

bool XmlFormIO::loadPmhCategories(const QString &uuidOrAbsPath)
{
    QString file = QFileInfo(uuidOrAbsPath).absolutePath() + "/pmhcategories.xml";
    // replace path TAGs
    file.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));

    QDomDocument *doc = 0;
    if (!reader()->isInCache(file)) {
        if (!canReadForms(file)) {
            LOG_ERROR("Unable to read Pmh Category file: " + file);
            return false;
        }
    }
    categoryCore()->removeAllExistingCategories("PMHx");
    doc = reader()->fromCache(file);
    Q_ASSERT(doc);
    if (!doc) {
        LOG_ERROR("No category document in XmlFormIO::loadPmhCategories("+file+")");
        return false;
    }
    QDomElement root = doc->firstChildElement(Constants::TAG_MAINXMLTAG);
    QDomElement element = root.firstChildElement(Constants::TAG_PMHX_CATEGORIES);
    element = element.firstChildElement(::Constants::TAG_CATEGORY);
    while (!element.isNull()) {
        createCategory(element, 0);
        element = element.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    pmhCore()->pmhCategoryModel()->refreshFromDatabase();
    return true;
}

bool XmlFormIO::createCategory(const QDomElement &element, Category::CategoryItem *parent)
{
    // create the category
    Category::CategoryItem *item = new Category::CategoryItem;
    item->setData(Category::CategoryItem::DbOnly_Mime, "PMHx");
    item->setData(Category::CategoryItem::ThemedIcon, element.attribute(::Constants::ATTRIB_ICON));

    // read the labels
    QDomElement label = element.firstChildElement(::Constants::TAG_SPEC_LABEL);
    while (!label.isNull()) {
        item->setLabel(label.text(), label.attribute(::Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
        label = label.nextSiblingElement(::Constants::TAG_SPEC_LABEL);
    }

    // get ExtraTag content -> CategoryItem::ExtraXml
    QDomElement extra = element.firstChildElement(::Constants::TAG_SPEC_EXTRA);
    if (!extra.isNull()) {
        item->setData(Category::CategoryItem::ExtraXml, extra.toDocument().toString(2));
    }

    // save to database
    if (parent) {
        parent->addChild(item);
        item->setParent(parent);
    }
    categoryCore()->saveCategory(item);

    // has children ?
    QDomElement child = element.firstChildElement(::Constants::TAG_CATEGORY);
    while (!child.isNull()) {
        createCategory(child, item);
        child = child.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    return true;
}

//bool XmlFormIO::loadElement(Form::FormItem *item, QDomElement &rootElement)
//{
//    bool descriptionPassed = false; // for speed improvements
//    QDomElement element = rootElement.firstChildElement();
//    while (!element.isNull()) {
//        int i=0;

//        // Do not proceed form description here
//        if (!descriptionPassed) {
//            if (element.tagName().compare(::Constants::TAG_FORM_DESCRIPTION)==0) {
//                descriptionPassed = true; // speed improvements
//                element = element.nextSiblingElement();
//                continue;
//            }
//        }

//        // Create a nem FormItem ?
//        i = Constants::createTags.indexOf(element.tagName());
//        if (i != -1) {
//            createElement(item, element);
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // if there is no item defined then go nextSibling till we find a new form, item, page...

//        if (!item) {
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // Values ?
//        if (element.tagName().compare(Constants::TAG_VALUE, Qt::CaseInsensitive)==0) {
//            populateValues(item, element);
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // Script ?
//        if (element.tagName().compare(Constants::TAG_SCRIPT, Qt::CaseInsensitive)==0) {
//            populateScripts(item,element);
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // Spec ?
//        i = ::m_SpecsTypes.value(element.tagName(), -1);
//        if (i != -1) {
//            QString lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
//            item->spec()->setValue(::m_SpecsTypes.value(element.tagName()), element.text(), lang);
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // Name ?
//        if (element.tagName().compare("name",Qt::CaseInsensitive)==0) {
//            item->setUuid(element.text());
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // Patient Data Representation ?
//        if (element.tagName().compare(Constants::TAG_DATAPATIENT, Qt::CaseInsensitive)==0) {
//            i = ::m_PatientDatas.value(element.text(), -1);
//            if (i != -1) {
//                item->setPatientDataRepresentation(i);
//            }
//            element = element.nextSiblingElement();
//            continue;
//        }

//    //             // optional?
////            if (element.hasAttribute(Constants::ATTRIB_OPTIONNAL)) {
////                if (element.attribute(Constants::ATTRIB_OPTIONNAL).compare("true",Qt::CaseInsensitive)==0)
////                    item->valueReferences()->setOptionnal(true);
////                else
////                    item->valueReferences()->setOptionnal(false);

//        // Add a file ?
//        if (element.tagName().compare(Constants::TAG_ADDFILE, Qt::CaseInsensitive)==0) {
//            QDomDocument doc;
//            QString fileName = element.text();
//            if (QFileInfo(fileName).isRelative())
//                fileName.prepend(QFileInfo(m_AbsFileName).absoluteDir().absolutePath() + QDir::separator());
//            fileName = QDir::cleanPath(fileName);
//            loadForm(fileName,0);
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // Options
//        if (element.tagName().compare(Constants::TAG_OPTIONS)==0) {
//            if (element.text().contains(Constants::TAG_OPTIONS_UNIQUE_EPISODE)) {
//                if (item==m_ActualForm)
//                    m_ActualForm->setEpisodePossibilities(Form::FormMain::UniqueEpisode);
//            } else if (element.text().contains(Constants::TAG_OPTIONS_NO_EPISODE)) {
//                if (item==m_ActualForm)
//                    m_ActualForm->setEpisodePossibilities(Form::FormMain::NoEpisode);
//            } else {
//                item->addExtraData(element.tagName(), element.text());
//            }
//            element = element.nextSiblingElement();
//            continue;
//        }

//        // All others add them in extraData
////        qWarning() << "XML adding other tag" << element.tagName() << element.text();
//        item->addExtraData(element.tagName(), element.text());

//        element = element.nextSiblingElement();
//    }
//    return true;
//}

//bool XmlFormIO::createElement(Form::FormItem *item, QDomElement &element)
//{
////    qWarning() << "XmlFormIO create element" << element.text();
//    // new item
//    if (element.tagName().compare(Constants::TAG_NEW_ITEM, Qt::CaseInsensitive)==0) {
//        if (item) {
//            loadElement(item->createChildItem(), element);
//            return true;
//        }
//        else
//            return false; // XML ERROR --> an item MUST have a parent
//    }

//    // new form
//    if (element.tagName().compare(Constants::TAG_NEW_FORM, Qt::CaseInsensitive)==0) {
//        // create a new form
//        Form::FormMain *oldRootForm = m_ActualForm;
//        /** \todo add Forms' parent */
//        m_ActualForm = m_ActualForm->createChildForm(element.firstChildElement(Constants::TAG_NAME).text());
//        item = m_ActualForm;
//        if (item) {
//            loadElement(item, element);
//            // read specific form's datas
//            m_ActualForm = oldRootForm;
//            return true;
//        }
//        else
//            return false;
//    }

//    if (element.tagName().compare(Constants::TAG_NEW_PAGE, Qt::CaseInsensitive)==0) {
//        // create a new page
//        item = item->createPage(element.firstChildElement(Constants::TAG_NAME).text());
//        /** \todo add page to a form */
//        if (item) {
//            loadElement(item, element);
//            // read specific page's datas
//            return true;
//        }
//        else
//            return false;
//    }

//    return false;
//}

//bool XmlFormIO::createItemWidget(Form::FormItem *item, QWidget *parent)
//{
//    // does plugin was inform in the xml file ?
//    if (item->spec()->pluginName().isEmpty()) {
//        qWarning() << "no plugin name for item" << item->uuid();
//        item->setFormWidget(0);
//        return false;
//    }

//    // does asked widget exists in the plugins ?
//    QString askedWidget = item->spec()->pluginName().toLower();
//    if (!m_PlugsFactories.keys().contains(askedWidget)) {
//        item->setFormWidget(0);
//        qWarning() << "asked widget does not exists in plugins' widgets' list" << askedWidget << m_PlugsFactories;
//        return false;
//    }

//    // get the widget
//    Form::IFormWidgetFactory *factory = m_PlugsFactories.value(askedWidget);
//    Form::IFormWidget *w = 0;
//    w = factory->createWidget(askedWidget,item);
//    if (w->isContainer()) {
//        foreach(Form::FormItem *child, item->formItemChildren()) {
////            Form::IFormWidget *wchild = factory->createWidget(child->spec()->pluginName(),child,w);
////            w->addWidgetToContainer(wchild);
////            child->setFormWidget(wchild);
//            createItemWidget(child,w);
//        }
//    }
//    item->setFormWidget(w);
//    Form::IFormWidget *p = qobject_cast<Form::IFormWidget*>(parent);
//    if (p)
//        p->addWidgetToContainer(w);
//    return true;
//}

//bool XmlFormIO::createFormWidget(Form::FormMain *form)
//{
//    // Create a new Widget with a vbox and put the label
////    QWidget *w = new QWidget();
//    if (!createItemWidget(form, 0))
//        return false;
//    return true;
//}

//bool XmlFormIO::createWidgets(const Form::FormMain *rootForm)
//{
//    // foreach FormMain children
//    foreach(Form::FormMain *form, rootForm->flattenFormMainChildren()) {
//        // create the form
//        createFormWidget(form);
//    }
//    return true;
//}

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

