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
 *       David Geldreich <david.geldreich@free.fr>                         *
 ***************************************************************************/
#ifndef TKSERIALNUMBER_H
#define TKSERIALNUMBER_H

#include <tkExporter.h>

#include <QObject>

/**
 * \file tkSerialNumber.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.1
 * \date 04 May 2009
*/

/**
  \brief Serial number management.
  \ingroup toolkit
  \ingroup object_toolkit
*/
Q_TK_BEGIN_SERIALNUMBER

enum Ability {
    NoAbility        = 0x00,
    PrintAbility     = 0x01,
    MedinTuxAbility  = 0x02,
    SupportAbility   = 0x04
};
Q_DECLARE_FLAGS( Abilities, Ability );

Q_TK_EXPORT const void initialize();
Q_TK_EXPORT const QString createSerialNumber( Abilities ab = 0, const int lenght = 10 );
Q_TK_EXPORT const bool isValid( const QString &sn );
Q_TK_EXPORT const bool testAbility( Ability ab, const QString &sn );

Q_TK_END_SERIALNUMBER

Q_DECLARE_OPERATORS_FOR_FLAGS( tkSerialNumber::Abilities )

#endif // TKSERIALNUMBER_H
