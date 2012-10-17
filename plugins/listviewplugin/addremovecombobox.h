#ifndef ADDREMOVECOMBOBOX_H
#define ADDREMOVECOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QGroupBox>



class QPushButton;
class QLabel;

namespace Views {


class AddRemoveComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit AddRemoveComboBox(QWidget *parent = 0);
    explicit AddRemoveComboBox(const QString &labelText, QGroupBox *box, QWidget *parent = 0);
    void initialize();
    ~AddRemoveComboBox();

    void setLabel(const QString &text);

    inline void setModel(QAbstractItemModel *model) { Q_ASSERT(mCombo); mCombo->setModel(model); }
    inline void setModelColumn(int column) { Q_ASSERT(mCombo); mCombo->setModelColumn(column); }
    inline int currentIndex() const { Q_ASSERT(mCombo); return  mCombo->currentIndex(); }
    inline void setCurrentIndex(int index) { Q_ASSERT(mCombo); mCombo->setCurrentIndex(index); }

    void changeEvent(QEvent *e);

Q_SIGNALS:
    void aboutToAddItem();
    void itemAdded(const QModelIndex &index);
    void aboutToRemoveItem(const QModelIndex &index);
    void itemRemoved();
    void currentIndexChanged(int index);


public Q_SLOTS:
    void setEditText(const QString &text);

    void addItem();
    void removeItem();

private Q_SLOTS:

private:
    QLabel *mLabel;
    QComboBox *mCombo;
    QPushButton *mAddButton;
    QPushButton *mRemoveButton;
};

} // end namespace Views
#endif // ADDREMOVECOMBOBOX_H
