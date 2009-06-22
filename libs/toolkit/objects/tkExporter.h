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
#ifndef TKEXPORTER_H
#define TKEXPORTER_H

#include <qglobal.h>

// DEFINE EXPORTER
#if defined(TOOLKIT_CORE_BUILD)
#define Q_TK_EXPORT Q_DECL_EXPORT
#else
#define Q_TK_EXPORT Q_DECL_IMPORT
#endif

// DEFINE NAMESPACES
#define Q_TK_BEGIN_GLOBAL  namespace tkGlobal {
#define Q_TK_END_GLOBAL    }
#define Q_TK_USING_GLOBAL  using namespace tkGlobal;

/**
 \namespace tkConstants
 \brief Contains all constants for the toolkit.
 \ingroup toolkit object_toolkit widget_toolkit database_toolkit
*/
#define Q_TK_BEGIN_CONSTANTS  namespace tkConstants {
#define Q_TK_END_CONSTANTS    }
#define Q_TK_USING_CONSTANTS  using namespace tkConstants;

#define Q_TK_BEGIN_SERIALIZER  namespace tkSerializer {
#define Q_TK_END_SERIALIZER    }
#define Q_TK_USING_SERIALIZER  using namespace tkSerializer;

/**
 \namespace tkSerialNumber
 \ingroup toolkit object_toolkit
 */
#define Q_TK_BEGIN_SERIALNUMBER  namespace tkSerialNumber {
#define Q_TK_END_SERIALNUMBER    }
#define Q_TK_USING_SERIALNUMBER  using namespace tkSerialNumber;

#endif  // TKEXPORTER_H
