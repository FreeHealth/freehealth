#ifndef CATEGORY_LIST_WIDGET_H
#define CATEGORY_LIST_WIDGET_H

#include <QListWidget>

class CategoryListWidget : public QListWidget
{
    Q_OBJECT
public:
    CategoryListWidget(QWidget *parent = 0) : QListWidget(parent) {}

    QStyleOptionViewItem viewOptions() const;
};

#endif // CATEGORY_LIST_WIDGET_H
