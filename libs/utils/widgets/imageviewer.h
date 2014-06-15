/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <utils/global_exporter.h>
#include <QDialog>
#include <QPixmap>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE
class QLabel;
class QScrollArea;
class QScrollBar;
class QDialogButtonBox;
QT_END_NAMESPACE

namespace Utils {

class UTILS_EXPORT ImageViewer : public QDialog
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = 0);

    void setPixmap(const QPixmap &pixmap);
    void setPixmaps(const QList<QPixmap> &pixmaps);
    void showPixmapFile(const QString &absFilePath);

private Q_SLOTS:
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void next();
    void previous();
    void toggleFullScreen();
    void updateButtons();

private:
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QLabel *imageLabel;
    QScrollArea *scrollArea;
    int scaleFactor;
    QDialogButtonBox *m_ButBox;
    QPushButton *mPreviousButton, *mNextButton;
    QList<QPixmap> m_pixmaps;
    int m_CurrentIndex;
};

}  // End namespace Utils

#endif // IMAGEVIEWER_H
