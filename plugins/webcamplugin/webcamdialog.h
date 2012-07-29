/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Contributors:                                                         *
 *       Eric Maeker <eric.maeker@gmail.com                                *
 ***************************************************************************/
#ifndef WEBCAMDIALOG_H
#define WEBCAMDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QHBoxLayout>

namespace Webcam {

namespace Internal {
class OpenCVWidget;
}

namespace Ui {
class WebcamDialog;
}

class WebcamDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WebcamDialog(QWidget *parent = 0);
    ~WebcamDialog();
    
    QPixmap photo() const;

private Q_SLOTS:
    void updatefreezeButton(bool aFreeze);
    void autoFaceShot(const QPixmap &pix);

private:
    bool eventFilter(QObject *o, QEvent *e);
    void changeEvent(QEvent *event);

private:
    Ui::WebcamDialog *ui;
    QPushButton *m_freezeButton;
    int _faces;
    QHBoxLayout *_autoshotLayout;
    QPixmap _pixmap;
};

}  // namespace WebCam

#endif // WEBCAMDIALOG_H
