#ifndef ICDCODESELECTOR_H
#define ICDCODESELECTOR_H

#include <QWidget>

namespace Ui {
    class IcdCodeSelector;
}

class IcdCodeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit IcdCodeSelector(QWidget *parent = 0);
    ~IcdCodeSelector();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::IcdCodeSelector *ui;
};

#endif // ICDCODESELECTOR_H
