#include <MainWindow.h>

#include <QApplication>
#include <QVariant>
#include <QMainWindow>
#include <QDebug>

#include <tkActionManager.h>
#include <tkContextManager.h>
#include <tkTheme.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("actionmanagertester");
    qWarning() << app.applicationName();

    tkTheme::instance();
    tkTheme::instance()->setThemeRootPath(qApp->applicationDirPath() + "/../../../../global_resources/");

    MainWindow *mainwin = new MainWindow();
    tkActionManager::instance(mainwin);
    qWarning() << "main" << mainwin;
    tkContextManager::instance(mainwin);
    mainwin->initialize();
    mainwin->show();
    QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

    return app.exec();
}
