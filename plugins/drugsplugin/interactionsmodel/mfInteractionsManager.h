/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFINTERACTIONSMANAGER_H
#define MFINTERACTIONSMANAGER_H

// include drugswidget headers
#include <mfDrugsConstants.h>

// include toolkit headers
#include <utils/database.h>

// include Qt headers
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>
#include <QMultiMap>

/**
 * \file mfInteractionsManager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 24 July 2009
*/

using namespace mfInteractionsConstants;

namespace Drugs {
namespace Internal {
class DrugsData;
class DrugInteraction;
class InteractionsManagerPrivate;

class InteractionsManager : public QObject
{
    Q_OBJECT
public:
    InteractionsManager(QObject *parent = 0);
    ~InteractionsManager();

    void addDrug(DrugsData *drug);
    void setDrugsList(const QList<DrugsData *> &list);
    void removeLastDrug();
    void clearDrugsList();

    bool checkInteractions();            // must be called first

    bool drugHaveInteraction( const DrugsData *d ) const;                      // must call first interactions()
    Interaction::TypesOfIAM getMaximumTypeOfIAM( const DrugsData *d ) const;   // must call first interactions()
    QList<DrugInteraction*> getInteractions( const DrugsData *d ) const;           // must call first interactions()
    DrugInteraction * getLastInteractionFound() const;                      // must call first interactions()
    QList<DrugInteraction*> getAllInteractionsFound() const;                      // must call first interactions()

    QIcon iamIcon( const DrugsData *drug, const int &levelOfWarning = 0 ) const;
    static QString listToHtml( const QList<DrugInteraction *> & list, bool fullInfos );
    static QString synthesisToHtml( const QList<DrugInteraction *> & list, bool fullInfos );

private:
    // intialization state
    static bool m_initialized;
    InteractionsManagerPrivate * d;
};

}  // End Internal
}  // End Drugs

#endif   // MFINTERACTIONSMANAGER_H
