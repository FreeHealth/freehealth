#ifndef INTERACTIONSYNTHESISDIALOG_H
#define INTERACTIONSYNTHESISDIALOG_H

#include <QDialog>
class QTableWidgetItem;

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
    void interactorsActivated(QTableWidgetItem *item);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InteractionSynthesisDialog *ui;
    Internal::InteractionSynthesisDialogPrivate *d;
};


}  // End namespace DrugsWidget

#endif // INTERACTIONSYNTHESISDIALOG_H
