/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGINTERACTIONS_PLUGIN_H
#define DRUGINTERACTIONS_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file druginteractionsplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace DrugInteractions {
namespace Internal {
class DrugDrugInteractionEngine;
class PimEngine;
class DrugAllergyEngine;

class DrugInteractionsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.DrugInteractionsPlugin" FILE "DrugInteractions.json")

public:
    DrugInteractionsPlugin();
    ~DrugInteractionsPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();

private:
    DrugDrugInteractionEngine *m_DDIEngine;
    PimEngine *m_PimEngine;
    DrugAllergyEngine *m_AllergyEngine;
};


}  // End namespace Internal
}  // End namespace DrugInteractions

#endif  // End DRUGINTERACTIONS_PLUGIN_H
