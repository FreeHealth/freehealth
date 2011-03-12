#include "movementsIO.h"

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/availablemovementmodel.h>
#include <accountbaseplugin/movementmodel.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <utils/database.h>
#include <QMessageBox>
#include <QDebug>
#include <QDate>

using namespace AccountDB;
using namespace Constants;
using namespace Utils;
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
MovementsIODb::MovementsIODb(QObject *parent) :
        QObject(parent)
{
    m_modelMovements = new MovementModel(parent);
    for (int i = 0; i < Constants::MOV_MaxParam; i += 1)
    {
    	Database db;
    	QString value = db.fieldName(Constants::Table_Movement, i) ;
    	m_modelMovements->setHeaderData(i,Qt::Horizontal,value,Qt::EditRole);
        }
}

MovementsIODb::~MovementsIODb()
{
}

MovementModel *MovementsIODb::getModelMovements(QString &year)
{qDebug() << __FILE__ << QString::number(__LINE__) << " year =" << year ;
    QString filter = QString("DATEVALUE between '%1' AND '%2'").arg(year+"-01-01",year+"-12-31");
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    QString filterUid = m_modelMovements->filter();
    QString dateAndUidFilter = filterUid+" AND "+filter;
    m_modelMovements->setFilter(dateAndUidFilter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_modelMovements->filter() ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_modelMovements->rowCount()) ;
    return m_modelMovements;
}

QString MovementsIODb::getUserUid(){
    return m_modelMovements->m_UserUid;
}

QStringList MovementsIODb::listOfParents(){
    QStringList list;
    AvailableMovementModel *availablemodel = new AvailableMovementModel(this);
    for (int i = 0; i < availablemodel->rowCount(); i += 1)
    {
    	QString parent = availablemodel->data(availablemodel->index(i,AVAILMOV_PARENT),Qt::DisplayRole).toString();
    	list << parent;
        }
    return list;
}

QStandardItemModel  *MovementsIODb::getMovementsComboBoxModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(parent);
    AvailableMovementModel *availablemodel = new AvailableMovementModel(this);
    QStringList listOfAvModelParents;
    listOfAvModelParents = listOfParents();
    for (int i = 0; i < availablemodel->rowCount(); i += 1) {
        //todo : supprimer l'affichage des parents qui seront affichés en tool tip avec les commentaires
    	int type = availablemodel->data(availablemodel->index(i,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
    	QIcon icon;
        if (type == 1) {
            icon = QIcon(theme()->icon(Core::Constants::ICONADD));
        } else {
            icon = QIcon(theme()->icon(Core::Constants::ICONREMOVE));
    	}
    	QString label = availablemodel->data(availablemodel->index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();
    	QStandardItem *item = new QStandardItem(icon,label);
    	if (!listOfAvModelParents.contains(label))
    	{
    		  model->appendRow(item);// no parents in the list of items
    	    }
    	
        }
    return model;
}

QStringList MovementsIODb::getYearComboBoxModel()
{
    QStringList listOfYears;
    for (int i = 0; i < m_modelMovements->rowCount(); i += 1) {
    	QString dateStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString dateOfValueStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString yearDate = QString::number(QDate::fromString(dateStr,"yyyy-MM-dd").year());
    	QString yearDateOfValue = QString::number(QDate::fromString(dateOfValueStr,"yyyy-MM-dd").year());
    	listOfYears << yearDate << yearDateOfValue;
    }
    listOfYears.removeDuplicates();
    return listOfYears;
}

bool MovementsIODb::insertIntoMovements(QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    int rowBefore = m_modelMovements->rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_modelMovements->insertRows(rowBefore,1,QModelIndex())) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
    }
    QVariant data;
    for(int i = 1 ; i < MOV_MaxParam ; i ++) {
        data = hashValues.value(i);
        qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!m_modelMovements-> setData(m_modelMovements->index(rowBefore,i), data ,Qt::EditRole)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = "
                    << m_modelMovements->lastError().text() ;
        }
    }
    m_modelMovements->submit();
    if (m_modelMovements->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning ReceiptsEngine : \n"),__FILE__+QString::number(__LINE__)
                             + trUtf8("\nError = ") 
                             + m_modelMovements->lastError().text(),
                             QMessageBox::Ok);
        ret = false;
    }
    return ret;
}

bool MovementsIODb::deleteMovement(int row)
{qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    bool b = m_modelMovements->removeRows(row,1,QModelIndex());
    return b;
}

int MovementsIODb::getAvailableMovementId(QString &movementsComboBoxText)
{
    int availableMovementId = 0;
    AvailableMovementModel  availablemodel(this);
    availablemodel.setFilter(movementsComboBoxText);
    
    return availableMovementId;
}

bool MovementsIODb::validMovement(int row)
{
    return m_modelMovements->setData(m_modelMovements->index(row, MOV_ISVALID), 1, Qt::EditRole);
}
