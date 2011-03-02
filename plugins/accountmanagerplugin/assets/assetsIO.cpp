#include "assetsIO.h"
#include "assetmodel.h

#include <accountbaseplugin/constants.h>

#include <QDebug>
#include <QMessageBox>


using namespace AccountDB;
using namespace Constants;

AssetsIO::AssetsIO(QObject *parent) :
    QObject(parent), m_assetModel(new AssetModel(this))
{
}

AssetsIO::~AssetsIO()
{
}

bool AssetsIO::insertIntoAssets(QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    int rowBefore = m_assetModel->rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_assetModel->insertRows(rowBefore,1,QModelIndex())) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
    }
    QVariant data;
    for(int i = 1 ; i < ASSETS_MaxParam ; i ++){
        data = hashValues.value(i);
        qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!m_assetModel-> setData(m_assetModel->index(rowBefore,i), data ,Qt::EditRole)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << " asset model error = "
                    << m_assetModel->lastError().text() ;
        }
    }
    m_assetModel->submit();
    if (m_assetModel->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning AssetsIO : "),trUtf8("Error = ") 
                             + m_assetModel->lastError().text(),
                             QMessageBox::Ok);
        ret = false;
    }
    return ret;
}

bool AssetsIO::deleteAsset(int row)
{
    bool b = true;
    if (!m_assetModel->removeRow(row,QModelIndex())) {
        b = false;
    }
    return b;
}
