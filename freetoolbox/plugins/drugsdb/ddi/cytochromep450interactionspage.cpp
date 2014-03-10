#include "cytochromep450interactionspage.h"
#include "ui_cytochromep450interactionspage.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QFile>
#include <QDir>
#include <QUrl>

// TABLE
// http://www.medicine.iupui.edu/clinpharm/ddis/table.asp
// Refs
// http://www.medicine.iupui.edu/clinpharm/ddis/1A2references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/2B6references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/2C8references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/2C9references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/2C19references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/2D6references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/2E1references.asp
// http://www.medicine.iupui.edu/clinpharm/ddis/3A457references.asp

#define TESTS_LIMIT_NUMBER_OF_PROCESSED_FILES_TO  10

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

enum { TestOnly = true };

enum InhibitorsLevel {
    IL_Strong = 0,
    IL_Moderate,
    IL_Weak,
    IL_Other
};

const char* const  TABLE_URL     = "http://www.medicine.iupui.edu/clinpharm/ddis/table.asp";
const char* const  REF_1_URL     = "http://www.medicine.iupui.edu/clinpharm/ddis/%1references.asp";

const char* const  DATABASE_NAME = "CYTP450";

static QStringList cytochromes = QStringList() << "1A2"<< "2B6" << "2C8" << "2C9" << "2C19" << "2D6" << "2E1" << "3A4,5,7";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/CytP450/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/iam-cytp450.db");}
static inline QString unlinkeAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/textfiles/cytochrome_unlinked.csv");}

static inline QString iamDatabaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

namespace IAMDb {
namespace Internal {

struct MolInfo {
    MolInfo() : typeOfInhibitor(-1), cytId(-1) {}

    QString substrate, inducer, inhibitor;
    int typeOfInhibitor, cytId;
    QString link;
};

class CytochromeP450InteractionsPrivate
{
public:
    QString m_WorkingPath;
    QProgressDialog *m_Progress;
    QNetworkAccessManager *manager;

    QMultiHash<int, QString> substrates, inhibitors, inducers;  // int correspond to the cytochromes.indexOf(cyt)
    QMultiHash<int, QPair<QString, int> > inhibitorsLevel;
    QList<MolInfo> molName_Link;  // int correspond to the cytochromes.indexOf(cyt)

};
}  // End namespace Internal
}  // End namespace IAMDb




CytochromeP450InteractionsWidget::CytochromeP450InteractionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CytochromeP450InteractionsPage),
    d(new Internal::CytochromeP450InteractionsPrivate)
{
    ui->setupUi(this);
    setObjectName("Cytochrome");
    d->m_WorkingPath = workingPath();
    if (!QDir().mkpath(d->m_WorkingPath))
        Utils::Log::addError(this, "Unable to create Cytochrome Working Path :" + d->m_WorkingPath, __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            Utils::Log::addError(this, "Unable to create Cytochrome database output path :" + dbpath, __FILE__, __LINE__);
        else
            Utils::Log::addMessage(this, "Cytochrome database output dir created");
    }

}

CytochromeP450InteractionsWidget::~CytochromeP450InteractionsWidget()
{
    delete ui; ui=0;
    if (d)
        delete d;
    d = 0;
}

bool CytochromeP450InteractionsWidget::on_download_clicked()
{
    ui->download->setEnabled(false);
    // TODO: First : download A..Z files; then download drugs files */
    d->m_Progress = new QProgressDialog(this);
    d->m_Progress->setLabelText(tr("Downloading Cytochrome P450 interactions table"));
    d->m_Progress->setCancelButtonText(tkTr(Trans::Constants::CANCEL));
    d->m_Progress->setRange(0, 1);
    d->m_Progress->setWindowModality(Qt::WindowModal);
    d->m_Progress->setValue(0);
    d->manager = new QNetworkAccessManager(this);
    // get all tradename html pages from the site
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(indexPageDownloaded(QNetworkReply*)));
    d->manager->get(QNetworkRequest(QUrl(QString(TABLE_URL))));
    return true;
}

void CytochromeP450InteractionsWidget::indexPageDownloaded(QNetworkReply *reply)
{
    qWarning() << "get index" << reply->errorString()
            << reply->url();
    QString content = reply->readAll();
    if (!Utils::saveStringToFile(content, workingPath()+"index.html"))
        Utils::Log::addError(this, "Unable to save Table Index", __FILE__, __LINE__);

    d->m_Progress->setRange(0, cytochromes.count());
    d->m_Progress->setLabelText(tr("Downloading references files"));
    disconnect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(indexPageDownloaded(QNetworkReply*)));
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(substancePageDownloaded(QNetworkReply*)));
    foreach(QString cyt, cytochromes) {
        QString fileName = cyt.remove(",") + "references.asp";
        if (!QFile(d->m_WorkingPath + fileName).exists()) {
            d->manager->get(QNetworkRequest(QUrl(QString(REF_1_URL).arg(cyt.remove(",")))));
        } else {
            d->m_Progress->setValue(d->m_Progress->value() + 1);
        }
    }
    d->m_Progress->close();
}

void CytochromeP450InteractionsWidget::substancePageDownloaded(QNetworkReply *reply)
{
    qWarning() << "get references" << reply->errorString()
            << reply->url();
//    QString content = reply->readAll();
    QString fileName = reply->url().toString(QUrl::RemoveScheme|QUrl::RemovePassword|QUrl::RemoveUserInfo);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf("/"));
    QString cyt = fileName.left(fileName.indexOf("references"));

    d->m_Progress->setValue(d->m_Progress->value() + 1);
    // save file
    if (!QDir(d->m_WorkingPath + fileName).exists()) {
        QDir().mkpath(QFileInfo(d->m_WorkingPath + fileName).absolutePath());
    }

    if (!Utils::saveStringToFile(reply->readAll(), d->m_WorkingPath + fileName)) {
        Utils::Log::addError(this, QString("ERROR : Unable to save %1").arg(fileName));
        return;
    }
}

