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
 *   Adapted from the Qt examples by Eric MAEKER, <eric.maeker@gmail.com>  *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "imageviewer.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include <QLabel>
#include <QScrollBar>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include <QDebug>

using namespace Utils;
using namespace Trans::ConstantTranslations;

// TODO: add gesture for the resize mode and Alt(or Ctrl)+mousewheel to zoom in/out

ImageViewer::ImageViewer(QWidget *parent) :
    QDialog(parent), mPreviousButton(0), mNextButton(0), m_CurrentIndex(-1)
{
    setObjectName("ImageViewer");
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(false);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_ButBox = new QDialogButtonBox(this);
    //    QAbstractButton *zin = m_ButBox->addButton(tkTr(Trans::Constants::ZOOMIN_TEXT).remove("&"), QDialogButtonBox::ActionRole);
    //    QAbstractButton *zout = m_ButBox->addButton(tkTr(Trans::Constants::ZOOMOUT_TEXT).remove("&"), QDialogButtonBox::ActionRole);

    mPreviousButton = m_ButBox->addButton(tkTr(Trans::Constants::PREVIOUS_TEXT).remove("&"), QDialogButtonBox::ActionRole);
    mNextButton = m_ButBox->addButton(tkTr(Trans::Constants::NEXT_TEXT).remove("&"), QDialogButtonBox::ActionRole);
//    QAbstractButton *full = m_ButBox->addButton(tkTr(Trans::Constants::FULLSCREEN_TEXT).remove("&"), QDialogButtonBox::ActionRole);
    QPushButton *close = m_ButBox->addButton(QDialogButtonBox::Close);

    QVBoxLayout *l = new QVBoxLayout(this);
    setLayout(l);
    l->addWidget(scrollArea);
    l->addWidget(m_ButBox);

    //    connect(zin, SIGNAL(clicked()), this, SLOT(zoomIn()));
    //    connect(zout, SIGNAL(clicked()), this, SLOT(zoomOut()));

    connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(mPreviousButton, SIGNAL(clicked()), this, SLOT(previous()));
    connect(close, SIGNAL(clicked()), this, SLOT(accept()));
//    connect(full, SIGNAL(clicked()), this, SLOT(toggleFullScreen()));

    Utils::resizeAndCenter(this);
}

void ImageViewer::setPixmap(const QPixmap &pixmap)
{
    setPixmaps(QList<QPixmap>() << pixmap);
}

void ImageViewer::setPixmaps(const QList<QPixmap> &pixmaps)
{
    if (pixmaps.isEmpty())
        return;
    m_pixmaps = pixmaps;
    imageLabel->setPixmap(pixmaps.first());
    normalSize();
    fitToWindow();
    m_CurrentIndex = 0;
    Utils::resizeAndCenter(this);
    updateButtons();
}

void ImageViewer::showPixmapFile(const QString &absFilePath)
{
    Q_UNUSED(absFilePath);
    // TODO: code here ImageViewer::showPixmapFile(const QString &absFilePath)
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    //    bool fitToWindow = fitToWindowAct->isChecked();
    //    scrollArea->setWidgetResizable(true);
    //        normalSize();
    //    updateActions();
}

void ImageViewer::next()
{
    if (m_CurrentIndex < (m_pixmaps.count() - 1)) {
        ++m_CurrentIndex;
        imageLabel->setPixmap(m_pixmaps.at(m_CurrentIndex));
        normalSize();
        fitToWindow();
    }
}

void ImageViewer::previous()
{
    if (m_CurrentIndex >= 1) {
        --m_CurrentIndex;
        imageLabel->setPixmap(m_pixmaps.at(m_CurrentIndex));
        normalSize();
        fitToWindow();
    }
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    //     zoomInAct->setEnabled(scaleFactor < 3.0);
    //     zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void ImageViewer::toggleFullScreen()
{
    Utils::setFullScreen(this, true);
}

void ImageViewer::updateButtons()
{
    if (mPreviousButton)
        mPreviousButton->setEnabled(m_CurrentIndex > 1);
    if (mNextButton)
        mNextButton->setEnabled(m_CurrentIndex < m_pixmaps.count()-1);
}
