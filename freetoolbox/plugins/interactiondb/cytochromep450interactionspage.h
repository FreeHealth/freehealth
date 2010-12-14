#ifndef CYTOCHROMEP450INTERACTIONSPAGE_H
#define CYTOCHROMEP450INTERACTIONSPAGE_H

#include <coreplugin/itoolpage.h>
#include <QWidget>

class QNetworkReply;

namespace IAMDb {
namespace Internal {
class CytochromeP450InteractionsPrivate;
}

namespace Ui {
    class CytochromeP450InteractionsPage;
}

class CytochromeP450InteractionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CytochromeP450InteractionsWidget(QWidget *parent = 0);
    ~CytochromeP450InteractionsWidget();

protected Q_SLOTS:
    bool on_download_clicked();
    void indexPageDownloaded(QNetworkReply *reply);
    void substancePageDownloaded(QNetworkReply *reply);
    void on_process_clicked();
    void generateUnlinkedAtcCsvFile();
    void getLinkReferences(const int cytId, QHash<QString, QString> correctedLinks);
    void populateDatabase();
    void createXML();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CytochromeP450InteractionsPage *ui;
    Internal::CytochromeP450InteractionsPrivate *d;
};


class CytochromeP450InteractionsPage : public Core::IToolPage
{
    Q_OBJECT

public:
    CytochromeP450InteractionsPage(QObject *parent = 0) : IToolPage(parent) {}
    ~CytochromeP450InteractionsPage() {}

    virtual QString id() const {return "CytochromeP450InteractionsPage";}
    virtual QString name() const {return "Cytochrome P450 interactions";}
    virtual QString category() const {return tr("Interaction database");}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0) {return new CytochromeP450InteractionsWidget(parent);}
};


}  // End IAMDb

#endif // CYTOCHROMEP450INTERACTIONSPAGE_H
