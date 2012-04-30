#ifndef SEGMENTEDBUTTON_H
#define SEGMENTEDBUTTON_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QPushButton;
class QHBoxLayout;
QT_END_NAMESPACE


class SegmentedButton : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentedButton(QWidget *parent = 0);

    void setFirstButton(QPushButton *but);
    void addMiddleButton(QPushButton *but);
    void setLastButton(QPushButton *but);

signals:
    
public slots:
private:
    QHBoxLayout *_buttonLayout;
    QPushButton *_first, *_last;
    QList<QPushButton*> _buttons;
};

#endif // SEGMENTEDBUTTON_H
