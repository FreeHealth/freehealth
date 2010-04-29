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
#ifndef IFORMITEMSPEC_H
#define IFORMITEMSPEC_H

#include <formmanagerplugin/formmanager_exporter.h>

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
 * \file iformitem.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/

namespace Form {
class IFormWidget;

class FORM_EXPORT FormItemIdentifiants : public QObject
{
public:
    FormItemIdentifiants(QObject *parent =0) : QObject(parent) {}
    ~FormItemIdentifiants() {}

    void setUuid(const QString &uuid);
    int uuid() const;

    // define operator< or > or == for the sorting method of items
private:
    QString m_Name;
    QString m_Uuid;
    int id;
};

namespace Internal{
class FormItemSpecPrivate;
}

class FORM_EXPORT FormItemSpec
{
public:
    enum {
        Spec_Author = 0,
        Spec_License,
        Spec_Category,
        Spec_CreationDate,
        Spec_LastModified,
        Spec_Version,
        Spec_Description,
        Spec_Bibliography,
        Spec_Label,
        Spec_Plugin,
        Spec_IconFileName
    };
    FormItemSpec();
    ~FormItemSpec();

    void setValue(int type, const QVariant &val, const QString &lang = QString(Trans::Constants::ALL_LANGUAGE));
    QVariant value(const int type, const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const;

    QString author(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const { return value(Spec_Author,lang).toString(); }
    QString license(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_License,lang).toString();}
    QString category(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const { return value(Spec_Category,lang).toString();}
    QDateTime creationDate(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_CreationDate,lang).toDateTime();}
    QDateTime modificationDate(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_LastModified,lang).toDateTime();}
    QString version(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_Version,lang).toString();}
    QString description(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_Description,lang).toString();}
    QString bibliography(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_Bibliography,lang).toString();}
    QString label(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_Label,lang).toString();}
    QString pluginName(const QString &lang = QString(Trans::Constants::ALL_LANGUAGE)) const {return value(Spec_Plugin,lang).toString();}

    void toTreeWidget(QTreeWidgetItem *tree) const;

private:
    Form::Internal::FormItemSpecPrivate *d;
};


} // end Form

#endif // IFORMITEMSPEC_H
