#ifndef MOVEMENTSVIEWER_H
#define MOVEMENTSVIEWER_H
#include <QWidget>

namespace Ui{
    class MovementsViewerWidget;
}

class MovementsViewer : public QWidget
{
    Q_OBJECT
    public:
        MovementsViewer(QWidget * parent = 0);
        ~MovementsViewer();
    private Q_SLOTS:
        void showMovements();
        void recordMovement();
        void deleteMovement();
        void validMovement();
        void validAndRecord();
        void setMovementsComboBoxToolTips(int);
    private:
        Ui::MovementsViewerWidget * ui;
        void fillMovementsComboBox();
        void fillYearComboBox();
        void fillBankComboBox();
        int m_valid ;
};

#endif

