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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Jerome Pinguet <jerome@jerome.cc                       *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef RECORDIMPORT_RECORDIMPORTDIALOG_H
#define RECORDIMPORT_RECORDIMPORTDIALOG_H

#include <memory>

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>
#include <formmanagerplugin/formfilesselectorwidget.h>

#include <recordimportplugin/recordimport_exporter.h>
#include <recordimportconstants.h>


#include <qglobal.h>
#include <QDialog>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QHash>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QDateTimeEdit;
class QComboBox;
QT_END_NAMESPACE

namespace Form {
class FormMain;
class FormIODescription;
}

namespace Form {
class FormFilesSelectorWidget;
}

namespace RecordImport {
namespace Internal {
class RecordImportDialogPrivate;
} // namespace Internal

class RECORDIMPORT_EXPORT RecordImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordImportDialog(QWidget *parent = 0);
    ~RecordImportDialog();

    void setSoftwareComboBoxItems(const QStringList &softlist);


private Q_SLOTS:
    void import();
    int selectForm();
    void parse();

private:
    void matchPatientWidget();
    void matchEpisodeWidget();
    void matchFormWidget();
    void parser();
    void modify();
    QString xmlEpisode(int &i, const QString previousXml);
    void changeEvent(QEvent *e);
    Internal::RecordImportDialogPrivate *d;
    Form::FormFilesSelectorWidget *m_selector;
    Form::FormIODescription *m_selectedForm;
    QLineEdit *m_episodeLabelLineEdit;
    QComboBox *m_episodeLabelComboBox;
    QDateTimeEdit *m_episodeUserDateTimeDateTimeEdit;
    QComboBox *m_episodeUserDateTimeComboBox;
    QComboBox *m_episodeUserCreatorNameComboBox;
    QString m_uuid;
    QString m_formUuid;
    QVector<QStringList> *m_data;
    QStringList m_field;
    QList<Form::FormItem*> m_formItemList;
    QList<QComboBox*> m_comboFormList;
    Form::FormMain *m_formMain;
    QHash<QString, QString> *m_formItemHash;
    QChar m_record;
    QChar m_fieldSeparator;
    QString m_soft;
};

} // namespace RecordImport

#endif // RECORDIMPORT_RECORDIMPORTDIALOG_H
