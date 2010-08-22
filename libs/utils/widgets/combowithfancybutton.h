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
#ifndef COMBOWITHFANCYBUTTON_H
#define COMBOWITHFANCYBUTTON_H

#include <QComboBox>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QTreeView;
class QSettings;
class QString;
class QStringListModel;
QT_END_NAMESPACE

/**
 * \file combowithfancybutton.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 18 Aug 2010
*/

namespace Utils {
namespace Internal {
class ItemDelegate;
class StringModel;
}

class ComboWithFancyButton : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboWithFancyButton(QWidget *parent = 0);

    void setRemoveItems(bool state);
    void setMoveItems(bool state);

    void addItems(const QStringList &list, const QVariant &userData = QVariant());
    void addItem(const QString & text, const QVariant &userData = QVariant());
    QStringList items(const QVariant &userData = QVariant()) const;


    //    void setSettings(QSettings *settings) {m_Settings = settings;}
    //    void setSettingsKey(const QString &key) {m_Key = key;}
    void saveItemsToSettings();

    void setDarkIcon(const QIcon &icon);
    void setRemoveLightIcon(const QIcon &icon);
    void setMoveUpLightIcon(const QIcon &icon);
    void setMoveDownLightIcon(const QIcon &icon);

public Q_SLOTS:
    void fancyClear();

protected Q_SLOTS:
    void handlePressed(const QModelIndex &index);

protected:
    void showPopup();
    void hidePopup();


private:
    Internal::ItemDelegate *delegate;
    QTreeView *view;
    QSettings *m_Settings;
    Internal::StringModel *model;
    QString m_Key;
    bool m_ignoreHide, m_editableState;
};

}  // End namespace Utils

#endif // COMBOWITHFANCYBUTTON_H
