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
 ***************************************************************************/
#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <utils/global_exporter.h>

#include <QObject>
#include <QUrl>
#include <QString>
#include <QProgressBar>
class QWidget;

namespace Utils {
namespace Constants {
    const char* const  FREEMEDFORMS_UPDATE_URL  = "http://www.ericmaeker.fr/FreeMedForms/update-fmf.txt";
    const char* const  FREEDIAMS_UPDATE_URL     = "http://www.ericmaeker.fr/FreeMedForms/update-di.txt";
}
}

/**
 * \file updatechecker.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 30 Sept 2009
*/

namespace Utils {
namespace Internal {
class UpdateCheckerPrivate;
}

class UTILS_EXPORT UpdateChecker : public QObject
{
    Q_OBJECT
    friend class Internal::UpdateCheckerPrivate;

public:
     UpdateChecker(QObject *parent = 0);
    ~UpdateChecker();

    void check( const QString &url );
    void check( const QUrl &url );
    void cancel();

    bool fileRetreived();
    bool hasUpdate();
    QString lastVersion();
    QString updateText();

    QProgressBar *progressBar(QWidget *parent);

public Q_SLOTS:
    void showUpdateInformations();

Q_SIGNALS:
    void updateFound(const QString &updateText);
    void updateFound();
    void done(bool);

protected:
    void emitSignals();

private:
    Internal::UpdateCheckerPrivate *d;
};
}

#endif  // UPDATECHECKER_H
