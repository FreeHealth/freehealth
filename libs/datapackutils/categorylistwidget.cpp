#include "categorylistwidget.h"

QStyleOptionViewItem CategoryListWidget::viewOptions() const {
    QStyleOptionViewItem item = QListWidget::viewOptions();
    item.decorationPosition = QStyleOptionViewItem::Top;
    item.displayAlignment = Qt::AlignHCenter;
    return item;
}
