#ifndef DRUGSPLUGIN_H
#define DRUGSPLUGIN_H

#include <extensionsystem/iplugin.h>

namespace Drugs {
namespace Internal {
    class CoreImpl;
}
}


namespace Drugs {
namespace Internal {

class DrugsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    DrugsPlugin();
    ~DrugsPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

public slots:
    void remoteArgument(const QString&);
};

} // namespace Internal
} // namespace Drugs

#endif // COREPLUGIN_H
