#include "drugsplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <QtCore/QtPlugin>

#include <QDebug>

using namespace Drugs::Internal;

DrugsPlugin::DrugsPlugin()
{
}

DrugsPlugin::~DrugsPlugin()
{
}

bool DrugsPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    return true;
}

void DrugsPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new Core::PluginAboutPage(this->pluginSpec(), this));
}

void DrugsPlugin::remoteArgument(const QString& arg)
{
    qWarning() << "drugplugin" << arg;
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

Q_EXPORT_PLUGIN(DrugsPlugin)
