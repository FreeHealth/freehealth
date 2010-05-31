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
#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>

/**
 * \file formmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class QStackedLayout;
QT_END_NAMESPACE

namespace Core {
class UniqueIDManager;
}

namespace Form {
class FormMain;
class FormItem;
class IFormIO;
class FormPlaceHolder;

namespace Internal {
class FormManagerPrivate;
}

class FORM_EXPORT FormManager : public QObject
{
     Q_OBJECT
     FormManager(QObject *parent = 0);

public:
     static FormManager *instance();
     ~FormManager();

     bool loadFile(const QString &filename, const QList<Form::IFormIO *> &iolist);
     void activateMode();

     Core::UniqueIDManager *uuidManager() const;

     FormMain *createForm(const QString &uuid, FormMain *parent = 0);
     QList<FormMain*> forms() const;
     bool hasForm(const QString &uuid) const;
     FormMain *form(const QString &uuid) const;

     FormPlaceHolder *formPlaceHolder() const;

     template <class T>
     T* getParent(FormItem *item)
     {
         T* parent = 0;
         QObject *p = item;
         while (p) {
             parent = qobject_cast<T*>(p);
             if (!parent) {
                 p = p->parent();
             } else {
                 break;
             }
         }
         return parent;
     }

public Q_SLOTS:
     bool translateForms();
     void setCurrentPatient(const QString &uuid);

 private:
     Internal::FormManagerPrivate *d;
     static FormManager *m_Instance;
};

} // End Form

#endif  // FORMMANAGER_H
