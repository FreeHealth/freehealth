#include "receiptviewer.h"
#include "ui_receiptviewer.h"

#include <utils/widgets/spinboxdelegate.h>

#include <QAbstractItemModel>

#include <QDebug>
#include <QMessageBox>

namespace Internal {

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
            Row_Visa,
            Row_Cheque,
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

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        {
            if (!index.isValid())
                return QVariant();

            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                switch (index.column()) {
                case Col_Value: return m_Values[index.row()];
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




ReceiptViewer::ReceiptViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptViewer)
{
    ui->setupUi(this);
    ui->amountsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->amountsView->setModel(new Internal::AmountModel(this));
    ui->amountsView->setItemDelegateForColumn(0, new Utils::SpinBoxDelegate(this));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->deleteLineButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
        

}

ReceiptViewer::~ReceiptViewer()
{
    delete ui;
}

void ReceiptViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ReceiptViewer::deleteLine(){
    QMessageBox::information(0,"try","delete line",QMessageBox::Ok);
}
