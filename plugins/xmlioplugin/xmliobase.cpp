/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "xmliobase.h"
#include "constants.h"
#include "xmlformcontentreader.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/icommandline.h>

#include <formmanagerplugin/iformio.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorycore.h>

#include <printerplugin/textdocumentextra.h>

#include <pmhplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDateTime>

using namespace XmlForms;
using namespace Internal;
using namespace XmlForms::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline XmlFormContentReader *reader() {return XmlFormContentReader::instance();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline Category::CategoryCore *categoryCore() {return  Category::CategoryCore::instance();}

static inline QString normalizedFormUid(const QString &formUid)
{
    QString newUid = formUid;
    newUid.replace("\\", "/");
    if (newUid.endsWith(".xml")) {
        newUid = newUid.left(newUid.lastIndexOf("/"));
    }
    return newUid;
}

// Initializing static datas
bool XmlIOBase::m_initialized = false;
XmlIOBase *XmlIOBase::m_Instance = 0;

/**
  \brief Returns the unique instance of XmlIOBase. If the instance does not exist it is created.
  You should never construct a instance of this object using the constructor.
*/
XmlIOBase *XmlIOBase::instance()
{
    if (!m_Instance)
        m_Instance = new XmlIOBase(qApp);
    return m_Instance;
}

XmlIOBase::XmlIOBase(QObject *parent) :
        QObject(parent), Utils::Database()
{
    setObjectName("XmlIOBase");
    m_initialized = false;

    // populate tables and fields of database
    addTable(Table_FORMS,         "FORMS");
    addTable(Table_FORM_CONTENT,  "CONTENT");
    addTable(Table_VERSION,       "VERSION");

    addField(Table_FORMS, FORM_ID,           "FORM_ID",        FieldIsUniquePrimaryKey);
    addField(Table_FORMS, FORM_UUID,         "FORM_UUID",      FieldIsUUID);
    addField(Table_FORMS, FORM_ORIGINALUID,  "FORM_ORG_UUID",  FieldIsShortText);
    addField(Table_FORMS, FORM_ORIGINALDATE, "FORM_ORG_DATE",  FieldIsDate);
    addIndex(Table_FORMS, FORM_UUID);
    addIndex(Table_FORMS, FORM_ORIGINALUID);

    addField(Table_FORM_CONTENT, FORMCONTENT_ID,        "CONTENT_ID",     FieldIsUniquePrimaryKey);
    addField(Table_FORM_CONTENT, FORMCONTENT_FORM_ID,   "FORM_ID",        FieldIsInteger);
    addField(Table_FORM_CONTENT, FORMCONTENT_TYPE,      "TYPE",           FieldIsShortText);
    addField(Table_FORM_CONTENT, FORMCONTENT_MODENAME,  "MODE",           FieldIsShortText);
    addField(Table_FORM_CONTENT, FORMCONTENT_ISVALID,   "ISVALID",        FieldIsBoolean);
    addField(Table_FORM_CONTENT, FORMCONTENT_ISORIGINAL,"ISORIGINAL",     FieldIsBoolean);
    addField(Table_FORM_CONTENT, FORMCONTENT_CONTENT,   "CONTENT",        FieldIsBlob);
    addIndex(Table_FORM_CONTENT, FORMCONTENT_ID);
    addIndex(Table_FORM_CONTENT, FORMCONTENT_FORM_ID);

    // information
    addTable(Table_VERSION, "VERSION");
    addField(Table_VERSION, VERSION_ACTUAL,  "ACTUAL", FieldIsShortText);

    initialize();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

/** \brief Initialize the database. */
bool XmlIOBase::initialize()
{
    if (m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    if (!checkDatabaseVersion())
        return false;

    m_initialized = true;
    return true;
}

/** Return true if the XmlIOBase is initialized and usable */
bool XmlIOBase::isInitialized() const
{
    return m_initialized;
}

/** Return true if the XmlIOBase is the last version (database is updated by this member if needed) */
bool XmlIOBase::checkDatabaseVersion()
{
    /** \todo Code : XmlIOBase::checkDatabaseVersion() */
    return true;
}

/** Create the default users database if it does not exist. */
bool XmlIOBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString &login, const QString &pass,
                    const int port,
                    CreationOption createOption
                   )
{
    Q_UNUSED(access);
    Q_UNUSED(createOption);

    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(QCoreApplication::translate("XmlIOBase",
                                    "Trying to create empty database.\n"
                                    "       Location: %1\n"
                                    "       FileName: %2\n"
                                    "       Driver: %3")
        .arg(pathOrHostName, dbName).arg(driver));

    setConnectionName(connectionName);
    setDriver(driver);

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    } else if (driver == MySQL) {
        /** \todo test grants here or before ? */
        if (QSqlDatabase::connectionNames().contains(connectionName)) {
            DB = QSqlDatabase::database(connectionName);
        } else {
            DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
            DB.setHostName(pathOrHostName);
            DB.setUserName(login);
            DB.setPassword(pass);
            DB.setPort(port);
        }
        DB.setDatabaseName("mysql");
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
        //        qWarning() << "createMySQLDatabase(dbName);";
        createMySQLDatabase(dbName);
        // change database connection
        DB.setDatabaseName(dbName);
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    // create db structure
    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }

    // Table INFORMATION
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_ACTUAL, Constants::DB_VERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // database is readable/writable
    LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + dbName));

    return true;
}

void XmlIOBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    initialize();
}

/**
  Return true if the form \e formUid, XmlIOBase::TypeOfForm \e type and \e modeName exists in the database.
  Empty \e modeName are interpreted as the central form.
*/
bool XmlIOBase::isFormExists(const QString &formUid, const int type, const QString &modeName)
{
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(DB.lastError().text()));
            return false;
        }
    }
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORMS, Constants::FORM_ID);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("=%1").arg(type));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    if (modeName.isEmpty()) {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("='central'"));
    } else {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("='%1'").arg(modeName));
    }
    QString req = select(gets, joins, conds);
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            return true;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return false;
}

/** Get all forms descriptions from the database according to the \e query */
QList<Form::FormIODescription *> XmlIOBase::getFormDescription(const Form::FormIOQuery &formQuery)
{
    QList<Form::FormIODescription *> toReturn;
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(DB.lastError().text()));
            return toReturn;
        }
    }
    // Just manage type of forms
    QSqlQuery query(DB);

    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    if (formQuery.getAllAvailableFormDescriptions()) {
        conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("like '%'"));
    } else {
        if (formQuery.typeOfForms() & Form::FormIOQuery::CompleteForms) {
            conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("like '%1%'").arg(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH));
        }
        if (formQuery.typeOfForms() & Form::FormIOQuery::SubForms) {
            conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("like '%1%'").arg(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH));
        }
    }
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("=%1").arg(Description));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));

    QString req = select(gets, joins, conds);
    if (query.exec(req)) {
        while (query.next()) {
            QDomDocument doc;
            doc.setContent(query.value(1).toString());
            Form::FormIODescription *descr = reader()->readXmlDescription(doc.firstChildElement(Constants::TAG_FORM_DESCRIPTION), query.value(0).toString());
            if (descr) {
                descr->setData(Form::FormIODescription::Category, descr->data(Form::FormIODescription::Category).toString(), QLocale().name().left(2));
                descr->setData(Form::FormIODescription::FromDatabase, true);

                if (formQuery.getScreenShots()) {
                    QHash<QString, QPixmap> pix = getScreenShots(query.value(0).toString(), QLocale().name().left(2).toLower());
                    foreach(const QString &k, pix.keys()) {
                        descr->addScreenShot(k, pix.value(k));
                    }
                }
                toReturn << descr;
            }
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    return toReturn;
}

/** Return a QHash with as key the XmlIOBase::TypeOfForm and as value the XML content of the form */
QHash<QString, QString> XmlIOBase::getAllFormFullContent(const QString &formUid)
{
    QHash<QString, QString> toReturn;
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(DB.lastError().text()));
            return toReturn;
        }
    }
    // Get all formIds from Forms table
    QVector<int> ids;
    QSqlQuery query(DB);
    QHash<int, QString> where;
    where.insert(Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    QString req = select(Table_FORMS, FORM_ID, where);

    if (query.exec(req)) {
        while (query.next()) {
            ids.append(query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return toReturn;
    }
    query.finish();

    // Retreive all contents
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);

    for(int i = 0; i < ids.count(); ++i) {
        int id = ids.at(i);
        Utils::FieldList conds;
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("=%1").arg(FullContent));
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID, QString("=%1").arg(id));
        req = select(gets, joins, conds);

        if (query.exec(req)) {
            while (query.next()) {
                toReturn.insert(query.value(0).toString(), query.value(1).toString());
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
        query.finish();
    }
    return toReturn;
}

/** Return the XML content of the form according to the XmlIOBase::TypeOfForm \e type and the \e formUid */
QString XmlIOBase::getFormContent(const QString &formUid, const int type, const QString &modeName)
{
//    qWarning() << Q_FUNC_INFO;
    /** \todo manage modes */
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(DB.lastError().text()));
            return QString();
        }
    }
    QSqlQuery query(DB);
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("=%1").arg(type));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    if (modeName.isEmpty()) {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("='central'"));
    } else {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("='%1'").arg(modeName));
    }
    QString req = select(gets, joins, conds);

    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QString();
}

