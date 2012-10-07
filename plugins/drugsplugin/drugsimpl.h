#ifndef COREIMPL_H
#define COREIMPL_H

#include "icore.h"

namespace Core {
    class MainWindow;
    class ActionManager;
    class ISettings;

namespace Internal {
    class ThemePrivate;
    class ActionManagerPrivate;
    class ContextManagerPrivate;
    class SettingsPrivate;
}
}


namespace Core {
namespace Internal {

class CoreImpl : public Core::ICore
{
    Q_OBJECT
public:
    CoreImpl(QObject *parent);
    ~CoreImpl();

    ActionManager *actionManager() const;
    ContextManager *contextManager() const;
    UniqueIDManager *uniqueIDManager() const;

    ITheme *theme() const;
    Translators *translators() const;

    ISettings *settings() const;

    QMainWindow *mainWindow() const;
    void setPadTools(IPadTools *pad) {m_PadTools=pad;}
    IPadTools *padTools() const {return m_PadTools;}

    // initialization
    bool initialize();
    bool extensionsInitialized();

private:
    MainWindow *m_MainWindow;
    ActionManagerPrivate *m_ActionManager;
    ContextManagerPrivate *m_ContextManager;
    UniqueIDManager *m_UID;
    ThemePrivate *m_Theme;
    Translators *m_Translators;
    SettingsPrivate *m_Settings;
    IPadTools *m_PadTools;
};

} // namespace Internal
} // namespace Core

#endif // COREIMPL_H
