#ifndef ASSETSIO_H
#define ASSETSIO_H
#include "../../accountbaseplugin/assetmodel.h"
#include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>
using namespace AccountDB;
class AssetsIO : public QObject {
    Q_OBJECT
    public :
        AssetsIO(QObject * parent);
        ~AssetsIO();
        AssetModel * getModelAssets();
        bool insertIntoAssets(QHash<int,QVariant> & hashValues);
        bool deleteAsset(int row);
    private :
        AssetModel * m_assetModel;
};

#endif