/** Returns the shot \e shotName associated to the form \e formUid */
QPixmap XmlIOBase::getScreenShot(const QString &formUid, const QString &shotName)
{
    // Get shot content
    QString content = getFormContent(formUid, ScreenShot, shotName);
    if (content.isEmpty())
        return QPixmap();
    // Create pixmap
    QPixmap pix;
    pix.loadFromData(QByteArray::fromBase64(content.toAscii()));
    return pix;
}

/** Returns the shots for the language \e lang associated to the form \e formUid */
QHash<QString, QPixmap> XmlIOBase::getScreenShots(const QString &formUid, const QString &lang)
{
    QHash<QString, QPixmap> pixmaps;
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(DB.lastError().text()));
            return pixmaps;
        }
    }
    QSqlQuery query(DB);
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("=%1").arg(ScreenShot));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE '%1/%'").arg(lang));
    QString req = select(gets, joins, conds);

    if (query.exec(req)) {
        while (query.next()) {
            QPixmap pix;
            pix.loadFromData(QByteArray::fromBase64(query.value(1).toByteArray()));
            pixmaps.insert(query.value(0).toString() ,pix);
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return pixmaps;
}

/** Save the \e content of the form \e form to the database and return the used formUid. If the \e content is empty the form file is accessed */
bool XmlIOBase::saveForm(const XmlFormName &form)
{
//    qWarning() << Q_FUNC_INFO << form;
    LOG("Saving forms to database: " + form.uid);

    QDir dir(form.absPath);

    database().transaction();

    // save all XML files && files included in the form using the <file> XML tag
    foreach(const QFileInfo &f, dir.entryInfoList(QStringList() << "*.xml", QDir::Files | QDir::Readable)) {
        // save the content of the form file
        QString modeName = f.baseName();
        QString content = Utils::readTextFile(f.absoluteFilePath(), Utils::DontWarnUser);
        if (f.fileName().compare(Constants::PMHXCATEGORIES_FILENAME, Qt::CaseInsensitive)==0) {
            if (!saveContent(form.uid, content, XmlIOBase::PmhCategories, modeName))
                LOG_ERROR("Can not save pmhx categories associated with the form to database");
            else
                LOG("Saving pmhx categories file to database " + f.absoluteFilePath());
            savePmhxCategories(form, content);
        } else {
            if (!saveContent(form.uid, content, XmlIOBase::FullContent, modeName))
                LOG_ERROR("Can not save form to database");
            else
                LOG("Saving attached *xml files to database " + f.absoluteFilePath());
        }

        // Try to catch file addition
        // File addition is done using the tag ‘file’ or an attrib of the same name
        QDomDocument doc;
        if (!doc.setContent(content)) {
            LOG_ERROR("XML Error");
            continue;
        }
        QDomNodeList list = doc.elementsByTagName("file");

        for(int i=0; i < list.count(); ++i) {
            const QDomNode &node = list.at(i);
            const QString &include = node.toElement().text();
            if (include.endsWith(".xml", Qt::CaseInsensitive)) {
                XmlFormName includeForm(include);
                if (!saveForm(includeForm))
                    LOG_ERROR("unable to save included form: " + includeForm.uid);
            }
        }
    }

    // Save scripts
    saveFiles(form, "scripts", "js", XmlIOBase::ScriptFile);
    // Save uis
    saveFiles(form, "ui", "ui", XmlIOBase::UiFile);
    // Save qml
//    saveFiles(form, "qml", "qml", XmlIOBase::QmlFile);
    // Save html
    saveFiles(form, "html", "html", XmlIOBase::HtmlFile);
    // Save screenshots
    saveScreenShots(form);

    database().commit();
    return true;
}

Category::CategoryItem * XmlIOBase::createCategory(const XmlFormName &form, const QDomElement &element, Category::CategoryItem *parent) const
{
    // create the category
    Category::CategoryItem *item = new Category::CategoryItem;
    item->setData(Category::CategoryItem::DbOnly_Mime, QString("%1@%2").arg(PMH::Constants::CATEGORY_MIME).arg(form.uid));
    item->setData(Category::CategoryItem::DbOnly_IsValid, 1);
    item->setData(Category::CategoryItem::ThemedIcon, element.attribute(::Constants::ATTRIB_ICON));
    item->setData(Category::CategoryItem::Uuid, element.attribute(::Constants::ATTRIB_UUID));
    item->setData(Category::CategoryItem::SortId, element.attribute(::Constants::ATTRIB_SORT_ID));

    // read the labels
    QDomElement label = element.firstChildElement(::Constants::TAG_SPEC_LABEL);
    while (!label.isNull()) {
        item->setLabel(label.text(), label.attribute(::Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
        label = label.nextSiblingElement(::Constants::TAG_SPEC_LABEL);
    }

    // get ExtraTag content -> CategoryItem::ExtraXml
    QDomElement extra = element.firstChildElement(::Constants::TAG_SPEC_EXTRA);
    if (!extra.isNull()) {
        QString content;
        QTextStream s(&content);
        extra.save(s, 2);
        item->setData(Category::CategoryItem::ExtraXml, content);
    }

    // reparent item
    if (parent) {
        parent->addChild(item);
        item->setParent(parent);
    }
    // has children ?
    QDomElement child = element.firstChildElement(::Constants::TAG_CATEGORY);
    while (!child.isNull()) {
        createCategory(form, child, item);
        child = child.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    return item;
}

/** Save PMHx categories to the categories database. The pmhx category file must be already in the database. */
void XmlIOBase::savePmhxCategories(const XmlFormName &form, const QString &content)
{
    Q_UNUSED(form);
    if (content.isEmpty()) {
        LOG_ERROR("Empty content.");
        return;
    }
    // analyze XML content
    QDomDocument doc;
    int line = -1;
    int col = -1;
    QString error;
    if (!doc.setContent(content, &error, &line, &col)) {
        LOG_ERROR(QString("Error while oading PMHxCategories XML files.\n  %1: %2;%3").arg(error).arg(line).arg(col));
        return;
    }
    QDomElement root = doc.firstChildElement(Constants::TAG_MAINXMLTAG);
    QDomElement element = root.firstChildElement(Constants::TAG_PMHX_CATEGORIES);
    element = element.firstChildElement(::Constants::TAG_CATEGORY);
    QVector<Category::CategoryItem *> rootCategories;
    while (!element.isNull()) {
        rootCategories << createCategory(form, element, 0);
        element = element.nextSiblingElement(::Constants::TAG_CATEGORY);
    }

    // save categories in the categories plugin
    if (!categoryCore()->saveCategories(rootCategories))
        LOG_ERROR(tr("Error while saving PMHxCateogries (%1)").arg(form.uid));
}

/** Save screenshots files associated with the forms. */
void XmlIOBase::saveScreenShots(const XmlFormName &form)
{
    QDir shotPath(form.absPath + QDir::separator() + "shots");
    if (shotPath.exists()) {
        LOG_FOR("XmlFormIO","Saving attached screenshots to database " + form.uid);
        QFileInfoList files = Utils::getFiles(shotPath, "*.png", Utils::Recursively);
        foreach(const QFileInfo &f, files) {
            QString fp = f.absoluteFilePath();
            QFile file(fp);
            // mode = last dir (lang) + fileName.extension
            int end = fp.lastIndexOf("/");
            int begin = fp.lastIndexOf("/", end - 1) + 1;
            QString mode = fp.mid(begin, end-begin) + "/" + f.fileName();
            if (file.open(QFile::ReadOnly)) {
                QByteArray ba = file.readAll();
                saveContent(form.uid, ba.toBase64(), XmlIOBase::ScreenShot, mode);
            }
        }
    }
}

/** Save files associated with the forms. */
void XmlIOBase::saveFiles(const XmlFormName &form, const QString &subDir, const QString &fileExtension, XmlIOBase::TypeOfContent type)
{
//    WARN_FUNC << absPathDir << form.uid;
    QDir path(form.absPath + QDir::separator() + subDir);
    if (!path.exists())
        path.cdUp();
    if (path.exists()) {
        LOG_FOR("XmlFormIO","Saving attached *." + fileExtension + " files to database " + form.uid);
        QFileInfoList files = Utils::getFiles(path, "*." + fileExtension, Utils::Recursively);
        foreach(const QFileInfo &f, files) {
            QString fp = f.absoluteFilePath();
            QString mode = fp;
            mode = "." + mode.remove(form.absPath);
            saveContent(form.uid, Utils::readTextFile(fp, Utils::DontWarnUser), type, mode);
        }
    }
}

/** Save the content of an XML form content \e xmlContent into the database using the Form Uuid \e formUid, XmlIOBase::TypeOfForm \e type, Mode name \e modeName and \e date */
bool XmlIOBase::saveContent(const QString &formUid, const QString &xmlContent, const int type, const QString &modeName, const QDateTime &date)
{
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(DB.lastError().text()));
        }
    }

    QSqlQuery query(DB);
    int formId = -1;
    QString req;
    QString mode = modeName;
    if (mode.isEmpty()) {
        mode = "central";
    }
    mode.remove(".xml");

    QHash<int, QString> where;
    // create in form table ?
    where.insert(FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    int nb = count(Table_FORMS, FORM_ID, getWhereClause(Table_FORMS, where));
    if (!nb) {
        // Insert and retreive FormId
        req = prepareInsertQuery(Table_FORMS);
        query.prepare(req);
        query.bindValue(FORM_ID, QVariant());
        query.bindValue(FORM_UUID, normalizedFormUid(formUid));
        query.bindValue(FORM_ORIGINALUID, formUid);
        query.bindValue(FORM_ORIGINALDATE, date);
        if (query.exec()) {
            formId = query.lastInsertId().toInt();
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    } else {
        // Get FormId
        req = select(Table_FORMS, FORM_ID, where);
        if (query.exec(req)) {
            if (query.next()) {
                formId = query.value(0).toInt();
            }
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }

    // update or insert content table ?
    where.clear();
    where.insert(FORMCONTENT_FORM_ID, QString("=%1").arg(formId));
    where.insert(FORMCONTENT_TYPE, QString("=%1").arg(type));
    where.insert(FORMCONTENT_MODENAME, QString("='%1'").arg(mode));
    where.insert(FORMCONTENT_CONTENT, "IS NOT NULL");
    nb = count(Table_FORM_CONTENT, FORMCONTENT_FORM_ID, getWhereClause(Table_FORM_CONTENT, where));
    if (!nb) {
        // Insert
        req = prepareInsertQuery(Table_FORM_CONTENT);
        query.prepare(req);
        query.bindValue(FORMCONTENT_ID, QVariant());
        query.bindValue(FORMCONTENT_FORM_ID, formId);
        query.bindValue(FORMCONTENT_TYPE, type);
        query.bindValue(FORMCONTENT_MODENAME, mode);
        query.bindValue(FORMCONTENT_ISVALID, 1);
        query.bindValue(FORMCONTENT_ISORIGINAL, 1);
        query.bindValue(FORMCONTENT_CONTENT, xmlContent);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();

        // add the description
        if (mode.compare("central",Qt::CaseInsensitive)==0) {
            int beg = xmlContent.indexOf(QString("<%1>").arg(Constants::TAG_FORM_DESCRIPTION));
            int end = xmlContent.indexOf(QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION), beg);
            if (beg==-1 || end==-1) {
                LOG_ERROR("no description found");
                return false;
            }
            end += QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION).length();
            QString descr = xmlContent.mid(beg, end-beg),
            req = prepareInsertQuery(Table_FORM_CONTENT);
            query.prepare(req);
            query.bindValue(FORMCONTENT_ID, QVariant());
            query.bindValue(FORMCONTENT_FORM_ID, formId);
            query.bindValue(FORMCONTENT_TYPE, Description);
            query.bindValue(FORMCONTENT_MODENAME, mode);
            query.bindValue(FORMCONTENT_ISVALID, 1);
            query.bindValue(FORMCONTENT_ISORIGINAL, 1);
            query.bindValue(FORMCONTENT_CONTENT, descr);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }

    } else {
        // Update
        req = prepareUpdateQuery(Table_FORM_CONTENT, FORMCONTENT_CONTENT, where);
        query.prepare(req);
        query.bindValue(0, xmlContent);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();

        // update the description
        if (mode.compare("central",Qt::CaseInsensitive)==0) {
            int beg = xmlContent.indexOf(QString("<%1>").arg(Constants::TAG_FORM_DESCRIPTION));
            int end = xmlContent.indexOf(QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION), beg);
            if (beg==-1 || end==-1) {
                LOG_ERROR("no description found");
                return false;
            }
            end += QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION).length();
            QString descr = xmlContent.mid(beg, end-beg);

            where.insert(FORMCONTENT_TYPE, QString("=%1").arg(Description));
            req = prepareUpdateQuery(Table_FORM_CONTENT, FORMCONTENT_CONTENT, where);
            query.prepare(req);
            query.bindValue(0, descr);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }

    }
    return true;
}

