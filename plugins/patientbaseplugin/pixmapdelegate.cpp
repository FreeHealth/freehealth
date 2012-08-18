#include "pixmapdelegate.h"
//#include <QPushButton>
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
