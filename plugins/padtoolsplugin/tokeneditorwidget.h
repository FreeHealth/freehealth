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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_TOKENEDITORWIDGET_H
#define PADTOOLS_TOKENEDITORWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file tokeneditorwidget.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 25 Apr 2012
*/


namespace PadTools {
namespace Internal {
class TokenModel;
class PadItem;

namespace Ui {
class TokenEditorWidget;
}

class TokenEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TokenEditorWidget(QWidget *parent = 0);
    ~TokenEditorWidget();

    void setTokenModel(TokenModel *model);

public Q_SLOTS:
    void clear();

    void setCurrentIndex(const QModelIndex &index);
    void setTokenUid(const QString &uid);

    void setConditionnalBeforeHtml(const QString &html);
    void setConditionnalAfterHtml(const QString &html);

    void setConditionnalBeforePlainText(const QString &txt);
    void setConditionnalAfterPlainText(const QString &txt);

    QString toRawSourceHtml() const;
    void getOutput(QString &html, PadItem &item, int startingOutputPos) const;

private:
    Ui::TokenEditorWidget *ui;
    QString _tokenUid;
    TokenModel *_model;
};

} // namespace Internal
} // namespace PadTools

#endif // PADTOOLS_TOKENEDITORWIDGET_H