void CytochromeP450InteractionsWidget::on_process_clicked()
{
    if (!QFile(iamDatabaseAbsPath()).exists()) {
        Utils::warningMessageBox(tr("Interaction database does not exists."),
                                 tr("You must create the standard interactions database before adding "
                                    "the cytochrome p450 interactions."));
        return;
    }

    QString content = Utils::readTextFile(workingPath()+"index.html");
    if (content.isEmpty()) {
        Utils::warningMessageBox(tr("Unable to find index.html file. Did you download the sources ?"),
                                 tr("Download the sources first. Process aborded."));
        return;
    }

    // Get Substrates
    int begin = content.indexOf("<h1>SUBSTRATES</h1>");
    // remove first <tr> of table
    begin = content.indexOf("<tr", begin) + 3;
    begin = content.indexOf("<tr", begin);
    int end = content.indexOf("<h1>INHIBITORS</h1>", begin);
    int col = 0;
    QString substratesText = content.mid(begin, end-begin);
    d->substrates.clear();
    // each <td> correspond to a cytochrome (same order as the cytochromes const list)
    foreach(const QString &column, substratesText.split("</td>", QString::SkipEmptyParts)) {
        foreach(const QString &line, column.split("\n", QString::SkipEmptyParts)) {
            if (line.startsWith("<strong"))
                continue;
            if (line.startsWith("<td>"))
                continue;
            if (line.startsWith("<tr"))
                continue;
            if (line.startsWith("</tr"))
                continue;
            if (line.startsWith("<a href")) {
                int b = line.indexOf("\">") + 2;
                int e = line.indexOf("</a>", b);
                QString tmp = line.mid(b, e-b);
                d->substrates.insertMulti(col, tmp);
            } else {
                QString tmp = line.left(line.indexOf("<br"));
                d->substrates.insertMulti(col, tmp);
            }
        }
        ++col;
    }

//    foreach(const int k, substrates.uniqueKeys()) {
//        qWarning() << k << substrates.values(k);
//    }

    // Get Inhibitors
    begin = content.indexOf("<h1>INHIBITORS</h1>");
    // remove first <tr> of table
    begin = content.indexOf("<tr bgcolor=\"#CCCCCC", begin) + 20;
    begin = content.indexOf("<tr", begin);
    end = content.indexOf("<h1>INDUCERS</h1>", begin);
    col = 0;
    QString inhibitorsText = content.mid(begin, end-begin);
    d->inhibitors.clear();
    d->inhibitorsLevel.clear();
    // each <td> correspond to a cytochrome (same order as the cytochromes const list)
    foreach(const QString &column, inhibitorsText.split("</td>", QString::SkipEmptyParts)) {
        foreach(const QString &line, column.split("\n", QString::SkipEmptyParts)) {
            if (line.startsWith("<strong"))
                continue;
            if (line.startsWith("<td>"))
                continue;
            if (line.startsWith("<tr"))
                continue;
            if (line.startsWith("</tr"))
                continue;
            if (line.startsWith("<a href")) {
                int b = line.indexOf("\">") + 2;
                int e = line.indexOf("</a>", b);
                QString tmp = line.mid(b, e-b);
                QPair<QString, int> p;
                p.first = tmp;
                p.second = IL_Other;
                d->inhibitorsLevel.insert(col, p);
            } else if (line.startsWith("<img")) {
                int b = line.indexOf("\">") + 2;
                int e = line.indexOf("</a>", b);
                QString tmp = line.mid(b, e-b);
                d->inhibitors.insertMulti(col, tmp);
                // Get level
                if (line.contains("red.jpg")) {
                    QPair<QString, int> p;
                    p.first = tmp;
                    p.second = IL_Strong;
                    d->inhibitorsLevel.insert(col, p);
                } else if (line.contains("orange.jpg")) {
                    QPair<QString, int> p;
                    p.first = tmp;
                    p.second = IL_Moderate;
                    d->inhibitorsLevel.insert(col, p);
                } else if (line.contains("green.jpg")) {
                    QPair<QString, int> p;
                    p.first = tmp;
                    p.second = IL_Weak;
                    d->inhibitorsLevel.insert(col, p);
                } else {
                    QPair<QString, int> p;
                    p.first = tmp;
                    p.second = IL_Other;
                    d->inhibitorsLevel.insert(col, p);
                }
            } else {
                QString tmp = line.left(line.indexOf("<br"));
                QPair<QString, int> p;
                p.first = tmp;
                p.second = IL_Other;
                d->inhibitorsLevel.insert(col, p);
            }
        }
        ++col;
    }

//    foreach(const int k, inhibitors.uniqueKeys()) {
//        qWarning() << k << inhibitors.values(k);
//    }
//    qWarning() << inhibitorsLevel.count();  //33

    // Get Inducers
    begin = content.indexOf("<h1>INDUCERS</h1>");
    // remove first <tr> of table
    begin = content.indexOf("<tr", begin) + 3;
    begin = content.indexOf("<tr", begin);
    end = content.indexOf("</table>", begin);
    col = 0;
    QString inducersText = content.mid(begin, end-begin);
    d->inducers.clear();
    // each <td> correspond to a cytochrome (same order as the cytochromes const list)
    foreach(const QString &column, inducersText.split("</td>", QString::SkipEmptyParts)) {
        foreach(const QString &line, column.split("\n", QString::SkipEmptyParts)) {
            if (line.startsWith("<strong"))
                continue;
            if (line.startsWith("<td>"))
                continue;
            if (line.startsWith("<tr"))
                continue;
            if (line.startsWith("</tr"))
                continue;
            if (line.startsWith("<a href")) {
                int b = line.indexOf("\">") + 2;
                int e = line.indexOf("</a>", b);
                QString tmp = line.mid(b, e-b);
                d->inducers.insertMulti(col, tmp);
            } else {
                QString tmp = line.left(line.indexOf("<br"));
                d->inducers.insertMulti(col, tmp);
            }
        }
        ++col;
    }

//    foreach(const int k, inducers.uniqueKeys()) {
//        qWarning() << k << inducers.values(k);
//    }
//    generateUnlinkedAtcCsvFile();
//    populateDatabase();
    createXML();
}

