#ifndef ICORE_H
#define ICORE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>

namespace Core {
class ActionManager;
class ContextManager;
class UniqueIDManager;
class Translators;
class ITheme;
class ISettings;
class IMainWindow;
class FormManager;
}

namespace Core {

class CORE_EXPORT ICore : public QObject
{
    Q_OBJECT

public:
    ICore(QObject *parent) : QObject(parent) {}
    virtual ~ICore() {}

    static ICore *instance();

//    virtual QStringList showNewItemDialog(const QString &title,
//                                          const QList<IWizard *> &wizards,
//                                          const QString &defaultLocation = QString()) = 0;
//
//    virtual bool showOptionsDialog(const QString &group = QString(),
//                                   const QString &page = QString(),
//                                   QWidget *parent = 0) = 0;

    virtual ActionManager *actionManager() const = 0;
    virtual ContextManager *contextManager() const = 0;
    virtual UniqueIDManager *uniqueIDManager() const = 0;

    virtual ITheme *theme() const = 0;
    virtual Translators *translators() const = 0;

    virtual ISettings *settings() const = 0;

    virtual IMainWindow *mainWindow() const = 0;

    virtual FormManager *formManager() const = 0;

//    virtual FileManager *fileManager() const = 0;
//    virtual MessageManager *messageManager() const = 0;
//    virtual EditorManager *editorManager() const = 0;
//    virtual ProgressManager *progressManager() const = 0;
//    virtual ScriptManager *scriptManager() const = 0;
//    virtual VariableManager *variableManager() const = 0;
//    virtual VCSManager *vcsManager() const = 0;
//    virtual ModeManager *modeManager() const = 0;
//    virtual MimeDatabase *mimeDatabase() const = 0;
//
//    virtual QSettings *settings() const = 0;
//    virtual SettingsDatabase *settingsDatabase() const = 0;
//    virtual QPrinter *printer() const = 0;
//
//    virtual QString resourcePath() const = 0;

Q_SIGNALS:
    void coreAboutToOpen();
    void coreOpened();
    void saveSettingsRequested();
    void optionsDialogRequested();
    void coreAboutToClose();
};

} // namespace Core

#endif // ICORE_H
