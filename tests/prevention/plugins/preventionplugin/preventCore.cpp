#include "preventCore.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>

#include <QApplication>
#include <QDesktopWidget>

static inline Core::ICore *core() {return Core::ICore::instance();}
static inline Core::IMainWindow *mainWindow() {return core()->mainWindow();}


PreventCore::PreventCore(QObject * object) {
    QWidget * parent = QApplication::desktop()->screen();
    m_ihm = new PreventIHM(mainWindow());
    mainWindow()->setCentralWidget(m_ihm);
//    m_ihm->move(parent->width()/4,parent->height()/4);



    //m_IO = new PreventIO(this);
    //m_modelForTreeView = new VariantItemModel(this);
    /*m_modelForTreeView = m_IO->getVariantItemModel();
    m_ihm->m_treeView->setModel(m_modelForTreeView);
    m_ihm->m_treeView->collapseAll();*/
    qDebug() << __FILE__ << QString::number(__LINE__)   ;
}

PreventCore::~PreventCore(){}

void PreventCore::run(){
   qDebug() << __FILE__ << QString::number(__LINE__)   ;
   
   qDebug() << __FILE__ << QString::number(__LINE__)   ;
   m_ihm->show();
   qDebug() << __FILE__ << QString::number(__LINE__)   ;
}
