/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Christian A. Reiter <christian.a.reiter@gmail.com>                *
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include <QLabel>
#include <QRubberBand>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <QStandardItemModel>

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

    QRect frame() const;
    QStandardItemModel* model() { return m_imageModel; }

public Q_SLOTS:
    void setFrozen(bool aFreeze);
    void freeze();
    void unFreeze();

private Q_SLOTS:
    void onActionCaptureTriggered();

Q_SIGNALS:
    void frozen(bool);
    void imageReady(bool);
    void clicked();
    void autoFaceShot(const QPixmap &shot);

private:
    void timerEvent(QTimerEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void restrictRubberBandConstraints();
    QImage Mat2QImage(const cv::Mat3b &src);

private:
    QImage m_image;
    cv::VideoCapture m_capture;
//    CvCapture *m_camera;
    bool m_frozen;
    int m_timerId, m_updateFreq;
    QRubberBand *m_rubberBand;
    RubberBandMode m_Mode;
    QPoint m_clickOrigin;
    QPoint m_rubberOrigin;

    cv::CascadeClassifier m_cascade;
    cv::MemStorage m_storage;
    QPixmap* m_pixmap;
    int m_frames;
//    QList<IplImage *> m_imageList;
    QStandardItemModel *m_imageModel;
};

} // end Internal
} // end Webcam

#endif // OPENCVWIDGET_H