void CytochromeP450InteractionsWidget::generateUnlinkedAtcCsvFile()
{
    // Generates only once
    if (QFile(unlinkeAbsPath()).exists())
        return;

    QSqlDatabase iam = QSqlDatabase::addDatabase("QSQLITE", Core::Constants::IAM_DATABASE_NAME);
    iam.setDatabaseName(iamDatabaseAbsPath());
    if (!iam.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(iam.connectionName(), iam.lastError().text()));
        return;
    }

    if (!QDir(unlinkeAbsPath()).exists()) {
        QDir().mkpath(QFileInfo(unlinkeAbsPath()).absolutePath());
    }

    QStringList unfound;
    QSqlQuery toAtc(iam);
    QString req;
    foreach(const QString &mol, d->substrates.values()) {
        req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE \"%1\";").arg(mol);
        if (toAtc.exec(req)) {
            if (!toAtc.next()) {
                unfound << mol;
            }
        }
        toAtc.finish();
    }
    foreach(const QString &mol, d->inducers.values()) {
        req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE \"%1\";").arg(mol);
        if (toAtc.exec(req)) {
            if (!toAtc.next()) {
                unfound << mol;
            }
        }
        toAtc.finish();
    }
    foreach(const QString &mol, d->inhibitors.values()) {
        req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE \"%1\";").arg(mol);
        if (toAtc.exec(req)) {
            if (!toAtc.next()) {
                unfound << mol;
            }
        }
        toAtc.finish();
    }
    req = unfound.join(";\n");
    Utils::saveStringToFile(req, unlinkeAbsPath());
}

void CytochromeP450InteractionsWidget::getLinkReferences(const int cytId, QHash<QString, QString> correctedLinks)
{
    if (cytId < 0 || cytId >= cytochromes.count()) {
        return;
    }
    // Add some specific corrections
    correctedLinks.insert("fluoroquinolones", "fluoro");
    correctedLinks.insert("beta-naphthoflavone", "beta");
    correctedLinks.insert("methylcholanthrene", "methyl");
    correctedLinks.insert("S-warfarin", "warfarin");
    correctedLinks.insert("phenytoin-4-OH2", "phenytoin-4");
    correctedLinks.insert("losartan","lorsartan");
    correctedLinks.insert("S-naproxen&rarr;Nor","S-naproxen");
    correctedLinks.insert("R-mephobarbital", "mephobarbital");
    correctedLinks.insert("N,N-dimethylformamide", "dimethylformamide");
    correctedLinks.insert("acetaminophen&rarr;NAPQI", "acetaminophen");
    correctedLinks.insert("diethyl-dithiocarbamate", "dithiocarbamate");
    correctedLinks.insert("St. John's wort", "stjohnswort");

    QString fileName = cytochromes.at(cytId);
    fileName = workingPath() + fileName.remove(",") + "references.asp";
    QString content = Utils::readTextFile(fileName);
    if (content.isEmpty())
        return;
    int begin = content.indexOf("Peer-reviewed Literature References");
    int end = 0;
    QString proceed;
    // get Substrates
    proceed = Core::Tools::getBlock(content, begin, end, "<h2><font color=\"#FFFFFF\" style=\"background-color:#999999\">");
//    qWarning() << "proceed" << proceed;

    foreach(QString sub, d->substrates.values(cytId)) {
        sub = sub.simplified();
        // Find paragraph  "<b><a name="sub"Sub"...
        int b = proceed.indexOf("<b><a name=\"" + sub + "Sub");
        int e = proceed.indexOf("<b><a name=", b + 10);
        if (b==-1) {
            if (correctedLinks.contains(sub) && !correctedLinks.value(sub).isEmpty()) {
                b = proceed.indexOf("<b><a name=\"" + correctedLinks.value(sub) + "Sub");
                e = proceed.indexOf("<b><a name=", b + 10);
            } else {
                qWarning() << "SUBSTRATE NOT FOUND (no correctedLink)" << cytochromes.at(cytId) << sub;
                continue;
            }
            if (b==-1) {
                qWarning() << "SUBSTRATE NOT FOUND (correctedLink not found)" << cytochromes.at(cytId) << sub;
                continue;
            }
        }
        // Extract PIMD  "PMID "xxxx"</a>
        int s = 0;
        QString tmp = proceed.mid(b, e-b);
        while (true) {
            s = tmp.indexOf("PMID ", s);
            if (s == -1)
                break;
            e = tmp.indexOf("</a>", s);
            if (e==-1) {
                qWarning() << "PMID can not be extracted (wrong end)" << e << tmp;
            } else {
                s += 5;
                Internal::MolInfo info;
                info.cytId = cytId;
                info.substrate = sub;
                info.link = "http://www.ncbi.nlm.nih.gov/pubmed/" + tmp.mid(s, e-s);
                d->molName_Link.append(info);
            }
            s += 10;
        }
    }

    // get Inhibitors
    begin = end;
    proceed = Core::Tools::getBlock(content, begin, end, "<h2><font color=\"#FFFFFF\" style=\"background-color:#999999\">");
    QStringList inhibitors;
    typedef QPair<QString, int> MyPair;
    foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
        inhibitors << p.first;
    }

    foreach(QString sub, inhibitors) {
        sub = sub.simplified();
        // Find paragraph  "<b><a name="sub"Inh"...
        int b = proceed.indexOf("<b><a name=\"" + sub + "Inh");
        int e = proceed.indexOf("<b><a name=", b + 10);
        if (b==-1) {
            if (correctedLinks.contains(sub) && !correctedLinks.value(sub).isEmpty()) {
                b = proceed.indexOf("<b><a name=\"" + correctedLinks.value(sub).toLower() + "Inh");
                e = proceed.indexOf("<b><a name=", b + 10);
            } else {
                qWarning() << "INHIBITOR NOT FOUND (no correctedLink)" << cytochromes.at(cytId) << sub;
                continue;
            }
            if (b==-1) {
                qWarning() << "INHIBITOR NOT FOUND (correctedLink not found)" << cytochromes.at(cytId) << sub;
                continue;
            }
        }
        // Extract PIMD  "PMID "xxxx"</a>
        int s = 0;
        QString tmp = proceed.mid(b, e-b);
        while (true) {
            s = tmp.indexOf("PMID ", s);
            if (s == -1)
                break;
            e = tmp.indexOf("</a>", s);
            if (e==-1) {
                qWarning() << "INDUCER::PMID can not be extracted (wrong end)" << e << tmp;
            } else {
                s += 5;
                Internal::MolInfo info;
                info.cytId = cytId;
                info.inhibitor = sub;
                info.link = "http://www.ncbi.nlm.nih.gov/pubmed/" + tmp.mid(s, e-s);
                d->molName_Link.append(info);
            }
            s += 10;
        }
    }

    // get Inducers
    begin = end;
    proceed = Core::Tools::getBlock(content, begin, end, "<h2><font color=\"#FFFFFF\" style=\"background-color:#999999\">");
    foreach(QString sub, d->inducers.values(cytId)) {
        sub = sub.simplified();
        // Find paragraph  "<b><a name="sub"Ind"...
        int b = proceed.indexOf("<b><a name=\"" + sub.toLower() + "Ind");
        int e = proceed.indexOf("<b><a name=", b + 10);
        if (b==-1) {
            if (correctedLinks.contains(sub) && !correctedLinks.value(sub).isEmpty()) {
                b = proceed.indexOf("<b><a name=\"" + correctedLinks.value(sub).toLower() + "Ind");
                e = proceed.indexOf("<b><a name=", b + 10);
            } else {
                qWarning() << "INDUCER NOT FOUND (no correctedLink)" << cytochromes.at(cytId) << sub;
                continue;
            }
            if (b==-1) {
                qWarning() << "INDUCER NOT FOUND (correctedLink not found)" << cytochromes.at(cytId) << sub;
                continue;
            }
        }
        // Extract PIMD  "PMID "xxxx"</a>
        int s = 0;
        QString tmp = proceed.mid(b, e-b);
        while (true) {
            s = tmp.indexOf("PMID ", s);
            if (s == -1)
                break;
            e = tmp.indexOf("</a>", s);
            if (e==-1) {
                qWarning() << "INDUCER::PMID can not be extracted (wrong end)" << e << tmp;
            } else {
                s += 5;
                Internal::MolInfo info;
                info.cytId = cytId;
                info.inducer = sub;
                info.link = "http://www.ncbi.nlm.nih.gov/pubmed/" + tmp.mid(s, e-s);
                d->molName_Link.append(info);
            }
            s += 10;
        }
    }
}

