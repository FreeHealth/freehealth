#include "receiptviewer.h"
#include "receiptsmanager.h"
#include "findReceiptsValues.h"
#include "choiceDialog.h"

#include "constants.h"

#include "ui_receiptviewer.h"

#include <utils/widgets/spinboxdelegate.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <QAbstractItemModel>

#include <QDebug>
#include <QMessageBox>
#include <QFrame>
using namespace Core;
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
using namespace ReceiptsConstants;
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
    ui->amountsView->setModel(new InternalAmount::AmountModel(this));
    ui->amountsView->setItemDelegateForColumn(0, new Utils::SpinBoxDelegate(this));
    ui->dateExecution->setDate(QDate::currentDate());
    ui->datePayment->setDate(QDate::currentDate());
    ui->deleteLineButton->hide();
    ui->inputRadioButton->setChecked(true);
    fillActionTreeView();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->deleteLineButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
    connect(ui->actionsTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(treeViewsActions(const QModelIndex&)));
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

void ReceiptViewer::setPosition(QWidget *parent){
    QRect rect = parent->rect();
    setGeometry(parent->x(),parent->y(),rect.width()-10,rect.height()-10);
}

void ReceiptViewer::deleteLine()
{
    QMessageBox::information(0,"try","delete line",QMessageBox::Ok);
}

void ReceiptViewer::fillActionTreeView(){
    m_actionsTreeModel = new QStandardItemModel;
    QStringList listOfMainActions;
    QMap<QString,QString> parametersMap;
    parametersMap.insert("Debtor","insurance");
    parametersMap.insert("Thesaurus","thesaurus");
    parametersMap.insert("Values","values");
    parametersMap.insert("Sites","sites");
    parametersMap.insert("Preferentiel rate","preferentiel rate");
    listOfMainActions = parametersMap.keys();
    //insert items from tables if available
    QMap<QString,QString> mapSubItems;
    receiptsManager manager;
    QString strKeysParameters;
    foreach(strKeysParameters,listOfMainActions){
        QString table = parametersMap.value(strKeysParameters);
        QStringList listOfItemsOfTable;
        QString null = QString();
        listOfItemsOfTable = manager.getParametersDatas(null,table);
        QString strItemsOfTable;
        foreach(strItemsOfTable,listOfItemsOfTable){
            mapSubItems.insertMulti(strKeysParameters,strItemsOfTable);
        }
        //default values if unavailables :
        if (listOfItemsOfTable.size()<1)
        {
        	  if (strKeysParameters == "Debtor")
        	  {
        	       mapSubItems.insertMulti("Debtor","Patient");
                       mapSubItems.insertMulti("Debtor","CPAM28");  
        	      }
        	  else if (strKeysParameters == "Thesaurus")
        	  {
        	       mapSubItems.insertMulti("Thesaurus","CS");
                       mapSubItems.insertMulti("Thesaurus","V");  
        	      }
        	  else if (strKeysParameters == "Sites")
        	  {
        	       mapSubItems.insertMulti("Sites","cabinet");
                       mapSubItems.insertMulti("Sites","clinique");  
        	      }
        	  else
        	  {
        	       qWarning() << __FILE__ << QString::number(__LINE__) 
        	       << " No default value for "<< strKeysParameters ;
        	       }
            }
    }
    QStandardItem *parentItem = m_actionsTreeModel->invisibleRootItem();
    QString strMainActions;
    foreach(strMainActions,listOfMainActions){
        QStandardItem * actionItem = new QStandardItem(strMainActions);
        parentItem->appendRow(actionItem);
        QStringList listSubActions;
        listSubActions = mapSubItems.values(strMainActions);
        QString strSubActions;
        foreach(strSubActions,listSubActions){
            QStandardItem * subActionItem = new QStandardItem(strSubActions);
            actionItem->appendRow(subActionItem);
        }
    }
    ui->actionsTreeView->setHeaderHidden(true);
    ui->actionsTreeView->setStyleSheet("background-color: silver");
    ui->actionsTreeView->setModel(m_actionsTreeModel);
}

void ReceiptViewer::treeViewsActions(const QModelIndex & index){
    QString data = index.data(Qt::DisplayRole).toString();
    qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data;
    QHash<QString,QString> listOfValues;
    int typeOfPayment = ReceiptsConstants::Cash;
    if(data == "Values"){
        findReceiptsValues * rv = new findReceiptsValues(this);
        choiceDialog choice(rv);
        if(rv->exec() == QDialog::Accepted){
            listOfValues = rv -> getChoosenValues();
            if(listOfValues.keys().size() > 0){
                choice.exec();
                typeOfPayment = choice.returnChoiceDialog();//int
                }
            qDebug() << __FILE__ << QString::number(__LINE__) << " typeOfPayment = "<< QString::number(typeOfPayment);
            }
         }
}
