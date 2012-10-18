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
    explicit AddRemoveComboBox(const QString &labelText, QWidget *parent = 0);
    void initialize();
    ~AddRemoveComboBox();

    void setLabel(const QString &text);

    /*! Sets the internal model of the ComboBox. */
    inline void setModel(QAbstractItemModel *model) { Q_ASSERT(mCombo); mCombo->setModel(model); }

    /*! Sets the model column that is shown in the ComboBox. */
    inline void setModelColumn(int column) { Q_ASSERT(mCombo); mCombo->setModelColumn(column); }

    /*! returns the currentIndex() of the ComboBox model. */
    inline int currentIndex() const { Q_ASSERT(mCombo); return  mCombo->currentIndex(); }

    /*! sets the currentIndex() of the ComboBox model. */
    inline void setCurrentIndex(int index) { Q_ASSERT(mCombo); mCombo->setCurrentIndex(index); }

    void changeEvent(QEvent *e);

Q_SIGNALS:
    void aboutToAddItem();                              ///< emitted before Item add
    void itemAdded(const QModelIndex &index);           ///< item is successfully added
    void aboutToRemoveItem(const QModelIndex &index);   ///< emitted before Item removal
    void itemRemoved();                                 ///< the Item is removed
    void currentIndexChanged(int index);                ///< change the current index


public Q_SLOTS:
    void setEditText(const QString &text);

    void addItem();
    void removeItem();

private Q_SLOTS:
    void updateUi();

private:
    QLabel *mLabel;
    QComboBox *mCombo;
    QPushButton *mAddButton;
    QPushButton *mRemoveButton;
};

} // end namespace Views
#endif // ADDREMOVECOMBOBOX_H
