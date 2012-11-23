/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFINTERACTIONSMANAGER_H
#define MFINTERACTIONSMANAGER_H

// include drugswidget headers
#include <mfDrugsConstants.h>
class mfDrugs;
class mfDrugInteraction;
class mfInteractionsManagerPrivate;

// include toolkit headers
#include <tkDatabase.h>

// include Qt headers
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>
#include <QMultiMap>

/**
 * \file mfInteractionsManager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.4
 * \date 24 July 2009
*/

using namespace mfInteractionsConstants;

class mfInteractionsManager : public QObject
{
    Q_OBJECT
public:
    mfInteractionsManager(QObject *parent = 0);
    ~mfInteractionsManager();

    void addDrug(mfDrugs *drug);
    void setDrugsList(const QList<mfDrugs*> &list);
    void removeLastDrug();
    void clearDrugsList();

    bool checkInteractions();            // must be called first

    bool drugHaveInteraction( const mfDrugs * d ) const;                      // must call first interactions()
    Interaction::TypesOfIAM getMaximumTypeOfIAM( const mfDrugs * d ) const;   // must call first interactions()
    QList<mfDrugInteraction*> getInteractions( const mfDrugs * d ) const;           // must call first interactions()
    mfDrugInteraction * getLastInteractionFound() const;                      // must call first interactions()
    QList<mfDrugInteraction*> getAllInteractionsFound() const;                      // must call first interactions()

    QIcon iamIcon( const mfDrugs * drug, const int & levelOfWarning = 0 ) const;
    static QString listToHtml( const QList<mfDrugInteraction*> & list, bool fullInfos );
    static QString synthesisToHtml( const QList<mfDrugInteraction*> & list, bool fullInfos );

private:
    // intialization state
    static bool m_initialized;
    mfInteractionsManagerPrivate * d;
};

#endif   // MFINTERACTIONSMANAGER_H
