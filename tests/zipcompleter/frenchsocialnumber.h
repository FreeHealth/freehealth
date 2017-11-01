/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FRENCHSOCIALNUMBER_H
#define FRENCHSOCIALNUMBER_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace Ui {
    class FrenchSocialNumber;
}

class FrenchSocialNumber : public QWidget
{
    Q_OBJECT

public:
    explicit FrenchSocialNumber(QWidget *parent = 0);
    ~FrenchSocialNumber();

    void setNumberWithControlKey(const QString &number);
    void setNumberWithoutControlKey(const QString &number);

    bool isValid() const;
    bool isValid(const QString &number, const QString &key) const;
    int controlKey(const QString &number) const;

    QString numberWithControlKey() const;
    QString numberWithoutControlKey() const;

private:
    void populateLineEdits(QString number = QString::null);
    void addChar(const QString &c, int currentLineEditId, int pos);
    void removeChar(int currentLineEditId, int pos);
    void setCursorPosition(int currentLineEditId, int pos);
    void checkControlKey();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    Ui::FrenchSocialNumber *ui;
    QList<QLineEdit *> m_Edits;
    QList<int> m_NbChars;
    QString m_FullNumber;
    int m_CursorPos;
};

#endif // FRENCHSOCIALNUMBER_H
