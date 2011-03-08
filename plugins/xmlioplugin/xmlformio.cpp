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

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <translationutils/constanttranslations.h>

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
using namespace Trans::ConstantTranslations;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
    static QHash<QString, Form::IFormWidgetFactory *> m_PlugsFactories;
    static QHash<QString, int> m_ScriptsTypes;
    static QHash<QString, int> m_ValuesTypes;
    static QHash<QString, int> m_SpecsTypes;
    static QHash<QString, int> m_PatientDatas;
}

inline static Form::FormManager *formManager() { return Form::FormManager::instance(); }
inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

inline static void refreshPlugsFactories()
{
    ::m_PlugsFactories.clear();
    foreach(Form::IFormWidgetFactory *fact, pluginManager()->getObjects<Form::IFormWidgetFactory>()) {
        foreach(const QString &widgetname, fact->providedWidgets()) {
            ::m_PlugsFactories.insert(widgetname,fact);
        }
    }
}

inline static Form::FormMain *createNewForm(const QDomElement &element, Form::FormItem *item = 0)
{
    QString name = element.firstChildElement(Constants::TAG_NAME).text();
    Form::FormMain *parent = formManager()->getParent<Form::FormMain>(item);
    return formManager()->createForm(name, parent);
}

inline static bool populateValues(Form::FormItem *item, const QDomElement &root)
{
    QDomElement element = root.firstChildElement();


    QString lang = root.attribute(Constants::ATTRIB_LANGUAGE,Trans::Constants::ALL_LANGUAGE);
    while (!element.isNull()) {

//        qWarning() << "Values" << root.tagName() << element.tagName();

        int id = element.attribute(Constants::ATTRIB_ID, 0).toInt();
        QString val = element.text();
        int type = ::m_ValuesTypes.value(element.tagName(), -1);
        if (type != -1) {
            item->valueReferences()->setValue(type, id, val, lang);
        } else {
            if (element.tagName().compare(Constants::TAG_VALUE_DEFAULT, Qt::CaseInsensitive)==0) {
                item->valueReferences()->setDefaultValue(val, lang);
            }
        }
        element = element.nextSiblingElement();
    }
    return true;
}

inline static bool populateScripts(Form::FormItem *item, const QDomElement &root)
{
    QDomElement element = root.firstChildElement();
    QString lang = root.attribute(Constants::ATTRIB_LANGUAGE,Trans::Constants::ALL_LANGUAGE).left(2);
    while (!element.isNull()) {
        QString script = element.text();
        int type = ::m_ScriptsTypes.value(element.tagName(),Form::FormItemScripts::Script_OnDemand);
        item->scripts()->setScript(type,script,lang);
        element = element.nextSiblingElement();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  XmlFormIO  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
XmlFormIO::XmlFormIO(const QString &absFileName, QObject *parent) :
        IFormIO(absFileName, parent), m_AbsFileName(absFileName), m_Mute(false)
{
    setObjectName("XmlFormIO");

    ::m_ScriptsTypes.clear();
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONLOAD, Form::FormItemScripts::Script_OnLoad);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_POSTLOAD, Form::FormItemScripts::Script_PostLoad);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEMAND, Form::FormItemScripts::Script_OnDemand);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUECHANGED, Form::FormItemScripts::Script_OnValueChanged);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUEREQUIERED, Form::FormItemScripts::Script_OnValueRequiered);
    ::m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEPENDENCIESCHANGED, Form::FormItemScripts::Script_OnDependentValueChanged);

    ::m_ValuesTypes.clear();
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_UUID, Form::FormItemValues::Value_Uuid);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_NUMERICAL, Form::FormItemValues::Value_Numerical);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_SCRIPT, Form::FormItemValues::Value_Script);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_POSSIBLE, Form::FormItemValues::Value_Possible);
    ::m_ValuesTypes.insert(Constants::TAG_VALUE_DEPENDENCIES, Form::FormItemValues::Value_Dependency);

    ::m_SpecsTypes.clear();
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_PLUGINNAME, Form::FormItemSpec::Spec_Plugin);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_AUTHORS, Form::FormItemSpec::Spec_Author);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_CATEGORY, Form::FormItemSpec::Spec_Category);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_LICENSE, Form::FormItemSpec::Spec_License);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_CREATIONDATE, Form::FormItemSpec::Spec_CreationDate);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_BIBLIOGRAPHY, Form::FormItemSpec::Spec_Bibliography);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_DESCRIPTION, Form::FormItemSpec::Spec_Description);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_LABEL, Form::FormItemSpec::Spec_Label);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_VERSION, Form::FormItemSpec::Spec_Version);
    ::m_SpecsTypes.insert(Constants::TAG_SPEC_ICON, Form::FormItemSpec::Spec_IconFileName);

    ::m_PatientDatas.clear();
    ::m_PatientDatas.insert(Constants::TAG_DATAPATIENT_DRUGSALLERGIES, Core::IPatient::DrugsInnAllergies);
    ::m_PatientDatas.insert(Constants::TAG_DATAPATIENT_DRUGSCHRONIC, Core::IPatient::DrugsChronicTherapeutics);
    ::m_PatientDatas.insert(Constants::TAG_DATAPATIENT_WEIGHT, Core::IPatient::Weight);
    ::m_PatientDatas.insert(Constants::TAG_DATAPATIENT_HEIGHT, Core::IPatient::Height);
}

