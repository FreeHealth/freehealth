#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <QWidget>

namespace Ui {
    class AssetsViewerWidget;
}

class AssetsViewer : public QWidget
{
    Q_OBJECT
public:
    AssetsViewer(QWidget *parent = 0);
    ~AssetsViewer();

private Q_SLOTS:
    void recordAsset();
    void deleteAsset();

private:
    void fillModeComboBox();
    Ui::AssetsViewerWidget *ui;
};

#endif

