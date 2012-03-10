/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "versionnumber.h"

#include <utils/log.h>

/**
  \class Utils::VersionNumber
  \brief Ease the processing of version strings.
  Extract the major, minor and debug versions. Extract the alpha version.\n
  The version string must be formatted as follow: \n
      - major.minor.debug
      - major.minor.debug~alpha
      - major.minor.debug~alphaalpha_nb
      - major.minor.debug~beta
      - major.minor.debug~betabeta_nb
      - major.minor.debug~RC
      - major.minor.debug~RCrc_nb
*/

using namespace Utils;

VersionNumber::VersionNumber(const QString &version) :
        m_Version(version),
        m_Major(0),
        m_Minor(0),
        m_Debug(0),
        m_Alpha(0),
        m_Beta(0),
        m_RC(0),
        m_IsAlpha(false),
        m_IsBeta(false),
        m_IsRC(false)
{
//    int tilde = version.indexOf("~");
    int dotCount = version.count(".");
    int dot = 0;
    int nextDot = 0;
    if (dotCount<1) {
        LOG_ERROR_FOR("VersionNumber", "No major version detected");
    } else {
        dot = version.indexOf(".");
        m_Major = version.left(dot).toInt();
        ++dot;
        nextDot = version.indexOf(".", dot);
        m_Minor = version.mid(dot,nextDot-dot).toInt();
        dot = nextDot + 1;
        nextDot = version.indexOf(QRegExp("\\D"), dot);
        if (nextDot!=-1) {
            m_Debug = version.mid(dot,nextDot-dot).toInt();
        } else {
            m_Debug = version.mid(dot).toInt();
            nextDot = dot;
        }
    }
    if (version.contains("alpha", Qt::CaseInsensitive)) {
        m_IsAlpha = true;
        m_Alpha = version.mid(version.indexOf("alpha", Qt::CaseInsensitive) + 5).toInt();
    } else if (version.contains("beta", Qt::CaseInsensitive)) {
        m_IsBeta = true;
        m_Beta = version.mid(version.indexOf("beta", Qt::CaseInsensitive) + 4).toInt();
    } else if (version.contains("rc", Qt::CaseInsensitive)) {
        m_IsRC = true;
        m_RC = version.mid(version.indexOf("rc", Qt::CaseInsensitive) + 2).toInt();
    }
}

bool VersionNumber::operator>(const VersionNumber &b) const
{
    if (m_Major > b.majorNumber()) {
        return true;
    } else if (m_Major == b.majorNumber()) {
        if (m_Minor > b.minorNumber()) {
            return true;
        } else if (m_Minor == b.minorNumber()) {
            if (m_Debug > b.debugNumber()) {
                return true;
            } else if (m_Debug==b.debugNumber()) {
                // Check sub versions
                if (m_IsAlpha) {
                    if (b.isAlpha()) {
                        if (m_Alpha > b.alphaNumber()) {
                            return true;
                        }
                    }
                }

                if (m_IsBeta) {
                    if (b.isAlpha())
                        return true;
                    if (b.isBeta() && m_Beta > b.betaNumber())
                        return true;
                }

                if (m_IsRC) {
                    if (b.isAlpha() || b.isBeta())
                        return true;
                    if (m_RC > b.rcNumber())
                        return true;
                }
            }
        }
    }
    return false;
}

bool VersionNumber::operator<(const VersionNumber &b) const
{
    return (b > *this);
}

bool VersionNumber::operator>=(const VersionNumber &b) const
{
    return (*this > b || *this == b);
}

bool VersionNumber::operator<=(const VersionNumber &b) const
{
    return (*this < b || *this == b);
}

bool VersionNumber::operator==(const VersionNumber &b) const
{
    if (m_Major==b.majorNumber() && m_Minor==b.minorNumber() && m_Debug==b.debugNumber()) {
        if ((m_IsAlpha && b.isAlpha()) && (m_Alpha==b.alphaNumber())) {
            return true;
        } else if ((m_IsBeta && b.isBeta()) && (m_Beta==b.betaNumber())) {
            return true;
        } else if ((m_IsRC && b.isRC()) && (m_RC==b.rcNumber())) {
            return true;
        }
    }
    return false;
}

bool VersionNumber::operator!=(const VersionNumber &b) const
{
    return (!(*this == b));
}

QDebug operator<<(QDebug dbg, const Utils::VersionNumber &c)
{
    QString t = QString("VersionNumber(maj:%1; min%2; deb:%3").arg(c.majorNumber()).arg(c.minorNumber()).arg(c.debugNumber());
    if (c.isAlpha())
        t += "; alpha:" + QString::number(c.alphaNumber());
    if (c.isBeta())
        t += "; beta:" + QString::number(c.betaNumber());
    t += ")";
    dbg.nospace() << t;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Utils::VersionNumber *c)
{
    if (!c) {
        dbg.nospace() << "Utils::VersionNumber(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