void CytochromeP450InteractionsWidget::populateDatabase()
{
    // Send to IAM database
    QSqlDatabase iam = QSqlDatabase::addDatabase("QSQLITE", Core::Constants::IAM_DATABASE_NAME);
    iam.setDatabaseName(iamDatabaseAbsPath());
    if (!iam.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(iam.connectionName(), iam.lastError().text()));
        return;
    }

    // Read the csv link file
    QString links = Utils::readTextFile(unlinkeAbsPath());
    QHash<QString, QString> correctedLinks;
    foreach(const QString &line, links.split("\n",QString::SkipEmptyParts)) {
        int split = line.lastIndexOf(";");
        if ((split+1) == line.length())
            correctedLinks.insert(line.left(split), QString());
        else
            correctedLinks.insert(line.left(split), line.mid(split+1));
    }

    // Create the classes : ZP450..
    QStringList treeReqs;
    // Clean database first
    QString req = "DELETE FROM ATC WHERE CODE LIKE 'ZP450%';";
    Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);
    req = "DELETE FROM ATC_CLASS_TREE WHERE ID_CLASS > 200157;";
    Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);

    int i = 0;
    QSqlQuery query(iam);
    int id = 0;
    int cytId = 0;
    foreach(const QString &cyt, cytochromes) {

        // SUBSTRATES
        QString n = QString::number(i);
        n = n.rightJustified(3, '0');
        req = QString("INSERT INTO ATC (`CODE`, `FRENCH`, `ENGLISH`, `DEUTSCH`) VALUES "
                      "('%1', '%2', '%3', '%4');")
                .arg("ZP450"+n)
                .arg(QString("Substrats du cytochrome P450 %1").arg(cyt).toUpper())
                .arg(QString("Cytochrome P450 %1 substrates").arg(cyt).toUpper())
                .arg(QString("Cytochrom-P450 %1 substraten").arg(cyt).toUpper());
        // Add values to db
        if (query.exec(req)) {
            id = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        // Generate the tree (last ID_CLASS == 200157)
        QMultiHash<int, int> substratesLinkID;
        int substrateId = 0;
        foreach(const QString &mol, d->substrates.values(cytId)) {
            QSqlQuery toAtc(iam);
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(correctedLinks.value(mol));
            } else {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(mol);
            }
            if (toAtc.exec(req)) {
                bool found = false;
                while (toAtc.next()) {
                    found = true;
                    treeReqs << QString("INSERT INTO `ATC_CLASS_TREE` (`ID_CLASS`, `ID_ATC`) VALUES (%1,%2);")
                          .arg(id).arg(toAtc.value(0).toInt());
                    substratesLinkID.insertMulti(substrateId, toAtc.value(0).toInt());
                }
                if (!found) {
                    qWarning() << "NOT FOUND" << mol << correctedLinks.value(mol);
                }
            }
            toAtc.finish();
            ++substrateId;
        }
        int classSubstrateId = id;

        // INDUCERS
        ++i;
        n = QString::number(i);
        n = n.rightJustified(3, '0');
        req = QString("INSERT INTO ATC (`CODE`, `FRENCH`, `ENGLISH`, `DEUTSCH`) VALUES "
                      "('%1', '%2', '%3', '%4');")
                .arg("ZP450"+n)
                .arg(QString("Inducteurs du cytochrome P450 %1").arg(cyt).toUpper())
                .arg(QString("Cytochrome P450 %1 inducers").arg(cyt).toUpper())
                .arg(QString("Cytochrom-P450 %1 induktoren").arg(cyt).toUpper());
        // Add values to db
        id = -1;
        if (query.exec(req)) {
            id = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        // Generate the tree (last ID_CLASS == 200157)
        QMultiHash<int, int> inducersLinkID;
        int inducerId = 0;
        foreach(const QString &mol, d->inducers.values(cytId)) {
            if (mol.contains("wort")) {
                qWarning() << "xxxxxxxxxxxxxx" << mol << correctedLinks.value(mol);
            }

            QSqlQuery toAtc(iam);
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(correctedLinks.value(mol));
            } else {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(mol);
            }
            if (toAtc.exec(req)) {
                bool found = false;
                while (toAtc.next()) {
                    found = true;
                    treeReqs << QString("INSERT INTO `ATC_CLASS_TREE` (`ID_CLASS`, `ID_ATC`) VALUES (%1,%2);")
                          .arg(id).arg(toAtc.value(0).toInt());
                    inducersLinkID.insertMulti(inducerId, toAtc.value(0).toInt());
                }
                if (!found) {
                    qWarning() << "NOT FOUND" << mol << correctedLinks.value(mol);
                }
            }
            toAtc.finish();
            ++inducerId;
        }
        int classInducerId = id;

        // STRONG INHIBITORS
        ++i;
        n = QString::number(i);
        n = n.rightJustified(3, '0');
        req = QString("INSERT INTO ATC (`CODE`, `FRENCH`, `ENGLISH`, `DEUTSCH`) VALUES "
                      "('%1', '%2', '%3', '%4');")
                .arg("ZP450"+n)
                .arg(QString("Inhibiteurs de haut niveau du cytochrome P450 %1").arg(cyt).toUpper())
                .arg(QString("Strong cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                .arg(QString("Starken cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());
        // Add values to db
        id = -1;
        if (query.exec(req)) {
            id = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        // Generate the tree (last ID_CLASS == 200157)
        QMultiHash<int, int> strongInhibitorsLinkID;
        int inhitorId = 0;
        // Get list of weak inhibitors
        QStringList inhibitorsToProcess;
        typedef QPair<QString, int> MyPair;
        foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
            if (p.second==IL_Strong) {
                inhibitorsToProcess << p.first;
            }
        }
        foreach(const QString &mol, inhibitorsToProcess) {
            QSqlQuery toAtc(iam);
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(correctedLinks.value(mol));
            } else {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(mol);
            }
            if (toAtc.exec(req)) {
                bool found = false;
                while (toAtc.next()) {
                    found = true;
                    treeReqs << QString("INSERT INTO `ATC_CLASS_TREE` (`ID_CLASS`, `ID_ATC`) VALUES (%1,%2);")
                          .arg(id).arg(toAtc.value(0).toInt());
                    strongInhibitorsLinkID.insertMulti(inhitorId, toAtc.value(0).toInt());
                }
                if (!found) {
                    qWarning() << "NOT FOUND" << mol << correctedLinks.value(mol);
                }
            }
            toAtc.finish();
            ++inhitorId;
        }
        int classStrongInhibitors = id;

        // MODERATE INHIBITORS
        ++i;
        n = QString::number(i);
        n = n.rightJustified(3, '0');
        req = QString("INSERT INTO ATC (`CODE`, `FRENCH`, `ENGLISH`, `DEUTSCH`) VALUES "
                      "('%1', '%2', '%3', '%4');")
                .arg("ZP450"+n)
                .arg(QString("Inhibiteurs de niveau modéré du cytochrome P450 %1").arg(cyt).toUpper())
                .arg(QString("Moderate cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                .arg(QString("Moderate cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());
        // Add values to db
        id = -1;
        if (query.exec(req)) {
            id = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        // Generate the tree (last ID_CLASS == 200157)
        QMultiHash<int, int> moderateInhibitorsLinkID;
        inhitorId = 0;
        // Get list of moderate inhibitors
        inhibitorsToProcess.clear();
        foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
            if (p.second==IL_Moderate) {
                inhibitorsToProcess << p.first;
            }
        }
        foreach(const QString &mol, inhibitorsToProcess) {
            QSqlQuery toAtc(iam);
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(correctedLinks.value(mol));
            } else {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(mol);
            }
            if (toAtc.exec(req)) {
                bool found = false;
                while (toAtc.next()) {
                    found = true;
                    treeReqs << QString("INSERT INTO `ATC_CLASS_TREE` (`ID_CLASS`, `ID_ATC`) VALUES (%1,%2);")
                          .arg(id).arg(toAtc.value(0).toInt());
                    moderateInhibitorsLinkID.insertMulti(inhitorId, toAtc.value(0).toInt());
                }
                if (!found) {
                    qWarning() << "NOT FOUND" << mol << correctedLinks.value(mol);
                }
            }
            toAtc.finish();
            ++inhitorId;
        }
        int classModerateInhibitors = id;


        // WEAK INHIBITORS
        ++i;
        n = QString::number(i);
        n = n.rightJustified(3, '0');
        req = QString("INSERT INTO ATC (`CODE`, `FRENCH`, `ENGLISH`, `DEUTSCH`) VALUES "
                      "('%1', '%2', '%3', '%4');")
                .arg("ZP450"+n)
                .arg(QString("Inhibiteurs de faible niveau du cytochrome P450 %1").arg(cyt).toUpper())
                .arg(QString("Weak cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                .arg(QString("Schwach cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());
        // Add values to db
        id = -1;
        if (query.exec(req)) {
            id = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        // Generate the tree (last ID_CLASS == 200157)
        QMultiHash<int, int> weakInhibitorsLinkID;
        inhitorId = 0;
        // Get list of weak inhibitors
        inhibitorsToProcess.clear();
        foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
            if (p.second==IL_Weak) {
                inhibitorsToProcess << p.first;
            }
        }
        foreach(const QString &mol, inhibitorsToProcess) {
            QSqlQuery toAtc(iam);
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(correctedLinks.value(mol));
            } else {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(mol);
            }
            if (toAtc.exec(req)) {
                bool found = false;
                while (toAtc.next()) {
                    found = true;
                    treeReqs << QString("INSERT INTO `ATC_CLASS_TREE` (`ID_CLASS`, `ID_ATC`) VALUES (%1,%2);")
                          .arg(id).arg(toAtc.value(0).toInt());
                    weakInhibitorsLinkID.insertMulti(inhitorId, toAtc.value(0).toInt());
                }
                if (!found) {
                    qWarning() << "NOT FOUND" << mol << correctedLinks.value(mol);
                }
            }
            toAtc.finish();
            ++inhitorId;
        }
        int classWeakInhibitors = id;

        // OTHER INHIBITORS
        ++i;
        n = QString::number(i);
        n = n.rightJustified(3, '0');
        req = QString("INSERT INTO ATC (`CODE`, `FRENCH`, `ENGLISH`, `DEUTSCH`) VALUES "
                      "('%1', '%2', '%3', '%4');")
                .arg("ZP450"+n)
                .arg(QString("Inhibiteurs du cytochrome P450 %1").arg(cyt).toUpper())
                .arg(QString("Cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                .arg(QString("Cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());
        // Add values to db
        id = -1;
        if (query.exec(req)) {
            id = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        // Generate the tree (last ID_CLASS == 200157)
        QMultiHash<int, int> otherInhibitorsLinkID;
        int inhibitorId = 0;
        foreach(const QString &mol, d->inhibitors.values(cytId)) {
            QSqlQuery toAtc(iam);
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(correctedLinks.value(mol));
            } else {
                req = QString("SELECT ID FROM ATC WHERE ENGLISH LIKE '%1';").arg(mol);
            }
            if (toAtc.exec(req)) {
                bool found = false;
                while (toAtc.next()) {
                    found = true;
                    treeReqs << QString("INSERT INTO `ATC_CLASS_TREE` (`ID_CLASS`, `ID_ATC`) VALUES (%1,%2);")
                          .arg(id).arg(toAtc.value(0).toInt());
                    otherInhibitorsLinkID.insertMulti(inhibitorId, toAtc.value(0).toInt());
                }
                if (!found) {
                    qWarning() << "NOT FOUND" << mol << correctedLinks.value(mol);
                }
            }
            toAtc.finish();
            ++inhibitorId;
        }
        int classOtherInhibitors = id;


        // INTERACTIONS && INTERACTION_KNOWLEDGE
        int knowId = 0;

        // Inducters
        req = QString("INSERT INTO `INTERACTION_KNOWLEDGE` (`TYPE`,`RISK_FR`,`RISK_EN`, `REFERENCES_LINK`) VALUES "
                      "('450', \"%1\", \"%2\", \"http://tinyurl.com/23zrx7z\")")
                .arg(QString("Interaction d'induction du cytochrome P450 %1. Risque de sous-dosage du substrat.").arg(cyt))
                .arg(QString("Cytochrome P450 %1 substrate/inducer. Risk of underdosing of the substrate.").arg(cyt));
        if (query.exec(req)) {
            knowId = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        req = QString("INSERT INTO `INTERACTIONS` (`ATC_ID1`, `ATC_ID2`, `INTERACTION_KNOWLEDGE_ID`) VALUES "
                      "(%1, %2, %3);").arg(classSubstrateId).arg(classInducerId).arg(knowId);
        if (!query.exec(req)) {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();

        // Strong Inhibitors
        req = QString("INSERT INTO `INTERACTION_KNOWLEDGE` (`TYPE`,`RISK_FR`,`RISK_EN`, `REFERENCES_LINK`) VALUES "
                      "('450', \"%1\", \"%2\", \"http://tinyurl.com/23zrx7z\")")
                .arg(QString("Interaction d'inhibition de haut niveau du cytochrome P450 %1. Risque de surdosage du substrat important. Peut causer une augmentation > 5 fois des valeurs d'air sous la courbe des concentrations plasmatiques ou plus de 80% de baisse de la clairance.").arg(cyt))
                .arg(QString("Strong inhibition of the cytochrome P450 %1 . High risk of substrate overdose. Can cause a > 5-fold increase in the plasma area under the curve values or more than 80% decrease in clearance.").arg(cyt));
        if (query.exec(req)) {
            knowId = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        req = QString("INSERT INTO `INTERACTIONS` (`ATC_ID1`, `ATC_ID2`, `INTERACTION_KNOWLEDGE_ID`) VALUES "
                      "(%1, %2, %3);").arg(classSubstrateId).arg(classStrongInhibitors).arg(knowId);
        if (!query.exec(req)) {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();

        // Moderate Inhibitors
        req = QString("INSERT INTO `INTERACTION_KNOWLEDGE` (`TYPE`,`RISK_FR`,`RISK_EN`, `REFERENCES_LINK`) VALUES "
                      "('450', \"%1\", \"%2\", \"http://tinyurl.com/23zrx7z\")")
                .arg(QString("Interaction d'inhibition de niveau modéré du cytochrome P450 %1. Risque de surdosage du substrat. Peut causer une augmentation > 2 fois des valeurs d'air sous la courbe des concentrations plasmatiques ou plus de 50-80% de baisse de la clairance.").arg(cyt))
                .arg(QString("Cytochrome P450 %1 substrate/moderate inhibitor. Risk of substrate overdose. Can cause a > 2-fold increase in the plasma area under the curve values or more than 50-80% decrease in clearance.").arg(cyt));
        if (query.exec(req)) {
            knowId = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        req = QString("INSERT INTO `INTERACTIONS` (`ATC_ID1`, `ATC_ID2`, `INTERACTION_KNOWLEDGE_ID`) VALUES "
                      "(%1, %2, %3);").arg(classSubstrateId).arg(classModerateInhibitors).arg(knowId);
        if (!query.exec(req)) {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();

        // Weak Inhibitors
        req = QString("INSERT INTO `INTERACTION_KNOWLEDGE` (`TYPE`,`RISK_FR`,`RISK_EN`, `REFERENCES_LINK`) VALUES "
                      "('450', \"%1\", \"%2\", \"http://tinyurl.com/23zrx7z\")")
                .arg(QString("Interaction substrat/inhibiteur de niveau faible du cytochrome P450 %1. Risque de surdosage du substrat. Peut causer une augmentation > 1,25 fois mais < à 2 fois des valeurs d'air sous la courbe des concentrations plasmatiques ou plus de 20-50% de baisse de la clairance.").arg(cyt))
                .arg(QString("Cytochrome P450 %1 substrate/weak inhibitor. Risk of substrate overdose. Can cause a > 1.25-fold but < 2-fold increase in the plasma area under the curve values or more than 20-50% decrease in clearance.").arg(cyt));
        if (query.exec(req)) {
            knowId = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        req = QString("INSERT INTO `INTERACTIONS` (`ATC_ID1`, `ATC_ID2`, `INTERACTION_KNOWLEDGE_ID`) VALUES "
                      "(%1, %2, %3);").arg(classSubstrateId).arg(classWeakInhibitors).arg(knowId);
        if (!query.exec(req)) {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();

        // Other Inhibitors
        req = QString("INSERT INTO `INTERACTION_KNOWLEDGE` (`TYPE`,`RISK_FR`,`RISK_EN`, `REFERENCES_LINK`) VALUES "
                      "('450', \"%1\", \"%2\", \"http://tinyurl.com/23zrx7z\")")
                .arg(QString("Interaction d'inhibition du cytochrome P450 %1. Risque de surdosage du substrat.").arg(cyt))
                .arg(QString("Cytochrome P450 %1 substrate/inhibitor. Risk of substrate overdose.").arg(cyt));
        if (query.exec(req)) {
            knowId = query.lastInsertId().toInt();
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();
        req = QString("INSERT INTO `INTERACTIONS` (`ATC_ID1`, `ATC_ID2`, `INTERACTION_KNOWLEDGE_ID`) VALUES "
                      "(%1, %2, %3);").arg(classSubstrateId).arg(classOtherInhibitors).arg(knowId);
        if (!query.exec(req)) {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
        query.finish();

        ++cytId;
    }

    // Send ATC_CLASS_TREE
    QSqlQuery tree(iam);
    foreach(const QString &r, treeReqs) {
        if (!tree.exec(r))
            Utils::Log::addQueryError(this, tree, __FILE__, __LINE__);
        tree.finish();
    }
    treeReqs.clear();

    Utils::informativeMessageBox(tr("Process done."), tr("Thank you."));
}

void CytochromeP450InteractionsWidget::createXML()
{
    // Read the csv link file
    QString links = Utils::readTextFile(unlinkeAbsPath());
    QHash<QString, QString> correctedLinks;
    foreach(const QString &line, links.split("\n",QString::SkipEmptyParts)) {
        int split = line.lastIndexOf(";");
        if ((split+1) == line.length())
            correctedLinks.insert(line.left(split), QString());
        else
            correctedLinks.insert(line.left(split), line.mid(split+1));
    }

    int i = 0;
    int id = 0;
    int cytId = 0;
    QString xmlTree, xmlClass;
    foreach(const QString &cyt, cytochromes) {
        d->molName_Link.clear();
        getLinkReferences(cytId, correctedLinks);

        xmlTree += QString("    <Class sources=\"FreeMedForms\" review=\"\" reviewer=\"\" name=\"%1\" dateofreview=\"\">\n")
                   .arg(QString("Substrats du cytochrome P450 %1").arg(cyt).toUpper());

        xmlClass += QString("    <Label de=\"%3\" references=\"FreeMedForms\" atcCodes=\"\" comments=\"\" en=\"%2\" review=\"\" id=\"\" afssaps=\"%1\" reviewer=\"\" es=\"\" afssapsCat=\"class\" autoFound=\"\" dateofreview=\"\"/>\n")
                    .arg(QString("Substrats du cytochrome P450 %1").arg(cyt).toUpper())
                    .arg(QString("Cytochrome P450 %1 substrates").arg(cyt).toUpper())
                    .arg(QString("Cytochrom-P450 %1 substraten").arg(cyt).toUpper());

        foreach(const QString &mol, d->substrates.values(cytId)) {
            if (mol.startsWith("NOT"))
                continue;

            // get links
            QMultiHash<QString, QString> mol_links;
            foreach(const Internal::MolInfo &info, d->molName_Link) {
                if (!info.substrate.isEmpty()) {
                    mol_links.insertMulti(info.substrate, info.link);
                }
            }

            QString correctedMol = mol;
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                // use corrected key --> correctedLinks.value(mol)
                correctedMol = correctedLinks.value(mol);
            }

            xmlTree += QString("      <Molecule name=\"%1\">\n").arg(correctedMol);
            foreach(const QString &link, mol_links.values(mol)) {
                xmlTree += QString("        <Source link=\"%2\"/>\n").arg(link);
            }
            xmlTree += "      </Molecule>\n";
        }
        xmlTree += QString("    </Class>\n");


        // INDUCERS
        xmlTree += QString("    <Class sources=\"FreeMedForms\" review=\"\" reviewer=\"\" name=\"%1\" dateofreview=\"\">\n")
                   .arg(QString("Inducteurs du cytochrome P450 %1").arg(cyt).toUpper());

        xmlClass += QString("    <Label de=\"%3\" references=\"FreeMedForms\" atcCodes=\"\" comments=\"\" en=\"%2\" review=\"\" id=\"\" afssaps=\"%1\" reviewer=\"\" es=\"\" afssapsCat=\"class\" autoFound=\"\" dateofreview=\"\"/>\n")
                    .arg(QString("Inducteurs du cytochrome P450 %1").arg(cyt).toUpper())
                    .arg(QString("Cytochrome P450 %1 inducers").arg(cyt).toUpper())
                    .arg(QString("Cytochrom-P450 %1 induktoren").arg(cyt).toUpper());
        foreach(const QString &mol, d->inducers.values(cytId)) {
            if (mol.startsWith("NOT"))
                continue;

            // get links
            QMultiHash<QString, QString> mol_links;
            foreach(const Internal::MolInfo &info, d->molName_Link) {
                if (!info.inducer.isEmpty()) {
                    mol_links.insertMulti(info.inducer, info.link);
                }
            }

            QString correctedMol = mol;
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                // use corrected key --> correctedLinks.value(mol)
                correctedMol = correctedLinks.value(mol);
            }

            xmlTree += QString("      <Molecule name=\"%1\">\n").arg(correctedMol);
            foreach(const QString &link, mol_links.values(mol)) {
                xmlTree += QString("        <Source link=\"%2\"/>\n").arg(link);
            }
            foreach(const QString &link, mol_links.values(correctedMol)) {
                xmlTree += QString("        <Source link=\"%2\"/>\n").arg(link);
            }
            xmlTree += "      </Molecule>\n";
        }
        xmlTree += QString("    </Class>\n");

        xmlTree += QString("    <Class sources=\"FreeMedForms\" review=\"\" reviewer=\"\" name=\"%1\" dateofreview=\"\">\n")
                   .arg(QString("Inhibiteurs de haut niveau du cytochrome P450 %1").arg(cyt).toUpper());

        xmlClass += QString("    <Label de=\"%3\" references=\"FreeMedForms\" atcCodes=\"\" comments=\"\" en=\"%2\" review=\"\" id=\"\" afssaps=\"%1\" reviewer=\"\" es=\"\" afssapsCat=\"class\" autoFound=\"\" dateofreview=\"\"/>\n")
                    .arg(QString("Inhibiteurs de haut niveau du cytochrome P450 %1").arg(cyt).toUpper())
                    .arg(QString("Strong cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                    .arg(QString("Starken cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());

        QStringList inhibitorsToProcess;
        typedef QPair<QString, int> MyPair;
        foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
            if (p.second==IL_Strong) {
                inhibitorsToProcess << p.first;
            }
        }

        foreach(const QString &mol, inhibitorsToProcess) {
            if (mol.startsWith("NOT"))
                continue;

            // get links
            QMultiHash<QString, QString> mol_links;
            foreach(const Internal::MolInfo &info, d->molName_Link) {
                if (!info.inhibitor.isEmpty()) {
                    mol_links.insertMulti(info.inhibitor, info.link);
                }
            }

            QString correctedMol = mol;
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                // use corrected key --> correctedLinks.value(mol)
                correctedMol = correctedLinks.value(mol);
            }

            xmlTree += QString("      <Molecule name=\"%1\">\n").arg(correctedMol);
            foreach(const QString &link, mol_links.values(mol)) {
                xmlTree += QString("        <Source link=\"%2\"/>\n").arg(link);
            }
            xmlTree += "      </Molecule>\n";
        }
        xmlTree += QString("    </Class>\n");


        xmlTree += QString("    <Class sources=\"FreeMedForms\" review=\"\" reviewer=\"\" name=\"%1\" dateofreview=\"\">\n")
                   .arg(QString("Inhibiteurs de niveau modéré du cytochrome P450 %1").arg(cyt).toUpper());

        xmlClass += QString("    <Label de=\"%3\" references=\"FreeMedForms\" atcCodes=\"\" comments=\"\" en=\"%2\" review=\"\" id=\"\" afssaps=\"%1\" reviewer=\"\" es=\"\" afssapsCat=\"class\" autoFound=\"\" dateofreview=\"\"/>\n")
                    .arg(QString("Inhibiteurs de niveau modéré du cytochrome P450 %1").arg(cyt).toUpper())
                    .arg(QString("Moderate cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                    .arg(QString("Moderate cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());

        inhibitorsToProcess.clear();
        foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
            if (p.second==IL_Moderate) {
                inhibitorsToProcess << p.first;
            }
        }

        foreach(const QString &mol, inhibitorsToProcess) {
            if (mol.startsWith("NOT"))
                continue;

            // get links
            QMultiHash<QString, QString> mol_links;
            foreach(const Internal::MolInfo &info, d->molName_Link) {
                if (!info.inhibitor.isEmpty()) {
                    mol_links.insertMulti(info.inhibitor, info.link);
                }
            }

            QString correctedMol = mol;
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                // use corrected key --> correctedLinks.value(mol)
                correctedMol = correctedLinks.value(mol);
            }

            xmlTree += QString("      <Molecule name=\"%1\">\n").arg(correctedMol);
            foreach(const QString &link, mol_links.values(mol)) {
                xmlTree += QString("        <Source link=\"%2\"/>\n").arg(link);
            }
            xmlTree += "      </Molecule>\n";
        }
        xmlTree += QString("    </Class>\n");

        xmlTree += QString("    <Class sources=\"FreeMedForms\" review=\"\" reviewer=\"\" name=\"%1\" dateofreview=\"\">\n")
                   .arg(QString("Autres inhibiteurs du cytochrome P450 %1").arg(cyt).toUpper());

        xmlClass += QString("    <Label de=\"%3\" references=\"FreeMedForms\" atcCodes=\"\" comments=\"\" en=\"%2\" review=\"\" id=\"\" afssaps=\"%1\" reviewer=\"\" es=\"\" afssapsCat=\"class\" autoFound=\"\" dateofreview=\"\"/>\n")
                    .arg(QString("Autres inhibiteurs du cytochrome P450 %1").arg(cyt).toUpper())
                    .arg(QString("Other cytochrome P450 %1 inhibitors").arg(cyt).toUpper())
                    .arg(QString("anderen cytochrom-P450 %1 inhibitoren").arg(cyt).toUpper());

        inhibitorsToProcess.clear();
        foreach(const MyPair &p, d->inhibitorsLevel.values(cytId)) {
            if (p.second==IL_Other) {
                inhibitorsToProcess << p.first;
            }
        }

        foreach(const QString &mol, inhibitorsToProcess) {
            if (mol.startsWith("NOT"))
                continue;

            // get links
            QMultiHash<QString, QString> mol_links;
            foreach(const Internal::MolInfo &info, d->molName_Link) {
                if (!info.inhibitor.isEmpty()) {
                    mol_links.insertMulti(info.inhibitor, info.link);
                }
            }

            QString correctedMol = mol;
            if (correctedLinks.keys().contains(mol) && !correctedLinks.value(mol).isEmpty()) {
                // use corrected key --> correctedLinks.value(mol)
                correctedMol = correctedLinks.value(mol);
            }

            xmlTree += QString("      <Molecule name=\"%1\">\n").arg(correctedMol);
            foreach(const QString &link, mol_links.values(mol)) {
                xmlTree += QString("        <Source link=\"%2\"/>\n").arg(link);
            }
            xmlTree += "      </Molecule>\n";
        }
        xmlTree += QString("    </Class>\n");

        ++cytId;
    }

    Utils::saveStringToFile(xmlTree, workingPath()+"cytp450_tree.xml");
    Utils::saveStringToFile(xmlClass, workingPath()+"cytp450_class.xml");

    Utils::informativeMessageBox(tr("Process done."), tr("Thank you."));
}

void CytochromeP450InteractionsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
