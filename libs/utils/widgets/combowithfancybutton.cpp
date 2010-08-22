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
#include "combowithfancybutton.h"

#include <QModelIndex>
#include <QRect>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>
#include <QPainter>
#include <QIcon>
#include <QComboBox>
#include <QTreeView>
#include <QHeaderView>
#include <QStringListModel>
#include <QSettings>

#include <QMouseEvent>
#include <QApplication>

#include <QDebug>

using namespace Utils;

namespace Utils {
namespace Internal {

    enum ColumnRepresentation {
        ColumnMoveUp = 1,
        ColumnMoveDown,
        ColumnRemove
    };

class StringModel : public QStringListModel
{
public:
    StringModel(QObject *parent = 0) : QStringListModel(parent) {}

    int columnCount(const QModelIndex & = QModelIndex()) const {return 4;}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (index.column() < 1)
            return QStringListModel::data(index, role);
        return QVariant();
    }

    bool moveUp(const QModelIndex &index)
    {
        qWarning() << "moveUp";
        if (!index.isValid())
            return false;

        if (index.row() >= 1) {
            QStringList list = stringList();
            list.move(index.row(), index.row()-1);
            setStringList(list);
//            reset();
            return true;
        }
        return false;
    }

    bool moveDown(const QModelIndex &index)
    {
        qWarning() << "moveDown";
        if (!index.isValid())
            return false;

        if (index.row() < (rowCount()-1)) {
            QStringList list = stringList();
            list.move(index.row(), index.row()+1);
            setStringList(list);
//            reset();
            return true;
        }
        return false;
    }

};

class ItemDelegate : public QStyledItemDelegate
{
public:
    ItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}

    void setRemoveLightIcon(const QIcon &icon) {m_RemoveLight = icon;}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        if (option.state & QStyle::State_MouseOver) {
            if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
                pressedIndex = QModelIndex();
            QBrush brush = option.palette.alternateBase();
            if (index == pressedIndex)
                brush = option.palette.dark();
            painter->fillRect(option.rect, brush);
        }

        QStyledItemDelegate::paint(painter, option, index);

        if (option.state & QStyle::State_MouseOver) {
            QIcon icon;
            switch (index.column())
            {
            case ColumnRemove: // Remove
                {
                    icon = m_RemoveLight;
                    break;
                }
            case ColumnMoveDown: // MoveDown
                {
                    icon = m_MoveDownLight;
                    break;
                }
            case ColumnMoveUp: // MoveUp
                {
                    icon = m_MoveUpLight;
                    break;
                }
            }
            QRect iconRect(option.rect.right() - option.rect.height(),
                           option.rect.top(),
                           option.rect.height(),
                           option.rect.height());

            icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
        }
    }

public:
    mutable QModelIndex pressedIndex;
    QAbstractItemModel *m_Model;
    QIcon m_RemoveLight, m_MoveUpLight, m_MoveDownLight;
};

}  // End namespace Internal
}  // End namespace Utils


ComboWithFancyButton::ComboWithFancyButton(QWidget *parent) :
    QComboBox(parent), delegate(0), view(0), m_Settings(0), m_ignoreHide(false)
{
    view = new QTreeView(this);
    view->viewport()->setAttribute(Qt::WA_Hover);
    view->header()->hide();
    view->header()->hide();
    view->setFrameStyle(QFrame::NoFrame);
    view->setAttribute(Qt::WA_MacShowFocusRect, false);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setIndentation(0);

    delegate = new Internal::ItemDelegate(view);
    model = new Internal::StringModel(this);

    setModel(model);
    setView(view);

    view->header()->setStretchLastSection(false);
    view->header()->setResizeMode(0, QHeaderView::Stretch);
    view->header()->setResizeMode(1, QHeaderView::Fixed);
    view->header()->setResizeMode(2, QHeaderView::Fixed);
    view->header()->setResizeMode(3, QHeaderView::Fixed);
    view->header()->resizeSection(1, 16);
    view->header()->resizeSection(2, 16);
    view->header()->resizeSection(3, 16);

    view->setItemDelegate(delegate);

    connect(view, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));
}

void ComboWithFancyButton::setRemoveItems(bool state)
{
    Q_ASSERT(view);
    if (!view)
        return;
    view->setColumnHidden(3, !state);
}

void ComboWithFancyButton::setMoveItems(bool state)
{
    Q_ASSERT(view);
    if (!view)
        return;
    view->setColumnHidden(2, !state);
    view->setColumnHidden(1, !state);
}

void ComboWithFancyButton::addItems(const QStringList &list)
{
    model->setStringList(model->stringList() + list);
}

void ComboWithFancyButton::addItem(const QString &item)
{
    model->insertRow(model->rowCount());
    model->setData(model->index(model->rowCount()-1,0), item);
//    model->setStringList(model->stringList() + item);
}

QStringList ComboWithFancyButton::items() const
{
    if (model)
        return model->stringList();
    return QStringList();
}

void ComboWithFancyButton::saveItemsToSettings()
{
    if (!m_Settings)
        return;
    m_Settings->setValue(m_Key, items());
    m_Settings->sync();
}

void ComboWithFancyButton::setRemoveLightIcon(const QIcon &icon)
{
    Q_ASSERT(delegate);
    if (delegate)
        delegate->m_RemoveLight = icon;
}

void ComboWithFancyButton::setMoveUpLightIcon(const QIcon &icon)
{
    Q_ASSERT(delegate);
    if (delegate)
        delegate->m_MoveUpLight = icon;
}

void ComboWithFancyButton::setMoveDownLightIcon(const QIcon &icon)
{
    Q_ASSERT(delegate);
    if (delegate)
        delegate->m_MoveDownLight = icon;
}

void ComboWithFancyButton::handlePressed(const QModelIndex &index)
{
    qWarning() << "col" << index.column();
    switch (index.column()) {
    case Internal::ColumnRemove:
        {
            qWarning() << "remove";
            delegate->pressedIndex = index;
            model->removeRow(index.row());
            m_ignoreHide = true;
            QComboBox::showPopup();
            break;
        }
    case Internal::ColumnMoveUp:
        {
            delegate->pressedIndex = index;
            model->moveUp(index);
            m_ignoreHide = true;
            view->setCurrentIndex(index);
//            QComboBox::showPopup();
            break;
        }
    case Internal::ColumnMoveDown:
        {
            delegate->pressedIndex = index;
            model->moveDown(index);
            m_ignoreHide = true;
            view->setCurrentIndex(index);
//            QComboBox::showPopup();
            break;
        }
    }
}

void ComboWithFancyButton::showPopup()
{
//    this->setCurrentIndex(0);
//    setRootModelIndex(QModelIndex());
#ifdef Q_OS_MAC
    setEditable(true);
#endif
    QComboBox::showPopup();
}

void ComboWithFancyButton::hidePopup()
{
    if (m_ignoreHide) {
        m_ignoreHide=false;
    } else {
        setRootModelIndex(view->currentIndex().parent());
        setCurrentIndex(view->currentIndex().row());
        QComboBox::hidePopup();
#ifdef Q_OS_MAC
        setEditable(false);
#endif
    }
}
