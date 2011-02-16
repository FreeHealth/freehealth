#ifndef ASSETSIO_H
#define ASSETSIO_H
#include "../../accountbaseplugin/assetmodel.h"
#include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>
using namespace AccountDB;
class assetsIO : public QObject {
    Q_OBJECT
    public :
        assetsIO(QObject * parent);
        ~assetsIO();
        AssetModel * getModelAssets();
        bool insertIntoAssets(QHash<int,QVariant> & hashValues);
        bool deleteAsset(int row);
    private :
        AssetModel * m_assetModel;
};

#endif