XmlFormIO::~XmlFormIO()
{
}

QString XmlFormIO::lastestXmlVersion()
{
    return "0.3.0";
}

QStringList XmlFormIO::fileFilters() const
{
    return QStringList() << tr("FreeMedForms Form File (*.%1)").arg(Constants::DOCTYPE_EXTENSION);
}

void XmlFormIO::warnXmlReadError(bool muteUserWarnings, const QString &file, const QString &msg, const int line, const int col) const
{
    QString m = Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file) + " ; " +
                  Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                  .arg(msg).arg(line).arg(col);
    Utils::Log::addError(this, m);
    m_Error.append(Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                   .arg(msg).arg(line).arg(col));
//    m_Error.append(m_MainDoc);

    if (!muteUserWarnings)
        Utils::warningMessageBox(
            Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file),
            Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
            .arg(msg).arg(line).arg(col),"",qApp->applicationName());
}

bool XmlFormIO::setFileName(const QString &absFileName)
{
    m_Author.clear();
    m_Version.clear();
    m_Desc.clear();
    m_AbsFileName.clear();
    m_Error.clear();
    m_MainDoc.clear();

    if (!QFile(absFileName).exists()) {
        m_Error.append(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(absFileName));
        return false;
    }

    if (QFileInfo(absFileName).suffix()==managedFileExtension()) {
        m_AbsFileName = absFileName;
        return true;
    } else {
        m_Error.append(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(absFileName));
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
    bool ok = true;
    QString contents = Utils::readTextFile(m_AbsFileName, Utils::DontWarnUser);
    if (contents.isEmpty()) {
        /** \todo return a FormObject with a helptext that explains the error ? */
        warnXmlReadError(m_Mute, m_AbsFileName, tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(m_AbsFileName));
        m_Error.append(tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(m_AbsFileName));
        return false;
    }
    if (contents.count("<"+QString(Constants::TAG_NEW_FORM)+">") != contents.count("</"+QString(Constants::TAG_NEW_FORM)+">")) {
        ok = false;
        m_Error.append(tr("Wrong number of tags (%1)").arg(Constants::TAG_NEW_FORM));
        Utils::Log::addError(this, Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(m_AbsFileName));
    }
    if ((contents.count(QString("<%1>").arg(Constants::TAG_MAINXMLTAG)) != 1) ||
        (contents.count(QString("</%1>").arg(Constants::TAG_MAINXMLTAG)) != 1)) {
        m_Error.append(tr("Wrong number of tags (%1)").arg(Constants::TAG_NEW_FORM));
        ok = false;
    }

    // load document
    QString errorMsg;
    int errorLine, errorColumn;
    if (!m_MainDoc.setContent(contents, &errorMsg, &errorLine, &errorColumn)) {
        warnXmlReadError(m_Mute, m_AbsFileName, errorMsg, errorLine, errorColumn);
        ok = false;
    }

    // Check doctype name
    if (m_MainDoc.doctype().name().compare(Constants::DOCTYPE_NAME,Qt::CaseInsensitive)!=0) {
        const QString &error = tr("This file is not a FreeMedForms XML file. Document type name mismatch.");
        warnXmlReadError(m_Mute, m_AbsFileName, error);
        ok = false;
    }

    /** \todo check version of the file */
//    if (!contents.contains(QString("<%1>").arg(Constants::TAG_SPEC_VERSION), Qt::CaseInsensitive)) {
//        const QString &error = tr("No version number defined");
//        warnXmlReadError(m_Mute, file, error);
//        return false;
//    } else {
//        int beg = contents.indexOf(QString("<%1>").arg(Constants::TAG_SPEC_VERSION)) + QString("<%1>").arg(Constants::TAG_SPEC_VERSION).length();
//        int end = contents.indexOf(QString("</%1>").arg(Constants::TAG_SPEC_VERSION));
//        QString version = contents.mid(beg, end-beg).simplified();
//    }
    return ok;
}

bool XmlFormIO::readFileInformations()
{
    QDomElement root = m_MainDoc.documentElement();
    // get version
    m_Version = root.firstChildElement(Constants::TAG_SPEC_VERSION).text();
    // get author
    m_Author = root.firstChildElement(Constants::TAG_SPEC_AUTHORS).text();
    // get descriptions
    QDomElement desc = root.firstChildElement(Constants::TAG_SPEC_DESCRIPTION);
    while (!desc.isNull()) {
        m_Desc.insert(desc.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE), desc.text());
        desc = root.nextSiblingElement(Constants::TAG_SPEC_DESCRIPTION);
    }
    return true;
}

