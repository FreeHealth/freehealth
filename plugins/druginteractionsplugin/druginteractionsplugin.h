/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGINTERACTIONS_PLUGIN_H
#define DRUGINTERACTIONS_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file druginteractionsplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 17 Jan 2012
*/

namespace DrugInteractions {
namespace Internal {
class DrugDrugInteractionEngine;
class PimEngine;
class DrugAllergyEngine;

class DrugInteractionsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    DrugInteractionsPlugin();
    ~DrugInteractionsPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

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
