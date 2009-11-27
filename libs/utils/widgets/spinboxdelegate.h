#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <QItemDelegate>
#include <QWidget>
#include <QObject>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QSize>


namespace Utils {

class SpinBoxDelegate : public QItemDelegate
 {
     Q_OBJECT
 public:
     SpinBoxDelegate(QObject *parent = 0, double min = 0.0, double max = 100.0, bool isDouble = false);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;

 private:
     bool m_IsDouble;
     double m_Min, m_Max;
 };


} // end namespace Utils


#endif // SPINBOXDELEGATE_H
