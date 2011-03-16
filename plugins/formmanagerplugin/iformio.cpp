/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class Form::IFormIO
  This object interface is the in/out form object. It will be used to read patient files in order to
  construct patient files structure.
*/

/** \fn virtual QString Form::IFormIO::name() const
  Return the name of Form IO engine.
*/

/** \fn virtual void Form::IFormIO::muteUserWarnings(bool state);
  For debugging purpose, mute/unmute console warnings.
*/

/** \fn virtual bool Form::IFormIO::setFileName(const QString &absFileName);
  Set the filename to process, if IO device is a file. Return \e true if the IO engine
  can read this file according to its name and existence on the disk.
*/

/** \fn virtual QStringList Form::IFormIO::fileFilters() const;
  Return the file filters that the IO engine can read. Eg: <em>*.xml</em>
*/

/** \fn virtual QString Form::IFormIO::managedFileExtension() const;
  Return the file extension managed by the IO engine.
*/

/** \fn virtual bool Form::IFormIO::canReadFile() const;
  If the file is accessible to the IO engine, start the read process. Return \e true if the
  IO engine can read the file. \sa Form::IFormIO::setFileName()
*/

/** \fn virtual void formDescriptionToTreeWidget(QTreeWidget *tree, const QString &lang = Trans::Constants::ALL_LANGUAGE) const;
  If the file is accessible to the IO device and is readable, read its description and populate a
  QTreeWidget with it. Form::IFormIO::canReadFile() must be called first.
*/

/** \fn virtual Form::FormMain *Form::IFormIO::loadForm();
  Load the form, recreate the form tree and return the empty root Form::FormMain.
  Form::IFormIO::canReadFile() must be called first.
*/

/** \fn virtual bool Form::IFormIO::saveForm(QObject *treeRoot);
  Save a form (not its data).
*/

/** \fn virtual QString Form::IFormIO::lastError() const;
  Return the last error encountered wy the IO engine.
*/

#include "iformio.h"

#include <translationutils/constanttranslations.h>

#include <QDate>
#include <QDebug>

using namespace Form;
using namespace Trans::ConstantTranslations;

FormIODescription::FormIODescription()
{}

FormIODescription::~FormIODescription()
{}

QVariant FormIODescription::data(const int ref, const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty()) {
        l = QLocale().name().left(2);
        QHash<int, QVariant> *datas = m_Datas.getLanguage(l);
        if (datas) {
            if (!datas->value(ref).isNull())
                return datas->value(ref);
        }
        l = Trans::Constants::ALL_LANGUAGE;
        datas = m_Datas.getLanguage(l);
        if (datas) {
            if (!datas->value(ref).isNull())
                return datas->value(ref);
        }
    }
    QHash<int, QVariant> *datas = m_Datas.getLanguage(lang);
    if (datas) {
        if (!datas->value(ref).isNull())
            return datas->value(ref);
    }
    return QVariant();
}

bool FormIODescription::setData(const int ref, const QVariant &value, const QString &lang)
{
    QString l = lang;
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    QHash<int, QVariant> *datas = m_Datas.createLanguage(l);
    datas->insert(ref, value);
    return true;
}

void FormIODescription::formDescriptionToTreeWidget(QTreeWidget *tree) const
{
    /** \todo code here */
    tree->clear();
    tree->setColumnCount(2);
    QFont bold;
    bold.setBold(true);

    QTreeWidgetItem *general = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::INFORMATIONS));
    general->setFont(0, bold);
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::AUTHOR) << data(FormIODescription::Author).toString());
    new QTreeWidgetItem(general, QStringList() << QCoreApplication::translate("Forms", "License") << data(FormIODescription::License).toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::DESCRIPTION) << data(FormIODescription::ShortDescription).toString());

    QTreeWidgetItem *version = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::VERSION));
    version->setFont(0, bold);
    new QTreeWidgetItem(version, QStringList() << tkTr(Trans::Constants::VERSION) << data(FormIODescription::Version).toString());
    new QTreeWidgetItem(version, QStringList() << QCoreApplication::translate("Forms", "Creation date") << data(FormIODescription::CreationDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));
    new QTreeWidgetItem(version, QStringList() << QCoreApplication::translate("Forms", "Last modification date") << data(FormIODescription::LastModificationDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));

    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

void FormIODescription::warn() const
{
    QString tmp = "FormIODescription(";
    for(int i=0; i < MaxParam; ++i) {
        tmp += QString("%1:%2\n").arg(i).arg(data(i).toString());
    }
    qWarning() << tmp;
}
