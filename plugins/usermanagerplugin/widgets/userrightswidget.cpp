/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
/**
  \class UserRightsWidget
  \brief This widget is specifically develop for UserViewer rights datamapper.
  \ingroup usertoolkit widget_usertoolkit usermanager
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

int UserRightsModel::rowCount(const QModelIndex &parent) const
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
        if (index.row()==1) // All Rights
            return m_Rights == Core::IUser::AllRights ? Qt::Checked : Qt::Unchecked;
        if (index.row()==0) // No Rights
            return m_Rights == 0 ? Qt::Checked : Qt::Unchecked;
        if (m_Rights & m_NameToRole.value(index.row(), 0))
            return Qt::Checked;
        return Qt::Unchecked;
    }

    return QVariant();
}

bool UserRightsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role==Qt::CheckStateRole) {
        if (value.toInt() == Qt::Checked) {
            switch (index.row()) {
            case 0 :  // No Rights
                {
                    m_Rights = 0;
                    Q_EMIT dataChanged(this->index(0,0), this->index(m_NameToRole.count(), 0));
                    break;
                }
            case 1 :  // All Rights
                {
                    m_Rights = m_NameToRole.value(index.row(), 0);
                    Q_EMIT dataChanged(this->index(0,0), this->index(m_NameToRole.count(), 0));
                    break;
                }
            default :
            {
                m_Rights |= m_NameToRole.value(index.row(), 0);
                Q_EMIT dataChanged(index, index);
                Q_EMIT dataChanged(this->index(1,0), this->index(1,0));
            }
        }  // End switch
            return true;
        } else {
            switch (index.row()) {
            case 1 :
                {
                    m_Rights = 0;
                    Q_EMIT dataChanged(this->index(0,0), this->index(m_NameToRole.count(), 0));
                    break;
                }
            default :
            {
                m_Rights ^= m_NameToRole.value(index.row(), 0);
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
            << tr("Can read own datas")
            << tr("Can read delegates datas")
            << tr("Can read all datas")
            << tr("Can write own datas")
            << tr("Can write delegates datas")
            << tr("Can write all datas")
            << tr("Can print")
            << tr("Can create new datas")
            << tr("Can delete datas");
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
