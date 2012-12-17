#include "fakehtmlmodel.h"

#include <utils/global.h>

#include <QDir>
#include <QApplication>

namespace {
const char *const MAC = "/../../../";
const char *const PIX32 = "global_resources/pixmap/32x32/identity.png";
}

FakeHtmlModel::FakeHtmlModel(QObject *parent) :
    QStandardItemModel(parent)
{
    setColumnCount(1);
    QStandardItem *test = new QStandardItem;
    test->setText("<b>This is Multiline<b><br />html code");
    invisibleRootItem()->appendRow(test);
}

QVariant FakeHtmlModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    if (!index.isValid())
        return QVariant();
    switch (role) {
    case Qt::DecorationRole:
    {
        QString path = qApp->applicationDirPath();
        if (Utils::isRunningOnMac())
            path += MAC;
        path += PIX32;
        path = QDir::cleanPath(path);
        return QPixmap(path);
    }
    default: break;
    }
    return QStandardItemModel::data(index, role);
}
