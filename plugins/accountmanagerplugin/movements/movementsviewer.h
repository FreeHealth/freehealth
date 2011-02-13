#ifndef MOVEMENTSVIEWER_H
#define MOVEMENTSVIEWER_H
#include <QWidget>

namespace Ui{
    class MovementsViewerWidget;
}

class movementsViewer:public QWidget{
    Q_OBJECT
    public :
        movementsViewer(QWidget * parent = 0);
        ~movementsViewer();
    private slots :
        void showMovements();
        void recordMovement(int valid);
        void deleteMovement();
        void validMovement();
        void validAndRecord();
        void fillMovementsComboBox();
    private :
        Ui::MovementsViewerWidget * ui;
};

#endif

