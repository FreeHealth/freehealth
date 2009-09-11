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
#ifndef IFORMITEMVALUES_H
#define IFORMITEMVALUES_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/idebugpage.h>
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
 * \file iformitemvalues.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 27 Aug 2009
*/

namespace Core {

namespace Internal {
class FormItemValuesPrivate;
}

class CORE_EXPORT FormItemValues : public QObject
{
    Q_OBJECT
public:
    enum {
        Value_Possible = 0,
        Value_Script,
        Value_Numerical,
        Value_Default,
        Value_Dependency
    };
    FormItemValues(QObject *parent);
    ~FormItemValues();

    void setValue(int type, const int id, const QVariant &val, const QString &language = Trans::Constants::ALL_LANGUAGE);
    void setDefaultValue(const QVariant &val, const QString &lang = Trans::Constants::ALL_LANGUAGE);

    // this value represents the one the user's define : text of the LineEdit, value of the spinBox...
//    void setSelectedValue(const QVariant &val);
//    QVariant selectedValue() const;

    bool isOptionnal() const;
    void setOptionnal(bool state);

    QStringList values(const int typeOfValues) const;

    // unique or historic-values possibilities
    // actual selected value(s)

    // filename
    void setFileName(const QString &fileName) {m_FileName=fileName;}
    QString fileName() const {return m_FileName;}

    void toTreeWidget(QTreeWidgetItem *tree) const;

private:
    Internal::FormItemValuesPrivate *d;
    QString m_FileName;
};

} // end Core

#endif // IFORMITEMVALUES_H
