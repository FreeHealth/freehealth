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
#ifndef FORMPLACEHOLDER_H
#define FORMPLACEHOLDER_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QWidget>
#include <QTreeView>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class  QTreeWidgetItem;
class QStackedLayout;
class QModelIndex;
QT_END_NAMESPACE

namespace Form {
class EpisodeModel;

namespace Internal {

class FormPlaceHolderPrivate;

class FormItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FormItemDelegate(QObject *parent = 0);
    void setEpisodeModel(EpisodeModel *model);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    mutable QModelIndex pressedIndex;
    EpisodeModel *m_EpisodeModel;
};
}  // End namespace Internal


class FORM_EXPORT FormPlaceHolder : public QWidget
{
    Q_OBJECT
    friend class FormManager;

public:
    FormPlaceHolder(QWidget *parent = 0);
    ~FormPlaceHolder();

    void setEpisodeModel(EpisodeModel *model);

    QTreeView *formTree() const;
    QStackedLayout *formStackLayout() const;
    
    void addTopWidget(QWidget *top);
    void addBottomWidget(QWidget *bottom);

public Q_SLOTS:
    void setCurrentForm(const QString &formUuid);
    void setCurrentEpisode(const QModelIndex &index);

protected Q_SLOTS:
    void reset();
    void newEpisode();
    void removeEpisode();
    void validateEpisode();
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);

private:
    Internal::FormPlaceHolderPrivate *d;
};

}  // End namespace Form

#endif // FORMPLACEHOLDER_H
