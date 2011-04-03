 /***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <QWidget>
#include <QDate>
#include <QModelIndex>

namespace Ui {
    class AssetsViewerWidget;
}

class AssetsViewer : public QWidget
{
    Q_OBJECT
public:
    AssetsViewer(QWidget *parent = 0);
    ~AssetsViewer();
    enum itemValues {
        LABEL = 0,
        YEARLY_VALUE
        };

private Q_SLOTS:
    void recordAsset();
    void deleteAsset();
    void yearDateChanged(const QDate& year);
    void writeLabelByRow(const QModelIndex& index);

private:
    void fillModeComboBox();
    void fillBankComboBox();
    void refresh();
    void showAssets();
    Ui::AssetsViewerWidget *ui;
};

#endif

