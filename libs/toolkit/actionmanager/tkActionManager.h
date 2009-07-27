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
 *   This code is inspired of the QtCreator project from Nokia             *
 *   QtCreator : Copyright (c) 2009 Nokia Corporation and/or               *
 *               its subsidiary(-ies).                                     *
 *               GNU Lesser General Public License Usage                   *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKACTIONMANAGER_H
#define TKACTIONMANAGER_H

#include <tkExporter.h>
#include <tkActionContainer.h>
#include <tkCommand.h>

#include <QObject>
#include <QList>

QT_BEGIN_NAMESPACE
class QAction;
class QShortcut;
class QString;
QT_END_NAMESPACE

class tkActionManagerPrivate;

class Q_TK_EXPORT tkActionManager : public QObject
{
    Q_OBJECT
    friend class tkActionContainer;

public:
    // QMainWindow
    static tkActionManager *instance(QMainWindow *win=0);
    virtual ~tkActionManager();

    virtual tkActionContainer *createMenu(const QString &id);
    virtual tkActionContainer *createMenuBar(const QString &id);

    virtual tkCommand *registerAction(QAction *action, const QString &id, const QList<int> &context);
    virtual tkCommand *registerShortcut(QShortcut *shortcut, const QString &id, const QList<int> &context);

    virtual tkCommand *command(const QString &id) const;
    virtual tkActionContainer *actionContainer(const QString &id) const;

//protected:
    void setContext(const QList<int> &context);
    bool hasContext(int context) const;

    tkCommand *command(int uid) const;
    tkActionContainer *actionContainer(int uid) const;
    QList<int> defaultGroups() const;

public Q_SLOTS:
    void retranslateMenusAndActions();

private:
    tkActionManager(QMainWindow *parent);
    tkActionManagerPrivate *d;
    static tkActionManager *m_Instance;
};


#endif // TKACTIONMANAGER_H
