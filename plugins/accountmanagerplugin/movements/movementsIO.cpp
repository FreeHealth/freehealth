#include "movementsIO.h"
#include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>
#include "../../accountbaseplugin/availablemovementmodel.h"//<accountbaseplugin/availablemovementmodel>
#include "../../accountbaseplugin/movementmodel.h"//<accountbaseplugin/movementmodel>
#include <QMessageBox>
#include <QDebug>
#include <QDate>

using namespace AccountDB;
using namespace Constants;
	
movementsIODb::movementsIODb(QObject * parent){
    m_modelMovements = new MovementModel(parent);
}

movementsIODb::~movementsIODb(){
    delete m_modelMovements;
}

MovementModel * movementsIODb::getModelMovements(QString & year){
    QString filter = QString("DATEVALUE beetween '%1' AND '%2'").arg(year+"-01-01",year+"12-31");
    m_modelMovements->setFilter(filter);
    return m_modelMovements;
}

QStandardItemModel  * movementsIODb::getMovementsComboBoxModel(QObject *parent){
    QStandardItemModel * model = new QStandardItemModel(parent);
    AvailableMovementModel * availablemodel = new AvailableMovementModel(this);
    for (int i = 0; i < availablemodel->rowCount(); i += 1)
    {
    	int type = availablemodel->data(availablemodel->index(i,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
    	QIcon icon;
    	if (type == 1)
    	{
    		  icon = QIcon("../../../global_resources/pixmap/16x16/add.png");
    	    }
    	else{
    	          icon = QIcon("../../../global_resources/pixmap/16x16/remove.png");
    	}
    	QString label = availablemodel->data(availablemodel->index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();
    	QStandardItem * item = new QStandardItem(icon,label);
    	model->appendRow(item);
    }
    return model;
}

QStringList movementsIODb::getYearComboBoxModel(){
    QStringList listOfYears;
    for (int i = 0; i < m_modelMovements->rowCount(); i += 1)
    {
    	QString dateStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString dateOfValueStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString yearDate = QString::number(QDate::fromString(dateStr,"yyyy-MM-dd").year());
    	QString yearDateOfValue = QString::number(QDate::fromString(dateOfValueStr,"yyyy-MM-dd").year());
    	listOfYears << yearDate << yearDateOfValue;
    }
    listOfYears.removeDuplicates();
    return listOfYears;
}

bool movementsIODb::insertIntoMovements(QHash<int,QVariant> & hashValues){
    bool ret = true;
    int rowBefore = m_modelMovements->rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_modelMovements->insertRows(rowBefore,1,QModelIndex()))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
        }
    QVariant data;
    for(int i = 1 ; i < MOV_MaxParam ; i ++){
         data = hashValues.value(i);
         qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
         if (!m_modelMovements-> setData(m_modelMovements->index(rowBefore,i), data ,Qt::EditRole))
            {
            	qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = " 
                                                                    << m_modelMovements->lastError().text() ;
                }
        }
        m_modelMovements->submit();
    if (m_modelMovements->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning ReceiptsEngine : "),trUtf8("Error = ") 
                             + m_modelMovements->lastError().text(),
                             QMessageBox::Ok);
        ret = false;
    }
    return ret;
}

bool movementsIODb::deleteMovement(int row){
    bool b = true;
    if (!m_modelMovements->removeRow(row,QModelIndex()))
    {
    	  b = false;
        }
    return b;
}

int movementsIODb::getAvailableMovementId(QString & movementsComboBoxText){
    int availableMovementId = 0;
    AvailableMovementModel  availablemodel(this);
    availablemodel.setFilter(movementsComboBoxText);
    
    return availableMovementId;
}

bool movementsIODb::validMovement(int row){
    bool b = true;
    const QVariant value = QVariant(1);
    if (!m_modelMovements->setData(m_modelMovements->index(row,MOV_ISVALID), value , Qt::EditRole))
    {
    	  b = false;
        }
    return b;
}