QString XmlFormIO::formDescription(const QString &lang) const
{
    if (m_Desc.value(lang).isEmpty())
        return m_Desc.value(Trans::Constants::ALL_LANGUAGE);
    return m_Desc.value(lang);
}

void XmlFormIO::formDescriptionToTreeWidget(QTreeWidget *tree, const QString &lang) const
{
    tree->clear();
    tree->setColumnCount(2);
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *authors = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::AUTHOR));
    authors->setFont(0, bold);
    new QTreeWidgetItem(authors, QStringList() << tkTr(Trans::Constants::AUTHOR) << m_Author);
    QTreeWidgetItem *version = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::VERSION));
    version->setFont(0, bold);
    new QTreeWidgetItem(version, QStringList() << tkTr(Trans::Constants::VERSION) << m_Version);
    QTreeWidgetItem *desc = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::DESCRIPTION));
    desc->setFont(0, bold);
    QHashIterator<QString, QString> i(m_Desc);
     while (i.hasNext()) {
         i.next();
         new QTreeWidgetItem(desc, QStringList() << i.key() << i.value());
     }
    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

bool XmlFormIO::loadForm()
{
    Q_ASSERT(!m_AbsFileName.isEmpty());
    refreshPlugsFactories();
    m_ActualForm = 0;
    return loadForm(m_AbsFileName, m_ActualForm);
}

bool XmlFormIO::loadForm(const QString &file, Form::FormMain *rootForm)
{
    // Check root element --> must be a form type
    QDomElement root = m_MainDoc.firstChildElement(Constants::TAG_MAINXMLTAG);
    QDomElement newForm = root.firstChildElement(Constants::TAG_NEW_FORM);
    QDomElement addFile = root.firstChildElement(Constants::TAG_ADDFILE);

    // in case of no rootForm is passed --> XML must start with a file inclusion or a newform tag
    if (!rootForm) {
        if (addFile.isNull() && newForm.isNull()) {
            warnXmlReadError(m_Mute, file, tr("Wrong root tag %1 %2.").arg(root.tagName()).arg(Constants::TAG_NEW_FORM));
            return false;
        }
//        rootForm = createNewForm(newForm, m_ActualForm);
//        m_ActualForm = rootForm;
    }

    if (!loadElement(rootForm, root))
        return false;

//    rootForm->createDebugPage();
    createWidgets();
    return true;
}

