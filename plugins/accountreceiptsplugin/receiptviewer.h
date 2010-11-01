#ifndef RECEIPTVIEWER_H
#define RECEIPTVIEWER_H

#include <QWidget>

namespace Ui {
    class ReceiptViewer;
}

class ReceiptViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptViewer(QWidget *parent = 0);
    ~ReceiptViewer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ReceiptViewer *ui;
};

#endif // RECEIPTVIEWER_H
