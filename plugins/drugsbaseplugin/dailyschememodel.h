#ifndef DAILYSCHEMEMODEL_H
#define DAILYSCHEMEMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <translationutils/constanttranslations.h>

#include <QString>
#include <QAbstractTableModel>

/**
 * \file dailyschememodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.0
 * \date 23 Nov 2009
*/

namespace DrugsDB {
namespace Internal {
//class DrugsData;
//class DrugInteraction;
//class DrugInfo;
class DailySchemeModelPrivate;
} // end namespace Internal


class DRUGSBASE_EXPORT DailySchemeModel : public QAbstractTableModel
{
public:
    enum Column {
        DayReference = 0,
        Value,
        MaxParam
    };

    DailySchemeModel(QObject * parent = 0, const QString &serializedContent = QString::null);
    ~DailySchemeModel();

    QString serializedContent() const;
    void setSerializedContent(const QString &content);

    int columnCount(const QModelIndex &) const {return MaxParam;}
    int rowCount(const QModelIndex & = QModelIndex()) const {return Trans::Constants::Time::DS_MaxParam;}

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setScored(bool isScored);
    void setMaximumDay(double max);
    double sum() const;

private:
    Internal::DailySchemeModelPrivate *d;
};


} // end namespace DrugsDB

#endif // DAILYSCHEMEMODEL_H
