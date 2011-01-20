#ifndef QABSTRACTXMLTREEMODEL_H
#define QABSTRACTXMLTREEMODEL_H

#include <QAbstractItemModel>
class QDomNode;

namespace Utils {
namespace Internal {
class QAbstractXmlTreeModelPrivate;
}

class QAbstractXmlTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QAbstractXmlTreeModel(const QString &fileName, const QString &mainTag, QObject *parent = 0);
    ~QAbstractXmlTreeModel();

    bool setSubMainTag(const QString &childToMainTag);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

//    virtual bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
//    virtual void fetchMore(const QModelIndex &parent = QModelIndex());

    virtual QDomNode node(const QModelIndex &index) const;

public Q_SLOTS:
    virtual bool saveModel();

private:
    Internal::QAbstractXmlTreeModelPrivate *dqaxtm;
};


}  // End namespace Utils

#endif // QABSTRACTXMLTREEMODEL_H
