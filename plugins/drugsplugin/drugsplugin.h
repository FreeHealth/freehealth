#ifndef DRUGSPLUGIN_H
#define DRUGSPLUGIN_H

#include <extensionsystem/iplugin.h>

namespace Drugs {
class DrugsViewOptionsPage;
class DrugsUserOptionsPage;
class DrugsExtraOptionsPage;
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

private:
    Drugs::DrugsViewOptionsPage *viewPage;
    Drugs::DrugsUserOptionsPage *userPage;
    Drugs::DrugsExtraOptionsPage *extraPage;
};

} // namespace Internal
} // namespace Drugs

#endif // COREPLUGIN_H
