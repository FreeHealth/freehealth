#include "fakehtmlmodel.h"

#include <utils/global.h>

#include <QDir>
#include <QApplication>
#include <QDebug>

namespace {
const char *const MAC = "/../../../";
const char *const PIX32 = "/../../global_resources/pixmap/32x32/identity.png";
}

FakeHtmlModel::FakeHtmlModel(QObject *parent) :
    QStandardItemModel(parent)
{
    setColumnCount(2);
    QStandardItem *test = new QStandardItem;
    test->setText("<b>This is Multiline</b><br />html code asjkh lasdjkfh jfh jhl fkhakls hfljkhsgjhsg kjhasg dlfjkhajklshjk dfasdghf gsdhfg jsghf kjsghdfkjsghfk jhgsd fkjhgas dfkjhgas fkjhg");
    invisibleRootItem()->appendRow(test);
    test = new QStandardItem;
    test->setText("<b>This is Multiline</b><br />html code <br />without decoration asjkh lasdjkfh jfh  jhl fkhakls hfljkhsgjhsg kjhasg dlfjkhajklshjk dfasdghf gsdhfg jsghf kjsghdfkjsghfk jhgsd fkjhgas dfkjhgas fkjhg");
    invisibleRootItem()->appendRow(test);
    QStandardItem *test2 = new QStandardItem;
    test2->setText("<b>This is Child</b><br />html code <br />with decoration asjkh lasdjkfh jfh  jhl fkhakls hfljkhsgjhsg kjhasg dlfjkhajklshjk dfasdghf gsdhfg jsghf kjsghdfkjsghfk jhgsd fkjhgas dfkjhgas fkjhg");
    test->appendRow(test2);
}

QVariant FakeHtmlModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() == 1)
        return QVariant();
    switch (role) {
    case Qt::DecorationRole:
    {
        if (index.row()==0) {
            QString path = qApp->applicationDirPath();
            if (Utils::isRunningOnMac())
                path += MAC;
            path += PIX32;
            path = QDir::cleanPath(path);
            return QPixmap(path);
        }
    }
    default: break;
    }
    return QStandardItemModel::data(index, role);
}
