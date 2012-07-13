#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include <QLabel>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class QRubberBand;

namespace Webcam {
namespace Internal {

class OpenCVWidget : public QLabel
{
    Q_OBJECT
public:
    enum RubberBandMode {Create, Move};

    OpenCVWidget(QWidget *parent = 0);
    ~OpenCVWidget();

    void toggleFreezeMode();
    bool isFrozen() const {return m_frozen;}

    void setImageUpdateFrequency(const int ms);
    int defaultUpdateFrequency() const;

Q_SIGNALS:
    void frozen(bool);
    void imageReady(bool);
    
private:
    void timerEvent(QTimerEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void restrictRubberBandConstraints();
    
private:
    QImage _image;
    CvCapture *_camera;
    bool m_frozen;
    int _timerId, _updateFreq;
    QRubberBand *m_rubberBand;
    RubberBandMode rbMode;
    QPoint m_clickOrigin;
    QPoint m_rubberOrigin;
};

} // end Internal
} // end Webcam

#endif // OPENCVWIDGET_H
