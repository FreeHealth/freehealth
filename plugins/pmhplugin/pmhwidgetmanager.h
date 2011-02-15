#ifndef PMHWIDGETMANAGER_H
#define PMHWIDGETMANAGER_H

#include <pmhplugin/pmh_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QPointer>


namespace PMH {
class PmhContextualWidget;
namespace Internal {

class PMH_EXPORT PmhActionHandler : public QObject
{
    Q_OBJECT
public:
    PmhActionHandler(QObject *parent = 0);
    virtual ~PmhActionHandler() {}

    void setCurrentView(PmhContextualWidget *view);

private Q_SLOTS:
    void showPmhDatabaseInformations();
    void createPmh();
    void categoryManager();

private:
    void updateActions();

protected:
    QAction *aAddPmh;
    QAction *aRemovePmh;
    QAction *aAddCat;
    QAction *aRemoveCat;
    QAction *aCategoryManager;
    QAction *aPmhDatabaseInformations;
    QPointer<PmhContextualWidget> m_CurrentView;
};

}  // End namespace Internal

class PmhWidgetManager : private Internal::PmhActionHandler
{
    Q_OBJECT
public:
    static PmhWidgetManager *instance(QObject *parent = 0);
    PmhContextualWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    PmhWidgetManager(QObject *parent = 0);
    static PmhWidgetManager *m_Instance;
};


}  // End namespace PMH


#endif // PMHWIDGETMANAGER_H
