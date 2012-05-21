#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QWidget>
#include "filter.h"

class HeadState;

/*!
 * \class RenderWidget
 * \brief A Widget that displays frames from a OpenCV video source
 */
class RenderWidget : public QWidget, public Filter {

    Q_OBJECT

public:
    RenderWidget(QWidget* parent);
    void processPoint(HeadState* state);
public slots:
    void onFrameSizeChanged(int width, int height);
signals:
    void frameSizeChanged(int width, int height);
protected:
    void paintEvent(QPaintEvent*);
private:
    void updatePixmap(const IplImage* frame);
    QPixmap bufferPixmap;
    uchar* imageData;
    int imageWidth, imageHeight;
    int time;
    int frames;
};

#endif
