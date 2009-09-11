#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <extensionsystem/iplugin.h>

namespace Core {
namespace Internal {
    class CoreImpl;
}
}


namespace Core {
namespace Internal {

class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    CorePlugin();
    ~CorePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

public Q_SLOTS:
    void remoteArgument(const QString&);

private:
    CoreImpl *m_CoreImpl;
};

} // namespace Internal
} // namespace Core

#endif // COREPLUGIN_H
