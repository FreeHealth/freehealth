#ifndef RECEIPTVIEWER_H
#define RECEIPTVIEWER_H
#include <utils/widgets/spinboxdelegate.h>
#include <QWidget>
#include <QStandardItemModel>
#include <QHash>
#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QStringListModel>
#include <QTreeView>

namespace Ui {
    class ReceiptViewer;
}

namespace InternalAmount {

    class AmountModel : public QAbstractTableModel
    {
    public:
        enum ColumnRepresentation {
            Col_Value = 0,
            Col_Currency,  // Devise monétaire
            ColCount
        };

        enum RowRepresentation {
            Row_Cash = 0,
            Row_Cheque,
            Row_Visa,
            Row_Banking,  // Virement banquaire
            Row_Other,
            Row_Du,
            RowCount
        };

        AmountModel(QObject *parent = 0) : QAbstractTableModel(parent)
        {
            for(int i=0; i < rowCount(); ++i)
                m_Values.append(0.0);
        }

        int rowCount(const QModelIndex &parent = QModelIndex()) const {return RowCount;}
        int columnCount(const QModelIndex &parent = QModelIndex()) const {return ColCount;}
        
        bool submit(){
            if (!submit()) {
               return false;
            }
          return true;
        }


        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        {
            if (!index.isValid())
                return QVariant();

            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                switch (index.column()) {
                case Col_Value: 
                    return m_Values[index.row()];
                    break;
                default: return QVariant();
                }
            }
            return QVariant();
        }

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
        {
            if (!index.isValid())
                return false;

            if (role==Qt::EditRole) {
                switch (index.column()) {
                case Col_Value:
                    {
                        m_Values[index.row()] = value.toFloat();
                        Q_EMIT dataChanged(index, index);
                        return true;
                    }
                }
            }
            return false;
        }

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
        {
            if (role==Qt::DisplayRole) {
                if (orientation==Qt::Vertical) {
                    switch (section) {
                    case Row_Cash: return tr("Cash");
                    case Row_Visa: return "Visa";
                    case Row_Cheque: return "Cheque";
                    case Row_Banking: return "Banking";
                    case Row_Other: return "Other";
                    case Row_Du: return "Du";
                    }
                } else if (orientation==Qt::Horizontal) {
                    switch (section) {
                    case Col_Value: return "Value";
                    case Col_Currency : return "Currency";
                    }
                }
            }
            return QVariant();
        }
        
        QSqlError lastError(){
            return lastError();
            }

        Qt::ItemFlags flags(const QModelIndex &index) const
        {
            if (index.column()==Col_Value) {
                return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
            } else {
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            }
        }


    private:
        QList<float> m_Values;
    };

}  // End namespace Internal
using namespace InternalAmount;
class treeViewsActions: public QTreeView {
    Q_OBJECT
    public :
        treeViewsActions(QWidget * parent);
        ~treeViewsActions();
        void reset(){QTreeView::reset();}
        void fillActionTreeView();
        QStandardItemModel *m_actionsTreeModel;
    private :
        void mousePressEvent(QMouseEvent * event);
        bool deleteItemFromThesaurus(QModelIndex & index);
};
class ReceiptViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptViewer(QWidget *parent = 0);
    ~ReceiptViewer();
    void setPosition(QWidget *parent);
    QString m_userUuid;
    treeViewsActions *actionTreeView;

protected:
    void changeEvent(QEvent *e);
    void fillModel(QHash<QString,QString> & hashOfValues,int typeOfPayment,double percentage);
    QAction *m_clear;
    QMenu *m_menu;

private:
    Ui::ReceiptViewer *ui;
    QStringListModel *m_modelReturnedList;
    AmountModel * m_model;
    QVariant m_siteUid;
    QVariant m_insuranceUid;
    QStringList m_listOfValues;
    void mousePressEvent(QMouseEvent * event);

private slots :
    void deleteLine();
    void save();
    void saveAndQuit();
    void actionsOfTreeView(const QModelIndex&);
    void saveInThesaurus();
    void clearAll(bool b);
};

#endif // RECEIPTVIEWER_H
