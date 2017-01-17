/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "usercalendardelegatesmapper.h"
#include "usercalendarmodel.h"
#include "usercalendar.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <calendar/modelanditem/calendar_people.h>
#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>

#include "ui_usercalendardelegatesmapper.h"

#include <QStyledItemDelegate>
#include <QAbstractTableModel>
#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace {

class TreeItemDelegate : public QStyledItemDelegate
{
public:
    TreeItemDelegate(QObject *parent) : QStyledItemDelegate(parent), m_FancyColumn(-1)
    {
    }

    void setModel(QAbstractItemModel *model) {m_Model = model;}
    void setFancyColumn(int col) {m_FancyColumn=col;}
    int fancyColumn() {return m_FancyColumn;}

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        if (m_FancyColumn==-1) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        if (option.state & QStyle::State_MouseOver) {
            if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
                pressedIndex = QModelIndex();
            QBrush brush = option.palette.alternateBase();
            if (index == pressedIndex)
                brush = option.palette.dark();
            painter->fillRect(option.rect, brush);
        }

        QStyledItemDelegate::paint(painter, option, index);

        if (index.column()==m_FancyColumn && option.state & QStyle::State_MouseOver) {
            QIcon icon;
            if (option.state & QStyle::State_Selected) {
                icon = theme()->icon(Core::Constants::ICONCLOSELIGHT);
            } else {
                icon = theme()->icon(Core::Constants::ICONCLOSEDARK);
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
    int m_FancyColumn;

};

}  // End anonymous namespace


UserCalendarDelegatesMapperWidget::UserCalendarDelegatesMapperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::UserCalendarDelegatesMapperWidget),
    m_PeopleModel(0),
    m_Row(-1)
{
    ui->setupUi(this);
    TreeItemDelegate *delegate = new ::TreeItemDelegate(this);
    ui->delegateUserView->viewport()->setAttribute(Qt::WA_Hover);
    ui->delegateUserView->setItemDelegate(delegate);
    ui->delegateUserView->setFrameStyle(QFrame::NoFrame);
    ui->delegateUserView->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->delegateUserView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->delegateUserView->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_PeopleModel = new Calendar::CalendarPeopleModel(this);
    ui->delegateUserView->setModel(m_PeopleModel);
    ui->delegateUserView->header()->setStretchLastSection(false);
    ui->delegateUserView->header()->setSectionResizeMode(Calendar::CalendarPeopleModel::FullName, QHeaderView::Stretch);
    ui->delegateUserView->header()->setSectionResizeMode(Calendar::CalendarPeopleModel::EmptyColumn, QHeaderView::Fixed);
    ui->delegateUserView->hideColumn(Calendar::CalendarPeopleModel::Uid);
    ui->delegateUserView->hideColumn(Calendar::CalendarPeopleModel::PeopleTypeName);
    ui->delegateUserView->setColumnWidth(Calendar::CalendarPeopleModel::EmptyColumn, 16);
    ui->delegateUserView->header()->hide();

    delegate->setModel(m_PeopleModel);
    delegate->setFancyColumn(Calendar::CalendarPeopleModel::EmptyColumn);

    connect(ui->delegateUserView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(ui->delegateUserView, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));

    connect(ui->searchUser, SIGNAL(selectedUser(QString,QString)), this, SLOT(onPersonSelected(QString,QString)));
}

UserCalendarDelegatesMapperWidget::~UserCalendarDelegatesMapperWidget()
{
    delete ui;
}

void UserCalendarDelegatesMapperWidget::setUserCalendarModel(UserCalendarModel *model)
{
    m_UserCalendarModel = model;
    clear();
}

void UserCalendarDelegatesMapperWidget::setUserCalendarIndex(const int index)
{
    Q_ASSERT(m_UserCalendarModel);
    clear();
    m_Row = index;
    // get peoples
    UserCalendar *u = m_UserCalendarModel->userCalendarAt(index);
    if (!u)
        return;
    m_PeopleModel->setPeopleList(u->peopleList());
}

bool UserCalendarDelegatesMapperWidget::submit()
{
    if (m_UserCalendarModel) {
        m_UserCalendarModel->setPeopleList(m_Row, m_PeopleModel->peopleList());
    }
    return true;
}

void UserCalendarDelegatesMapperWidget::clear()
{
    m_PeopleModel->clear();
    ui->searchUser->clear();
    m_PeopleModel->clear();
    m_Row = -1;
}

void UserCalendarDelegatesMapperWidget::addRow(const QString &name, const QString &uid)
{
    m_PeopleModel->addPeople(Calendar::People(Calendar::People::PeopleUserDelegate, name, uid));
}

void UserCalendarDelegatesMapperWidget::removePerson(QAction *action)
{
    m_PeopleModel->removePeople(action->data().toString());
}

void UserCalendarDelegatesMapperWidget::onPersonSelected(const QString &name, const QString &uid)
{
    addRow(name, uid);
    ui->searchUser->clear();
}

void UserCalendarDelegatesMapperWidget::handlePressed(const QModelIndex &index)
{
    if (index.column() == Calendar::CalendarPeopleModel::EmptyColumn) {
        TreeItemDelegate *delegate = static_cast<TreeItemDelegate*>(ui->delegateUserView->itemDelegate());
        if (delegate)
            delegate->pressedIndex = index;
    }
}

void UserCalendarDelegatesMapperWidget::handleClicked(const QModelIndex &index)
{
    if (index.column() == Calendar::CalendarPeopleModel::EmptyColumn) { // the funky button
        // remove the string from the model
        ui->delegateUserView->model()->removeRow(index.row());

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QWidget *vp = ui->delegateUserView->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    }
}

//#include "usercalendardelegatesmapper.moc"
