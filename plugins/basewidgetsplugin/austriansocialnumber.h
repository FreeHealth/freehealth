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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>              *
 *  Contributors:                                                          *
 *       Christian A. Reiter <christian.a.reiter@gmail.com>                *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASEWIDGETS_INTERNAL_AUSTRIANSOCIALNUMBER_H
#define BASEWIDGETS_INTERNAL_AUSTRIANSOCIALNUMBER_H

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QWidget>
#include <QVariant>
#include <QLineEdit>

namespace BaseWidgets {
namespace Internal {

namespace Ui {
class AustrianSocialNumberEdit;
}

class AustrianSocialNumberEdit : public QWidget
{
    Q_OBJECT

public:
    explicit AustrianSocialNumberEdit(QWidget *parent = 0);
    ~AustrianSocialNumberEdit();

    void setNumber(const QString &number);

    bool isValid() const;
    bool isValid(const QString &number) const;
    int controlKey(const QString &number) const;

    QString number() const;

    QString emptyHtmlMask() const;
    QString toHtml() const;

public Q_SLOTS:
    void populateWithPatientData();

    /** \brief updates the status label (valid social number?)*/
    void updateStatus();

private:
    void populateLineEdits(QString number = QString::null);
    void addChar(const QString &c, int currentLineEditId, int pos);
    void removeChar(int currentLineEditId, int pos);
    void setCursorPosition(int currentLineEditId, int pos);
    void checkControlKey();

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::AustrianSocialNumberEdit *ui;
    QList<QLineEdit *> m_Edits;
    QList<int> m_NbChars;
    QString m_FullNumber;
    int m_CursorPos;
};

} // namespace Internal

//forward declaration of form data
class AustrianSocialNumberFormData;

class AustrianSocialNumberFormWidget: public Form::IFormWidget
{
    Q_OBJECT
public:
    AustrianSocialNumberFormWidget(Form::FormItem *formItem, QWidget *parent = 0);
    ~AustrianSocialNumberFormWidget();

    QString printableHtml(bool withValues = true) const;

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

public Q_SLOTS:
    void retranslate();

private:
    AustrianSocialNumberFormData *m_ItemData;
    Internal::AustrianSocialNumberEdit *m_SVNR;
};

class AustrianSocialNumberFormData : public Form::IFormItemData
{
public:
    AustrianSocialNumberFormData(Form::FormItem *item);
    ~AustrianSocialNumberFormData();

    void setWidget(Internal::AustrianSocialNumberEdit *w) {m_Widget = w; clear();}
    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;
    void setModified(bool modified);

    // TODO: code this
    //void setReadOnly(bool readOnly);
    //bool isReadOnly() const;

    // Use setData/Data for episode data
    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    Internal::AustrianSocialNumberEdit *m_Widget;
    QString m_OriginalValue;
};

} // namespace BaseWidgets


#endif // BASEWIDGETS_INTERNAL_AUSTRIANSOCIALNUMBER_H
