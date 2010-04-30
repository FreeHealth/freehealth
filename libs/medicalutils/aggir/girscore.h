/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef GIRSCORE_H
#define GIRSCORE_H

#include <medicalutils/medical_exporter.h>

#include <QString>

namespace MedicalUtils {
namespace AGGIR {
class GirScorePrivate;

class MEDICALUTILS_EXPORT GirScore
{
public:

    GirScore();
    ~GirScore();

    enum chaineGIR {
        GIR_coherence = 0,
        GIR_orientation,
        GIR_toilette,
        GIR_habillage,
        GIR_alimentation,
        GIR_elimination,
        GIR_transferts,
        GIR_interieur
    };

    bool isNull() const;
    bool isValid() const;

    // Variables
    void setValues(const QString &coherence,
                   const QString &orientation,
                   const QString &toilette,
                   const QString &habillage,
                   const QString &alimentation,
                   const QString &elimination,
                   const QString &transferts,
                   const QString &interieur);

    void setCoherence(const QString &val);
    void setOrientation(const QString &val);
    void setToilette(const QString &val);
    void setHabillage(const QString &val);
    void setAlimentation(const QString &val);
    void setElimination(const QString &val);
    void setTransferts(const QString &val);
    void setInterieur(const QString &val);

    // Descriptives
    void setExterieur(const QString &val);
    void setCommunication(const QString &val);

    int resultingGir() const;
    QString explanations(int girScore) const;

//protected:
    QString getGirString() const;

private:
    GirScorePrivate *d;
};

} // End AGGIR
} // End MedicalUtils

#endif  // End GIRSCORE_H

