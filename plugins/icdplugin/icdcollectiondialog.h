#ifndef ICDCOLLECTIONDIALOG_H
#define ICDCOLLECTIONDIALOG_H

#include <QDialog>

namespace ICD {
class IcdCollectionModel;
class IcdCentralWidget;

class IcdCollectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IcdCollectionDialog(QWidget *parent = 0);
    ~IcdCollectionDialog();

    void setIcdCollectionModel(IcdCollectionModel *model);
    void setXmlIcdCollection(const QString &xml);
    QString xmlIcdCollection();

public Q_SLOTS:
    void done(int r);

private:
    IcdCollectionModel *m_CollectionModel;
    IcdCentralWidget *m_View;
};

}  // End namespace ICD


#endif // ICDCOLLECTIONDIALOG_H
