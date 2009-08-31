/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef QCHECKABLECOMBOBOX_H
#define QCHECKABLECOMBOBOX_H

#include <QComboBox>
#include <QString>

/*
PasNox: tu creer un combobox sur le tas
[20:27] PasNox: et tu affiches ca view
[20:27] PasNox: QAbstractItemView * QComboBox::view () const
[20:27] PasNox: voir si elle est differente de null
[20:27] PasNox: si c'est different de null, alros ca va etre facile de creer ce genre de composant
[20:30] PasNox: si ya une view par defaut dans la cb
[20:30] PasNox: alros tu peux facilement faire devenir checkable les item
[20:30] PasNox: via
[20:31] PasNox: cb->view()->setFlags( )
[20:31] PasNox: ou sur son model
[20:31] PasNox: bref ca a pas l'air compliqué
*/

class QCheckableComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)
    Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
    Q_PROPERTY(QStringList checkedItems READ checkedItems WRITE setCheckedItems)

public:
    explicit QCheckableComboBox( QWidget * parent = 0 );
    virtual ~QCheckableComboBox();
    virtual void hidePopup();

    QString defaultText() const;
    void setDefaultText(const QString& text);

    Qt::CheckState itemCheckState(int index) const;
    void setItemCheckState(int index, Qt::CheckState state);

    QString separator() const;
    void setSeparator(const QString& separator);

    QStringList checkedItems() const;

public slots:
    void setCheckedItems(const QStringList& items);

signals:
    void checkedItemsChanged(const QStringList& items);

private:
    bool eventFilter(QObject* receiver, QEvent* event);
    QString separator;
    QString defaultText;
    bool containerMousePress;

public slots:
    void updateCheckedItems();
    void toggleCheckState(int index);


};


class QCheckableComboModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit QCheckableComboModel(QObject* parent = 0);

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

signals:
    void checkStateChanged();
};

#endif // QCHECKABLECOMBOBOX_H
