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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TEMPLATESVIEW_H
#define TEMPLATESVIEW_H

#include <templatesplugin/templates_exporter.h>

#include <QtGui/QWidget>
#include <QItemSelectionModel>
#include <QAbstractItemView>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace Templates {
class TemplatesModel;
namespace Internal {
class TemplatesViewPrivate;
class TemplatesViewActionHandler;
}  // End namespace Internal

class TEMPLATES_EXPORT TemplatesView : public QWidget
{
    Q_OBJECT
    friend class Internal::TemplatesViewPrivate;
    friend class Internal::TemplatesViewActionHandler;

public:
    enum ViewContent {
        TemplatesAndCategories = 0,
        CategoriesOnly
    };

    enum EditMode {
        None       = 0x00,
        Add        = 0x01,
        Remove     = 0x02,
        Edit       = 0x04,
        Drag       = 0x08,
        Drop       = 0x10,
        LockUnlock = 0x20,
        Save       = 0x40,
        Print      = 0x80,
        Defaults   = Add | Remove | Edit | Drag | Drop | LockUnlock | Save | Print
    };
    Q_DECLARE_FLAGS(EditModes, EditMode);

    TemplatesView(QWidget *parent = 0, int viewContent = TemplatesAndCategories, EditModes editModes = Defaults);
    ~TemplatesView();

    void setViewContent(int viewContent);
    void setEditMode(EditModes);

    QItemSelectionModel *selectionModel() const;
    bool currentItemIsTemplate() const;
    QModelIndex currentItem() const;

    TemplatesModel *templatesModel() const;
    void expandAll() const;
    void setSelectionMode(QAbstractItemView::SelectionMode mode);


    //    ITemplates *currentTemplate() const;

Q_SIGNALS:
    void currentItemChanged();

protected Q_SLOTS:
    void addCategory();
    void removeItem();
    void editCurrentItem();
    void saveModel();
    void lock(bool state = true);
    void databaseInformations();
    bool printTemplate();

protected:
    void changeEvent(QEvent *e);
    bool isLocked() const;

private:
    Internal::TemplatesViewPrivate *d;
};

}  // End namespace Templates
Q_DECLARE_OPERATORS_FOR_FLAGS(Templates::TemplatesView::EditModes)

#endif // TEMPLATESVIEW_H
