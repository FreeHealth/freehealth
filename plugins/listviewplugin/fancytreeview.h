#ifndef FANCYTREEVIEW_H
#define FANCYTREEVIEW_H

#include <QWidget>

namespace Ui {
    class FancyTreeView;
}

class FancyTreeView : public QWidget
{
    Q_OBJECT

public:
    explicit FancyTreeView(QWidget *parent = 0);
    ~FancyTreeView();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FancyTreeView *ui;
};

#endif // FANCYTREEVIEW_H
