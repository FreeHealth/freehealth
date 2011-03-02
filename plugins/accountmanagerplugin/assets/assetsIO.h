#ifndef ASSETSIO_H
#define ASSETSIO_H

#include <QObject>
#include <QVariant>
#include <QHash>

// #include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>  A eviter+++

// #include "../../accountbaseplugin/assetmodel.h"
// NON à la place déclare la class comme ça et met le include dans le cpp
// Intérêt == si modification du assetModel header pas de recompilation de cet objet avec ma méthode
// alors qu'il aurait été recompilé avec ton include.

namespace AccountDB {
class AssetModel;
}

class AssetsIO : public QObject
{
    Q_OBJECT
public:
    AssetsIO(QObject *parent);
    ~AssetsIO();

    AssetModel *getModelAssets();
    bool insertIntoAssets(QHash<int,QVariant> &hashValues);
    bool deleteAsset(int row);

private:
    AssetModel *m_assetModel;  // ça compilera sans problème
};


#endif
