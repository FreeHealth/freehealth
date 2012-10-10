#ifndef FULLRELEASEPAGE_H
#define FULLRELEASEPAGE_H

#include <coreplugin/itoolpage.h>

#include <QLabel>
#include <QHash>
class QSpacerItem;

namespace Core {

namespace Ui {
class FullReleasePage;
}

class FullReleasePage : public Core::IToolPage
{
public:
    FullReleasePage(QObject *parent = 0);
    ~FullReleasePage();

    QString id() const {return "FullReleasePage";}
    QString name() const {return tr("Full release data set");}
    QString category() const {return tr("General");}
    QIcon icon() const;

    // widget will be deleted after the show
    QWidget *createPage(QWidget *parent = 0);

    void addDownloadingProcess(const QString &message, const QString &id);
    void endDownloadingProcess(const QString &id);

    void addRunningProcess(const QString &message);
    void endLastAddedProcess();

private:
    QWidget *m_Widget, *m_CentralWidget;
    Ui::FullReleasePage *ui;
    QHash<QString, QLabel *> m_IconLabels;
    QSpacerItem *m_Spacer;
};

}  //  End namespace Core

#endif // FULLRELEASEPAGE_H
