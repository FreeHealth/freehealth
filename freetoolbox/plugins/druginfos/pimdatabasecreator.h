#ifndef PIMDATABASECREATOR_H
#define PIMDATABASECREATOR_H

#include <coreplugin/ifullreleasestep.h>
#include <coreplugin/itoolpage.h>

#include <QStringList>
#include <QVector>
#include <QWidget>
#include <QDomDocument>
#include <QMultiHash>
#include <QString>


namespace Utils {
    class PubMedDownloader;
}

namespace DrugInfos {
class PimStep;

namespace Ui {
    class PimDatabaseCreator;
}

class PimDatabaseCreator : public QWidget
{
    Q_OBJECT

public:
    explicit PimDatabaseCreator(QWidget *parent = 0);
    ~PimDatabaseCreator();

private Q_SLOTS:
    void on_createAndSave_clicked();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PimDatabaseCreator *ui;
    PimStep *m_Step;
};

class PimStep : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    PimStep(QObject *parent = 0);

    QString id() const {return "PimStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::PimDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();

    QString processMessage() const {return tr("Potentially inappropriate medications database creation");}
//    bool postProcessDownload() {m_ActiveDownloadId = -1; downloadNextSource(); return true;}
    bool postProcessDownload() {Q_EMIT postProcessDownloadFinished(); return true;}

    QStringList errors() const {return m_Errors;}

private:
    void savePim(const QDomElement &element, const int sourceId, const int typeId, const QMultiHash<QString, QString> &molNameToAtcCode);

//public Q_SLOTS:
//    void downloadNextSource();

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
    int m_ActiveDownloadId;
    QVector<int> m_SourceToDownload;
    Utils::PubMedDownloader *m_Downloader;
};

class PimDatabasePage : public Core::IToolPage
{
    Q_OBJECT

public:
    PimDatabasePage(QObject *parent = 0) : IToolPage(parent) {}
    ~PimDatabasePage() {}

    virtual QString id() const {return "PimDatabasePage";}
    virtual QString name() const {return "PIM Database Builder";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0) {return new PimDatabaseCreator(parent);}
};

}  //  End namespace DrugInfos

#endif // PIMDATABASECREATOR_H
