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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class UserPlugin::UserRightsWidget
  \brief This widget is specifically developed for UserViewer rights datamapper.
  The UserPlugin::UserRightsWidget is connected to the UserPlugin::UserRightsModel.
  It receives the user rights using setRights(). The widget property \e rights
  is defined to be used with any QDataWidgetMapper.
  \sa UserPlugin::UserViewer
*/

#include "userrightswidget.h"

#include <coreplugin/iuser.h>

#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>

using namespace UserPlugin::Internal;

UserRightsModel::UserRightsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    retranslate();
    m_NameToRole.insert(0, Core::IUser::NoRights);
    m_NameToRole.insert(1, Core::IUser::AllRights);
    m_NameToRole.insert(2, Core::IUser::ReadOwn);
    m_NameToRole.insert(3, Core::IUser::ReadDelegates);
    m_NameToRole.insert(4, Core::IUser::ReadAll);
    m_NameToRole.insert(5, Core::IUser::WriteOwn);
    m_NameToRole.insert(6, Core::IUser::WriteDelegates);
    m_NameToRole.insert(7, Core::IUser::WriteAll);
    m_NameToRole.insert(8, Core::IUser::Print);
    m_NameToRole.insert(9, Core::IUser::Create);
    m_NameToRole.insert(10, Core::IUser::Delete);
}

int UserRightsModel::rowCount(const QModelIndex &) const
{
    return m_RightsName.count();
}

QVariant UserRightsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole) {
        if (index.row() < m_RightsName.count())
            return m_RightsName.at(index.row());
        return QVariant();
    }
    if (role==Qt::CheckStateRole) {
        if (index.row()==1) {
            qDebug() << "index" << index
                     << "m_Rights" << m_Rights
                     << (m_Rights == Core::IUser::AllRights ? Qt::Checked : Qt::Unchecked);
            return m_Rights == Core::IUser::AllRights ? Qt::Checked : Qt::Unchecked;
        }
        if (index.row()==0) { // No Rights
            qDebug() << "index" << index
                     << "m_Rights" << m_Rights
                     << (m_Rights == Core::IUser::NoRights ? Qt::Checked : Qt::Unchecked);
            return m_Rights == 0 ? Qt::Checked : Qt::Unchecked;
        }
        if (m_Rights & m_NameToRole.value(index.row(), 0)) {
            qDebug() << "index" << index
                     << "m_Rights" << m_Rights
                     << "m_NameToRole.value(index.row(), 0)"
                     << m_NameToRole.value(index.row(), 0);
            return Qt::Checked;
        }
        return Qt::Unchecked;
    }

    return QVariant();
}

bool UserRightsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role==Qt::CheckStateRole) {
        QVector<int> roleChanged;
        roleChanged.push_back(Qt::CheckStateRole);

        if (value.toInt() == Qt::Checked) {
            switch (index.row()) {
            case 0 :  // No Rights
            {
                m_Rights = 0;
                Q_EMIT dataChanged(this->index(0,0), this->index(m_NameToRole.count(), 0), roleChanged);
                qDebug() << "row: " << index.row() << "rights: " << m_Rights;
                break;
            }
            case 1 :  // All Rights
            {
                m_Rights = m_NameToRole.value(index.row(), 0);
                qDebug() << "row: " << index.row() << "rights: " << m_Rights;
                Q_EMIT dataChanged(this->index(0,0), this->index(m_NameToRole.count(), 0), roleChanged);
                break;
            }
            default :
            {
                m_Rights |= m_NameToRole.value(index.row(), 0);
                qDebug() << "row: " << index.row() << "rights: " << m_Rights;
                Q_EMIT dataChanged(index, index, roleChanged);
                Q_EMIT dataChanged(this->index(1,0), this->index(1,0), roleChanged);
            }
            }  // End switch
            return true;
        } else {
            switch (index.row()) {
            case 1 :
            {
                m_Rights = 0;
                qDebug() << "row: " << index.row() << "rights: " << m_Rights;
                Q_EMIT dataChanged(this->index(0,0), this->index(m_NameToRole.count(), 0));
                break;
            }
            default :
            {
                m_Rights ^= m_NameToRole.value(index.row(), 0);
                qDebug() << "row: " << index.row() << "rights: " << m_Rights;
                Q_EMIT dataChanged(index, index);
            }
                return true;
            }  // End switch
        }
        return false;
    }

    return false;
}

Qt::ItemFlags UserRightsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

void UserRightsModel::retranslate()
{
    m_RightsName.clear();
    m_RightsName
            << tr("No Rights")
            << tr("All Rights")
            << tr("Can read own data")
            << tr("Can read delegate's data")
            << tr("Can read all data")
            << tr("Can write own data")
            << tr("Can write delegate's data")
            << tr("Can write all data")
            << tr("Can print")
            << tr("Can create new data")
            << tr("Can delete data");
}




UserRightsWidget::UserRightsWidget(QWidget * parent) :
    QListView(parent), m_Model(0)
{
    static int handle = 0;
    ++handle;
    setObjectName("UserRightsWidget_"+QString::number(handle));
    setModel(m_Model = new UserRightsModel(this));
}

void UserRightsWidget::changeEvent(QEvent *e)
{
    if ((e->type() == QEvent::LanguageChange))
        if (m_Model)
            m_Model->retranslate();
}
