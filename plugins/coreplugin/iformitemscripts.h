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
#ifndef IFORMITEMSCRIPTS_H
#define IFORMITEMSCRIPTS_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/constants.h>
#include <translationutils/constanttranslations.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QWidget>
#include <QVariant>
#include <QPointer>
#include <QHash>


#include <QDebug>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE


/**
 * \file iformitem.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 27 Aug 2009
*/

namespace Core {

namespace Internal{
class FormItemScriptsPrivate;
}

class CORE_EXPORT FormItemScripts
{
public:
    enum {
        Script_OnLoad = 0,
        Script_PostLoad,
        Script_OnDemand,
        Script_OnValueChanged,
        Script_OnValueRequiered,
        Script_OnDependentValueChanged
    };


    FormItemScripts(const QString &lang = Trans::Constants::ALL_LANGUAGE,
                    const QString &onLoad = QString::null,
                    const QString &postLoad = QString::null,
                    const QString &onDemand = QString::null,
                    const QString &onValChanged = QString::null,
                    const QString &onValRequiered = QString::null,
                    const QString &onDependentValuesChanged = QString::null
                    );
    ~FormItemScripts();

    void setScript(const int type, const QString &script, const QString &lang = Trans::Constants::ALL_LANGUAGE);
    QString script(const int type, const QString &lang = Trans::Constants::ALL_LANGUAGE) const;

    void warn() const;

    QString onLoadScript() const {return script(Script_OnLoad);}
    QString postLoadScript() const {return script(Script_PostLoad);}
    QString onDemandScript() const {return script(Script_OnDemand);}
    QString onValueChangedScript() const {return script(Script_OnValueChanged);}
    QString onValueRequieredScript() const {return script(Script_OnValueRequiered);}
    QString onDependentValueChangedScript() const {return script(Script_OnDependentValueChanged);}

    void toTreeWidget(QTreeWidgetItem *tree);
private:
    Internal::FormItemScriptsPrivate *d;
};


} // end Core

#endif // IFORMITEMSCRIPTS_H
