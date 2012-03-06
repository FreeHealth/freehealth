#ifndef PREVENTCORE_H
#define PREVENTCORE_H

#include "preventIHM.h"
#include "preventIO.h"

#include <QObject>

class PreventCore : public QObject {
    Q_OBJECT
    public:
        PreventCore(QObject * object = 0);
        ~PreventCore();
        void run();
        PreventIHM *m_ihm;
        PreventIO *m_IO;
        //VariantItemModel * m_modelForTreeView;
};

#endif

