/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developer : Christian A. Reiter <christian.a.reiter@gmail.com>   *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pixmapdelegate.h"
#include <pixmapbutton.h>

#include <QDebug>

#include <coreplugin/ipatient.h>
#include <QBuffer>

using namespace Patients::Internal;


PixmapDelegate::PixmapDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

/*!
 * \brief setEditorData sets the user picture to the given QPushButton widget.
 *
 * If there is no pixmap available, it displays a gendered default pixmap.
 * \param editor Widget where to display, must be a \sa QPushButton
 * \param index QModelIndex of the patient
 */
void PixmapDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == Core::IPatient::Photo_64x64) {
        PixmapButton* button = qobject_cast<PixmapButton*>(editor);
        if(button) {
            QImage image;
            image.loadFromData(index.data(Qt::EditRole).toByteArray());
            QPixmap pixmap = QPixmap::fromImage(image);
            qDebug() << "pixmap.isNull():" << pixmap.isNull();

            button->setPixmap(pixmap);
        }
    } else
        QItemDelegate::setEditorData(editor, index);
}

void PixmapDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    if (index.column() == Core::IPatient::Photo_64x64) {
        PixmapButton* button = qobject_cast<PixmapButton*>(editor);
        if (button) {
            QBuffer buf;
            buf.open(QIODevice::WriteOnly);
            if (button->pixmap().save(&buf))
                model->setData(index, buf.data(), Qt::EditRole);
        }
    } else
        QItemDelegate::setModelData(editor, model, index);
}
