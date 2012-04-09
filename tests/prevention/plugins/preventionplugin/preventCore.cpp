#include "preventCore.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>

#include <QApplication>
#include <QDesktopWidget>

static inline Core::ICore *core() {return Core::ICore::instance();}
static inline Core::IMainWindow *mainWindow() {return core()->mainWindow();}

PreventCore::PreventCore(QObject * object) 
{
    qWarning() << "Processing PreventCore" ;
    m_ihm = new PreventIHM(mainWindow());
    mainWindow()->setCentralWidget(m_ihm);
}

PreventCore::~PreventCore()
{
    qWarning() << "PreventCore::~PreventCore()" ;
}

void PreventCore::run()
{
   m_ihm->show();
}
