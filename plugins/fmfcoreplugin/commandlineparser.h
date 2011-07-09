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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FREEMEDFORMS_COMMANDLINEPARSER_H
#define FREEMEDFORMS_COMMANDLINEPARSER_H

#include <coreplugin/icommandline.h>

#include <QHash>
#include <QString>
#include <QApplication>
#include <QDebug>

namespace Core {
class CommandLine  : public Core::ICommandLine
{
public:
    enum Param {
        CL_Chrono,
        CL_TransmitDosage,
        CL_ConfigFile,
        CL_RunningUnderWine,
        CL_MaxParam
    };

    CommandLine() : ICommandLine()
    {
        ref.insert(CL_Chrono, "--chrono");
        ref.insert(CL_TransmitDosage, "--transmit-dosage");
        ref.insert(CL_ConfigFile, "--config");
        ref.insert(CL_RunningUnderWine, "--wine");
        const QStringList &args = qApp->arguments();
        foreach(const QString &a, args) {
            QString k = a;
            if (k.contains(" "))
                k = k.left(k.indexOf(" "));
            if (a.contains("="))
                k = k.left(k.indexOf("="));
            switch (ref.key(k,-1))
            {
                case CL_Chrono : params.insert(CL_Chrono, true); break;
                case CL_TransmitDosage : params.insert(CL_TransmitDosage, true); break;
                case CL_ConfigFile : params.insert(CL_ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_RunningUnderWine : params.insert(CL_RunningUnderWine, true); break;
                default : break;
            }
        }
    }

    QVariant value(int param, const QVariant &def = QVariant()) const
    {
        return params.value(param,def);
    }

    QString paramName(int param) const
    {
        if (ref.keys().contains(param))
            return ref.value(param);
        else
            return QString::number(param);
    }

private:
    QHash<int,QVariant> params;
    QHash<int, QString> ref;
};

}

#endif // FREEMEDFORMS_COMMANDLINEPARSER_H
