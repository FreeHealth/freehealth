#ifndef PIXMAPDELEGATE_H
#define PIXMAPDELEGATE_H

#include <QItemDelegate>

namespace Patients {
namespace Internal {


/*!
 * \brief The PixmapDelegate class
 *
 * Responsible for displaying a pixmap onto the IdentityWidget's QPushButton.
 * If there is no pixmap available, it displays a default picture according to the
 * selected patient's gender.
 */
class PixmapDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit PixmapDelegate(QObject *parent = 0);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

} // end Internal
} // end Patients

#endif // PIXMAPDELEGATE_H
