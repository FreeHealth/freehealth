/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "iconbadgealert.h"
#include <QString>

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
#import <AppKit/NSDockTile.h>
#import <AppKit/NSApplication.h>
#import <Foundation/NSString.h>
#import <Foundation/NSAutoreleasePool.h>
#endif

namespace Utils {
namespace MacOs {


#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    void setIconBadgeLabel(const QString &text)
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        const char *utf8String = text.toUtf8().constData();
        NSString *cocoaString = [[NSString alloc] initWithUTF8String:utf8String];
        [[NSApp dockTile] setBadgeLabel:cocoaString];
        [cocoaString release];
        [pool release];
    }

#else

    void setApplicationLabel(const QString &text) {Q_UNUSED(text);}

#endif

}  // End namespace MacOs
}  // End namespace Utils

