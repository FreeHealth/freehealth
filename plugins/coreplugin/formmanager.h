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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <coreplugin/core_exporter.h>
#include <QObject>

/**
 * \file formmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 21 Aug 2009
*/

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class QStackedLayout;
QT_END_NAMESPACE

namespace Core {
class UniqueIDManager;
class FormMain;
class FormItem;
}

namespace Core {
namespace Internal {
class FormManagerPrivate;
}

class CORE_EXPORT FormManager : public QObject
{
     Q_OBJECT
public:
     FormManager(QObject *parent = 0);
     ~FormManager();

     UniqueIDManager *uuidManager() const;
     FormMain *createForm(const QString &uuid, FormMain *parent = 0);

     QList<FormMain*> forms() const;
     bool hasForm(const QString &uuid) const;
     FormMain *form(const QString &uuid) const;

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

     // Uis members
     QTreeWidget *formsTreeWidget(QWidget *parent = 0) const;
     QStackedLayout *formsStackedLayout(QWidget *parent = 0) const;

public Q_SLOTS:
     bool translateForms();
     bool setFormObjects(QObject *root); // will be casted to mfObject while creating new class FormWidget
     void changeStackedLayoutTo(QTreeWidgetItem *item);


 private:
     Internal::FormManagerPrivate *d;

//private:
//     enum ForTreeView { LabelColumn = 0, StateColumn, MaxColumn };
//
//     mfAbstractWidget * getWidget( mfObject * mfo, mfAbstractWidget * parent);
//
//     void createForm( mfObject * mfo, QTreeWidgetItem * item = 0 );
//     void createActions();
//     void createToolBar();
//     void retranslateUi();
//     void changeEvent( QEvent * event );
//     void translateTreeItem( QTreeWidgetItem * item );
//     bool previousIsPossible() const;
//     bool nextIsPossible() const;
//
//
//private slots:
//     void treeToStack( QTreeWidgetItem * );
//     void createTreePopup( const QPoint & );
//     void addNewFile();
//     void removeSubForm();
//     void hideShowTree();
//     void gotoPrevious();
//     void gotoNext();
//     void aboutToShowAddMenu();
//     void addRootForm();
//
//private:
//     mfObject *              m_RootObject;
//     QWidget *               m_Parent;
//
//     QTreeWidgetItem *       m_Item;              /*!< the id in m_FormList / m_Stack is stored into
//                                                       data(LabelColumn, Qt::UserRole) */
//     QWidget *               m_WidgetToStack;
//     QList<mfObject *>       m_FormList;          /*!< contains the mfObject linked to treeitems */
//     QList<QTreeWidgetItem*> m_FormNavList;
//     int                     m_FormNavOffset;
//     bool                    m_FormNavRecord;
//
//     QMenu *                 m_Pop;               /*!< Popup menu for the treewidget */
//     QAction *addFileAct, *removeSubFormAct,
//             *hideShowTreeAct,
//             *previousAct,
//             *nextAct;
//
//     QMenu *                 m_addMenu;
//     QToolBar *              m_NavBar;
//
//     WidgetPlugins           m_widgetPlugins;
};

} // End Core

#endif  // FORMMANAGER_H
