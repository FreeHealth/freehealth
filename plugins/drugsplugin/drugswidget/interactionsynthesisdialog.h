#ifndef INTERACTIONSYNTHESISDIALOG_H
#define INTERACTIONSYNTHESISDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QTableWidgetItem;
class QModelIndex;
QT_END_NAMESPACE

namespace DrugsWidget {
namespace Internal {
class InteractionSynthesisDialogPrivate;
}

namespace Ui {
    class InteractionSynthesisDialog;
}

class InteractionSynthesisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InteractionSynthesisDialog(QWidget *parent = 0);
    ~InteractionSynthesisDialog();

protected Q_SLOTS:
    void levelActivated(QAction *a);
    void interactorsActivated(const QModelIndex &current, const QModelIndex &previous);
    void interactorsActivated(QTableWidgetItem *item);
    void print();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InteractionSynthesisDialog *ui;
    Internal::InteractionSynthesisDialogPrivate *d;
};


}  // End namespace DrugsWidget

#endif // INTERACTIONSYNTHESISDIALOG_H
