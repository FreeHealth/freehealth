/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientbar.h"
#include "patientmodel.h"
#include "ui_patientbar.h"

#include <QDataWidgetMapper>
#include <QIcon>
#include <QPainter>

#include <QDebug>

using namespace Patients;

namespace Patients {
namespace Internal {
class PatientBarPrivate
{
public:
    PatientBarPrivate() :
            ui(new Ui::PatientBar),
            m_Model(0),
            m_Index(0)
    {
    }

    ~PatientBarPrivate()
    {
        delete ui;
    }

    void setUi()
    {
        Q_ASSERT(m_Model);
        QIcon icon = m_Model->index(m_Index->row(), PatientModel::IconizedGender).data().value<QIcon>();
        ui->names->setText(m_Model->index(m_Index->row(), PatientModel::FullName).data().toString());
        ui->age->setText(m_Model->index(m_Index->row(), PatientModel::Age).data().toString());
        ui->gender->setPixmap(icon.pixmap(QSize(16,16)));
        QPixmap photo = m_Model->index(m_Index->row(), PatientModel::Photo).data().value<QPixmap>();
        photo = photo.scaled(QSize(32,32), Qt::KeepAspectRatio);
        ui->photo->setPixmap(photo);
    }

public:
    Ui::PatientBar *ui;
    PatientModel *m_Model;
    QPersistentModelIndex *m_Index;
};
}
}

PatientBar::PatientBar(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PatientBarPrivate)
{
    d->ui->setupUi(this);
    if (!PatientModel::activeModel()) {
        PatientModel::setActiveModel(new PatientModel(qApp));
    }
    setPatientModel(PatientModel::activeModel());
}

PatientBar::~PatientBar()
{
}

void PatientBar::setPatientModel(PatientModel *model)
{
    d->m_Model = model;
}

void PatientBar::setCurrentIndex(const QModelIndex &index)
{
    if (d->m_Index)
        delete d->m_Index;
    d->m_Index = new QPersistentModelIndex(index);
    d->setUi();
}

void PatientBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QRect rect = this->rect();

    QColor background = QColor(0, 0, 0, 10);
    QColor light = QColor(255, 255, 255, 40);
    QColor dark = QColor(0, 0, 0, 60);

    QLinearGradient gr(QPoint(rect.center().x(), 0), QPoint(rect.center().x(), rect.bottom()));
    gr.setColorAt(0, Qt::white);
    gr.setColorAt(0.3, QColor(250, 250, 250));
    gr.setColorAt(0.7, QColor(230, 230, 230));

    p.fillRect(rect, gr);
    p.setPen(QColor(200, 200, 200));
    p.drawLine(rect.topLeft(), rect.topRight());
    p.setPen(QColor(150, 160, 200));
    p.drawLine(rect.bottomLeft(), rect.bottomRight());

//    QString tabText(this->tabText(tabIndex));
//    QRect tabTextRect(tabRect(tabIndex));
//    QRect tabIconRect(tabTextRect);
//    QFont boldFont(painter->font());
//    boldFont.setPointSizeF(Utils::StyleHelper::sidebarFontSize());
//    boldFont.setBold(true);
//    painter->setFont(boldFont);
//    painter->setPen(selected ? Utils::StyleHelper::panelTextColor() : QColor(30, 30, 30, 80));
//    int textFlags = Qt::AlignCenter | Qt::AlignBottom | Qt::ElideRight | Qt::TextWordWrap;
//    painter->drawText(tabTextRect, textFlags, tabText);
//    painter->setPen(selected ? QColor(60, 60, 60) : Utils::StyleHelper::panelTextColor());
//    int textHeight = painter->fontMetrics().boundingRect(QRect(0, 0, width(), height()), Qt::TextWordWrap, tabText).height();
//    tabIconRect.adjust(0, 4, 0, -textHeight);
//    int iconSize = qMin(tabIconRect.width(), tabIconRect.height());
//    if (iconSize > 4)
//        style()->drawItemPixmap(painter, tabIconRect, Qt::AlignCenter | Qt::AlignVCenter,
//                                tabIcon(tabIndex).pixmap(tabIconRect.size()));
//    painter->translate(0, -1);
//    painter->drawText(tabTextRect, textFlags, tabText);
//    painter->restore();
}
