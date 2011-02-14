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
        void recordMovement();
        void deleteMovement();
        void validMovement();
        void validAndRecord();
    private :
        Ui::MovementsViewerWidget * ui;
        void fillMovementsComboBox();
        void fillYearComboBox();
        int m_valid ;
};

#endif

