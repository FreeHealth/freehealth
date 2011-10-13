#ifndef SEARCHATCINDATABASEDIALOG_H
#define SEARCHATCINDATABASEDIALOG_H

#include <QDialog>
class QModelIndex;

namespace DrugsDbCreator {
class SearchAtcInDatabaseDialogPrivate;

namespace Ui {
    class SearchAtcInDatabaseDialog;
}

class SearchAtcInDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchAtcInDatabaseDialog(QWidget *parent = 0, const QString &term = QString::null);
    ~SearchAtcInDatabaseDialog();

    QStringList getSelectedCodes();
    QStringList getSelectedLabels();

private Q_SLOTS:
    void setFilter();
    void on_term_textChanged(const QString &text);
    void on_lang_currentIndexChanged(const QString &text);
    void on_tableView_activated(const QModelIndex &index);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SearchAtcInDatabaseDialog *ui;
    SearchAtcInDatabaseDialogPrivate *d;
};

}  // End namespace DrugsDbCreator

#endif // SEARCHATCINDATABASEDIALOG_H
