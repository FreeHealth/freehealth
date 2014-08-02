/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
      - major.minor.debug-alpha
      - major.minor.debug-alphaalpha_nb
      - major.minor.debug-beta
      - major.minor.debug-betabeta_nb
      - major.minor.debug-RC
      - major.minor.debug-RCrc_nb
 * \note Unit-test available (see: tests/auto/auto.pro)
 * \sa http://semver.org/
*/

using namespace Utils;

VersionNumber::VersionNumber() :
    m_Major(0),
    m_Minor(0),
    m_Debug(0),
    m_Alpha(0),
    m_Beta(0),
    m_RC(0),
    m_IsAlpha(false),
    m_IsBeta(false),
    m_IsRC(false)
{}

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
    bool ok;
    if (version.compare("test", Qt::CaseInsensitive) != 0 && !version.isEmpty()) {
        if (dotCount<1) {
            LOG_ERROR_FOR("VersionNumber", "Unknown/invalid version number detected: " + version);
        } else {
            int dot = 0;
            int nextDot = 0;
            dot = version.indexOf(".");
            m_Major = version.left(dot).toInt(&ok);
            if (!ok)
                LOG_ERROR_FOR("VersionNumber", "Invalid major version detected: " + version.left(dot));
            ++dot;
            nextDot = version.indexOf(".", dot);
            m_Minor = version.mid(dot,nextDot-dot).toInt(&ok);
            if (!ok)
                LOG_ERROR_FOR("VersionNumber", "Invalid minor version detected: "+ version.mid(dot,nextDot-dot));
            dot = nextDot + 1;
            // get next non-digit character
            nextDot = version.indexOf(QRegExp("\\D"), dot);
            if (nextDot!=-1) {
                // there is a non-digit character in the string
                m_Debug = version.mid(dot,nextDot-dot).toInt(&ok);
                if (!ok)
                    LOG_ERROR_FOR("VersionNumber", "Invalid debug version detected: " + version.mid(dot,nextDot-dot));
            } else {
                // only digits
                m_Debug = version.mid(dot).toInt();
                // nextDot = dot;
            }
        }
    }
    if (version.contains(QRegExp("alpha\\d*", Qt::CaseInsensitive))) {
        m_IsAlpha = true;
        m_Alpha = version.mid(version.indexOf(QRegExp("alpha\\d*", Qt::CaseInsensitive)) + 5).toInt(&ok);
    } else if (version.contains(QRegExp("beta\\d*", Qt::CaseInsensitive))) {
        m_IsBeta = true;
        m_Beta = version.mid(version.indexOf(QRegExp("beta\\d*", Qt::CaseInsensitive)) + 4).toInt(&ok);
    } else if (version.contains(QRegExp("rc\\d*", Qt::CaseInsensitive))) {
        m_IsRC = true;
        m_RC = version.mid(version.indexOf(QRegExp("rc\\d*", Qt::CaseInsensitive)) + 2).toInt(&ok);
    }
}

bool VersionNumber::operator>(const VersionNumber &b) const
{
    if (m_Major > b.majorNumber())
        return true;
    if (m_Major < b.majorNumber())
        return false;

    // here: m_Major == b.majorNumber()
    if (m_Minor > b.minorNumber())
        return true;
    if (m_Minor < b.minorNumber())
        return false;

    // here: m_Major == b.majorNumber()  &&  m_Minor == b.minorNumber()
    if (m_Debug > b.debugNumber())
        return true;
    if (m_Debug < b.debugNumber())
        return false;

    // here: m_Major == b.majorNumber()  &&  m_Minor == b.minorNumber()  &&  m_Debug==b.debugNumber()

    // Major > alpha, beta, rc
    if (!m_IsAlpha && !m_IsBeta && !m_IsRC) {
        if (b.isAlpha() || b.isBeta() || b.isRC())
            return true;
    }
    if (!b.isAlpha() && !b.isBeta() && !b.isRC()) {
        if (m_IsAlpha || m_IsBeta || m_IsRC)
            return false;
    }

    // Check sub versions; Create a compact number
    quint32 compact =  0; // uint max 4 294 967 295
    int rc   = 10000000;
    int beta =    10000;
    int alpha =       1;
    m_IsRC ? compact += (m_RC+1)*rc : compact += m_RC*rc;
    m_IsBeta ? compact += (m_Beta+1)*beta : compact += m_Beta*beta;
    m_IsAlpha ? compact += (m_Alpha+1)*alpha : compact += m_Alpha*alpha;

    quint32 othercompact =  0; // uint max 4 294 967 295
    b.isRC() ? othercompact += (b.rcNumber()+1)*rc : othercompact += b.rcNumber()*rc;
    b.isBeta() ? othercompact += (b.betaNumber()+1)*beta : othercompact += b.betaNumber()*beta;
    b.isAlpha() ? othercompact += (b.alphaNumber()+1)*alpha : othercompact += b.alphaNumber()*alpha;
    return compact > othercompact;
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
        if (m_IsAlpha && b.isAlpha())
            return m_Alpha==b.alphaNumber();
        if (m_IsBeta && b.isBeta())
            return m_Beta==b.betaNumber();
        if (m_IsRC && b.isRC())
            return m_RC==b.rcNumber();
    }
    return false;
}

bool VersionNumber::operator!=(const VersionNumber &b) const
{
    return (!(*this == b));
}

QDebug operator<<(QDebug dbg, const Utils::VersionNumber &c)
{
    QString t = QString("VersionNumber(%1.%2.%3").arg(c.majorNumber()).arg(c.minorNumber()).arg(c.debugNumber());
    if (c.isAlpha())
        t += "-alpha:" + QString::number(c.alphaNumber());
    if (c.isBeta())
        t += "-beta:" + QString::number(c.betaNumber());
    if (c.isRC())
        t += "-RC:" + QString::number(c.rcNumber());
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
