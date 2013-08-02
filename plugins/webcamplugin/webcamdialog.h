/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *  Contributors:                                                          *
 *       Eric Maeker <eric.maeker@gmail.com                                *
 ***************************************************************************/
#ifndef WEBCAMDIALOG_H
#define WEBCAMDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QHBoxLayout>
#include <QStandardItemModel>

namespace Webcam {

namespace Internal {
class OpenCVWidget;
}

namespace Ui {
class WebcamDialog;
}

/*!
 * \brief The WebcamDialog class
 *
 * This class constucts a dialog with a webcam widget that enables you to make a photo with the webcam.
 * It generates shots automatically and saves them in a view. Therefore it detects the face in the
 * picture and  saves only a frame around it. Additionally you can freeze the picture manually and
 * select the face with the mouse using a "rubberband" selection.
 *
 * The dialog returns the chosen picture. If the user presses Escape or clicks on "Cancel",
 * a QPixmap() is returned instead.
 */
class WebcamDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WebcamDialog(QWidget *parent = 0);
    ~WebcamDialog();

    QPixmap photo() const;

private Q_SLOTS:
    void setDevice(int device);
    void updatefreezeButton(bool aFreeze);
    void autoFaceShot(const QPixmap &pix);
    void faceShotActivated(const QModelIndex &index);

private:
    void changeEvent(QEvent *event);

private:
    Ui::WebcamDialog *ui;
    QPushButton *m_freezeButton;
    QPixmap _pixmap;
    QStandardItemModel *m_imageModel;
    int m_device;
};

}  // namespace WebCam

#endif // WEBCAMDIALOG_H
