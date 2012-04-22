#include "nextdateitemsdialog.h"
//#include "preventionconstants.h"
#include "IPrevention.h"


//using namespace PreventionConstants;

NextDateDialog::NextDateDialog(const QObject * parent , 
                               QStringList & listOfItems, 
                               QSqlTableModel * nextDateModel)
{
    Q_UNUSED(parent);
    Q_UNUSED(listOfItems);
    setupUi(this);
    m_model = new QSqlTableModel;
    m_model = nextDateModel;
    m_model->select();
    tableView->setModel(m_model);
    tableView->setColumnHidden(PreventionEngine::IPreventionEngine::ID_NEXTDATE,true);
    tableView->setColumnHidden(PreventionEngine::IPreventionEngine::ND_USER_UID,true);    
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

NextDateDialog::~NextDateDialog(){}

QStringList NextDateDialog::getListForChosenItem()
{
    QStringList list;
    int row = tableView->currentIndex().row();
    for (int col = 0; col < m_model->columnCount() ; ++col)
    {
    	  list << m_model->data(m_model->index(row,col),Qt::DisplayRole).toString();
        }
    return list;
}