bool XmlFormIO::loadElement(Form::FormItem *item, QDomElement &rootElement)
{
    QDomElement element = rootElement.firstChildElement();
    while (!element.isNull()) {
        int i=0;

        // Create a nem FormItem ?
        i = Constants::createTags.indexOf(element.tagName());
        if (i != -1) {
            createElement(item, element);
            element = element.nextSiblingElement();
            continue;
        }

        // if there is no item defined then go nextSibling till we find a new form, item, page...
        if (!item) {
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
        i = ::m_SpecsTypes.value(element.tagName(), -1);
        if (i != -1) {
            QString lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
            item->spec()->setValue(::m_SpecsTypes.value(element.tagName()), element.text(), lang);
            element = element.nextSiblingElement();
            continue;
        }

        // Name ?
        if (element.tagName() == "name") {
            item->setUuid(element.text());
        }

        // Patient Data Representation ?
        if (element.tagName().compare(Constants::TAG_DATAPATIENT,Qt::CaseInsensitive)==0) {
            i = ::m_PatientDatas.value(element.text(), -1);
            if (i != -1) {
                item->setPatientDataRepresentation(i);
            }
        }

    //             // optional?
//            if (element.hasAttribute(Constants::ATTRIB_OPTIONNAL)) {
//                if (element.attribute(Constants::ATTRIB_OPTIONNAL).compare("true",Qt::CaseInsensitive)==0)
//                    item->valueReferences()->setOptionnal(true);
//                else
//                    item->valueReferences()->setOptionnal(false);

        // Add a file ?
        if (element.tagName().compare(Constants::TAG_ADDFILE, Qt::CaseInsensitive)==0) {
            QDomDocument doc;
            QString fileName = element.text();
            if (QFileInfo(fileName).isRelative())
                fileName.prepend(QFileInfo(m_AbsFileName).absoluteDir().absolutePath() + QDir::separator());
            fileName = QDir::cleanPath(fileName);
            loadForm(fileName,0);
        }

        // Options
        if (element.tagName().compare(Constants::TAG_OPTIONS)==0) {
            if (element.text().contains(Constants::TAG_OPTIONS_UNIQUE_EPISODE)) {
                if (item==m_ActualForm)
                    m_ActualForm->setEpisodePossibilities(Form::FormMain::UniqueEpisode);
            } else if (element.text().contains(Constants::TAG_OPTIONS_NO_EPISODE)) {
                if (item==m_ActualForm)
                    m_ActualForm->setEpisodePossibilities(Form::FormMain::NoEpisode);
            } else {
                item->addExtraData(element.tagName(), element.text());
            }
        }

        element = element.nextSiblingElement();
    }
    return true;
}

bool XmlFormIO::createElement(Form::FormItem *item, QDomElement &element)
{
//    qWarning() << "XmlFormIO create element" << element.text();
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
        Form::FormMain *oldRootForm = m_ActualForm;
        /** \todo add Forms' parent */
        m_ActualForm = formManager()->createForm(element.firstChildElement(Constants::TAG_NAME).text(), m_ActualForm);
        item = m_ActualForm;
        if (item) {
            loadElement(item, element);
            // read specific form's datas
            m_ActualForm = oldRootForm;
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

bool XmlFormIO::createItemWidget(Form::FormItem *item, QWidget *parent)
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
    Form::IFormWidgetFactory *factory = m_PlugsFactories.value(askedWidget);
    Form::IFormWidget *w = 0;
    w = factory->createWidget(askedWidget,item);
    if (w->isContainer()) {
        foreach(Form::FormItem *child, item->formItemChildren()) {
//            Form::IFormWidget *wchild = factory->createWidget(child->spec()->pluginName(),child,w);
//            w->addWidgetToContainer(wchild);
//            child->setFormWidget(wchild);
            createItemWidget(child,w);
        }
    }
    item->setFormWidget(w);
    Form::IFormWidget *p = qobject_cast<Form::IFormWidget*>(parent);
    if (p)
        p->addWidgetToContainer(w);
    return true;
}

bool XmlFormIO::createFormWidget(Form::FormMain *form)
{
    // Create a new Widget with a vbox and put the label
//    QWidget *w = new QWidget();
    if (!createItemWidget(form, 0))
        return false;
    return true;
}

bool XmlFormIO::createWidgets()
{
    // foreach Forms in FormManager
    foreach(Form::FormMain *form, formManager()->forms()) {
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

