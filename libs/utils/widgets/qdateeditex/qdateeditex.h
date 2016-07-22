/**************************************************************************
**
** Copyright (c) 2013 Qualiant Software GmbH
**
** Author: Andreas Holzammer, KDAB (andreas.holzammer@kdab.com)
**
** Contact: Qualiant Software (d.oberkofler@qualiant.at)
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Qualiant Software at d.oberkofler@qualiant.at.
**
**************************************************************************/

#ifndef DATEEDITEX_H
#define DATEEDITEX_H

#include <QDateEdit>
#include <QDateTimeEdit>
#include <utils/global_exporter.h>

namespace Utils {

class UTILS_EXPORT QDateEditEx : public QDateEdit
{
    Q_OBJECT

    Q_PROPERTY(bool nullable READ isNullable WRITE setNullable)
public:
    explicit QDateEditEx(QWidget *parent = 0);

    QDateTime dateTime() const;
    QDate date() const;
    QTime time() const;

    bool isNullable() const;
    void setNullable(bool enable);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

protected:
    /*! \reimp */ void showEvent(QShowEvent *event);
    /*! \reimp */ void resizeEvent(QResizeEvent *event);
    /*! \reimp */ void paintEvent(QPaintEvent *event);
    /*! \reimp */ void keyPressEvent(QKeyEvent *event);
    /*! \reimp */ void mousePressEvent(QMouseEvent *event);
    /*! \reimp */ void focusOutEvent(QFocusEvent *event);
    /*! \reimp */ bool focusNextPrevChild(bool next);
    /*! \reimp */ QValidator::State validate(QString &input, int &pos) const;

public Q_SLOTS:
    /*! \reimp */ void setDateTime(const QDateTime &dateTime);
    /*! \reimp */ void setDate(const QDate &date);
    /*! \reimp */ void setTime(const QTime &time);

private slots:
    void clearButtonClicked();

private:
    Q_DISABLE_COPY(QDateEditEx)
    class Private;
    friend class Private;
    Private* d;

};

} // end namespace Utils

#endif // DATEEDITEX_H
