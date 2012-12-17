#ifndef FAKEHTMLMODEL_H
#define FAKEHTMLMODEL_H

#include <QStandardItemModel>

class FakeHtmlModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit FakeHtmlModel(QObject *parent = 0);
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    
};

#endif // FAKEHTMLMODEL_H
