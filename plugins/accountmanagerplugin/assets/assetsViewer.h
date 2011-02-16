#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H
#include <QWidget>

namespace Ui {
    class AssetsViewerWidget;
}

class assetsViewer : public QWidget {
    Q_OBJECT
    public :
        assetsViewer(QWidget * parent = 0);
        ~assetsViewer();
    private slots :
        void recordAsset();
        void deleteAsset();
    private :
        void fillModeComboBox();
        Ui::AssetsViewerWidget * ui;
};

#endif

