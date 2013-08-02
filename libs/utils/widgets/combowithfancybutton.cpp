/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
        ColumnLabel = 0,
        ColumnMoveUp,
        ColumnMoveDown,
        ColumnRemove
    };

    struct String {
        QString s;
        QVariant userData;
    };

class StringModel : public QStringListModel
{
public:
    StringModel(QObject *parent = 0) : QStringListModel(parent) {}

    int rowCount(const QModelIndex & = QModelIndex()) const {return m_List.count();}
    int columnCount(const QModelIndex & = QModelIndex()) const {return 4;}

    void clear() {m_List.clear();}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() >= m_List.count())
            return QVariant();

        if (index.row() < 0)
            return QVariant();

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (index.column() == 0) {
                return m_List.at(index.row()).s;
            }
        } else if (role == Qt::BackgroundRole) {
            const QString &user = m_List.at(index.row()).userData.toString();
            if (user.startsWith("#")) {
                QColor color(user);
                color.setAlpha(125);
                return color;
            }
        }
        return QVariant();
    }

    void setStringList(const QStringList &strings, const QVariant &userData = QVariant())
    {
        m_List.clear();
        addStringList(strings, userData);
    }

    void addStringList(const QStringList &strings, const QVariant &userData = QVariant())
    {
        beginResetModel();
        foreach(const QString &str, strings) {
            if (str.isEmpty())
                continue;
            String s;
            s.s = str;
            s.userData = userData;
            m_List << s;
        }
        endResetModel();
    }

    QStringList stringList(const QVariant &userData = QVariant()) const
    {
        QStringList list;
        bool all = userData.isNull();
        foreach(const String &str, m_List) {
            if (all)
                list << str.s;
            else if (str.userData == userData)
                list << str.s;
        }
        return list;
    }

    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex())
    {
        beginRemoveRows(parent, row, row+count-1);
        for(int i=0; i < count; ++i) {
            m_List.removeAt(row);
        }
        endRemoveRows();
        return true;
    }

    bool moveUp(const QModelIndex &item)
    {
        if (!item.isValid())
            return false;

        if (item.row() >= 1) {
            m_List.move(item.row(), item.row()-1);
            Q_EMIT dataChanged(index(item.row()-1, 0), item);
            return true;
        }
        return false;
    }

    bool moveDown(const QModelIndex &item)
    {
        if (!item.isValid())
            return false;

        if (item.row() < (rowCount()-1)) {
            m_List.move(item.row(), item.row()+1);
            Q_EMIT dataChanged(index(item.row()+1, 0), item);
            return true;
        }
        return false;
    }

private:
    QList<String> m_List;
};

class ItemDelegate : public QStyledItemDelegate
{
public:
    ItemDelegate(QObject *parent = 0) :
        QStyledItemDelegate(parent),
        m_Model(0)
    {}

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
    QComboBox(parent), delegate(0), view(0), m_Settings(0), m_ignoreHide(false), m_Index(-1)
{
    view = new QTreeView(this);
    view->viewport()->setAttribute(Qt::WA_Hover);
    view->header()->hide();
    view->setFrameStyle(QFrame::NoFrame);
    view->setAttribute(Qt::WA_MacShowFocusRect, false);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setIndentation(0);
    view->setAlternatingRowColors(true);

    delegate = new Internal::ItemDelegate(view);
    stringModel = new Internal::StringModel(this);

    setModel(stringModel);
    setView(view);

    view->header()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    view->header()->setResizeMode(0, QHeaderView::Stretch);
    view->header()->setResizeMode(1, QHeaderView::Fixed);
    view->header()->setResizeMode(2, QHeaderView::Fixed);
    view->header()->setResizeMode(3, QHeaderView::Fixed);
#else
    // Qt5
    view->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    view->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    view->header()->setSectionResizeMode(2, QHeaderView::Fixed);
    view->header()->setSectionResizeMode(3, QHeaderView::Fixed);
#endif

    view->header()->resizeSection(1, 16);
    view->header()->resizeSection(2, 16);
    view->header()->resizeSection(3, 16);

    view->setItemDelegate(delegate);

    connect(view, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));
}

/** Enable/disable the remove button in the popup view */
void ComboWithFancyButton::setRemoveItems(bool state)
{
    Q_ASSERT(view);
    if (!view)
        return;
    view->setColumnHidden(3, !state);
}

/** Enable/disable the move buttons in the popup view */
void ComboWithFancyButton::setMoveItems(bool state)
{
    Q_ASSERT(view);
    if (!view)
        return;
    view->setColumnHidden(2, !state);
    view->setColumnHidden(1, !state);
}

void ComboWithFancyButton::fancyAddItems(const QStringList &list, const QVariant &userData)
{
    stringModel->addStringList(list, userData);
}

void ComboWithFancyButton::fancyAddItem(const QString &text, const QVariant &userData)
{
    stringModel->addStringList(QStringList() << text, userData);
}

/** Return the list of available items in the model used combo */
QStringList ComboWithFancyButton::fancyItems(const QVariant &userData) const
{
    if (stringModel)
        return stringModel->stringList(userData);
    return QStringList();
}

/** Define the icon to use in the popup view */
void ComboWithFancyButton::setRemoveLightIcon(const QIcon &icon)
{
    Q_ASSERT(delegate);
    if (delegate)
        delegate->m_RemoveLight = icon;
}

/** Define the icon to use in the popup view */
void ComboWithFancyButton::setMoveUpLightIcon(const QIcon &icon)
{
    Q_ASSERT(delegate);
    if (delegate)
        delegate->m_MoveUpLight = icon;
}

/** Define the icon to use in the popup view */
void ComboWithFancyButton::setMoveDownLightIcon(const QIcon &icon)
{
    Q_ASSERT(delegate);
    if (delegate)
        delegate->m_MoveDownLight = icon;
}

/** Clear the stringlist model associated with the combobox */
void ComboWithFancyButton::fancyClear()
{
    if (stringModel)
        stringModel->clear();
}

void ComboWithFancyButton::handlePressed(const QModelIndex &index)
{
    switch (index.column()) {
    case Internal::ColumnRemove:
    {
        delegate->pressedIndex = index;
        stringModel->removeRow(index.row());
        m_ignoreHide = true;
        QComboBox::showPopup();
        break;
    }
    case Internal::ColumnMoveUp:
    {
        delegate->pressedIndex = index;
        stringModel->moveUp(index);
        m_ignoreHide = true;
        view->setCurrentIndex(index);
        //            QComboBox::showPopup();
        break;
    }
    case Internal::ColumnMoveDown:
    {
        delegate->pressedIndex = index;
        stringModel->moveDown(index);
        m_ignoreHide = true;
        view->setCurrentIndex(index);
        // QComboBox::showPopup();
        break;
    }
    default:
    {
        // qWarning() << "selected" << index.row();
        setCurrentIndex(index.row());
    }
    }
}

void ComboWithFancyButton::showPopup()
{
    QComboBox::showPopup();
}

void ComboWithFancyButton::hidePopup()
{
    if (m_ignoreHide) {
        m_ignoreHide = false;
    } else {
        QComboBox::hidePopup();
    }
}
