/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGINTERACTIONS_LIBRARYEXPORTER_H
#define DRUGINTERACTIONS_LIBRARYEXPORTER_H

#include <qglobal.h>

#if defined(DRUGINTERACTIONS_LIBRARY)
#define DRUGINTERACTIONS_EXPORT Q_DECL_EXPORT
#else
#define DRUGINTERACTIONS_EXPORT Q_DECL_IMPORT
#endif

#endif
