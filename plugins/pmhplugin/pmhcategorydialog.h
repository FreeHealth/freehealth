#ifndef PMHCATEGORYDIALOG_H
#define PMHCATEGORYDIALOG_H

#include <QDialog>

namespace PMH {
class PmhCategoryModel;
namespace Internal {
class PmhCategoryDialogPrivate;
}

class PmhCategoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PmhCategoryDialog(QWidget *parent = 0);

    void setPmhCategoryModel(PmhCategoryModel *model);


public Q_SLOTS:
    void done(int r);

private:
    Internal::PmhCategoryDialogPrivate *d;
};

}  // End namespace PMH

#endif // PMHCATEGORYDIALOG_H
