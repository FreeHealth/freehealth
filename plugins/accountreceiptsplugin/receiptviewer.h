#ifndef RECEIPTVIEWER_H
#define RECEIPTVIEWER_H

#include <QWidget>
#include <QStandardItemModel>


namespace Ui {
    class ReceiptViewer;
}

class ReceiptViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptViewer(QWidget *parent = 0);
    ~ReceiptViewer();
    void setPosition(QWidget *parent);

protected:
    void changeEvent(QEvent *e);
    void fillActionTreeView();

private:
    Ui::ReceiptViewer *ui;
    QStandardItemModel *m_actionsTreeModel;

private slots :
    void deleteLine();
    void treeViewsActions(const QModelIndex&);
};

#endif // RECEIPTVIEWER_H
