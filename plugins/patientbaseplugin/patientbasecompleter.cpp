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
 ***************************************************************************/
#include "patientbasecompleter.h"
#include "patientbase.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QSqlTableModel>
#include <QAbstractItemView>

using namespace Patients;

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace {
class PatientCompleterModel : public QSqlTableModel
{
public:
    enum { ColumnFullName = 0 };

    enum FilterListIndex {
        NameIndex = 0,
        FirstNameIndex,
        SecondNameIndex
    };

    PatientCompleterModel(QObject *parent) : QSqlTableModel(parent, patientBase()->database())
    {
        setTable(patientBase()->table(Constants::Table_IDENT));
    }

    // See FilterListIndex for the content of the filter
    void setNameFilter(const QStringList &names)
    {
        qWarning() << Q_FUNC_INFO << names;

        /** \todo use setQuery -> limit 20, order by asc name... */

        // Filter is
        //   not dead ?
        //   active
        //   name/first/second
        const QString &table = patientBase()->table(Constants::Table_IDENT);
        QStringList filter;
        filter << QString("`%1`.`%2`=1")
                  .arg(table)
                  .arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_ISACTIVE));

        if (!names.at(NameIndex).isEmpty()) {
            filter << QString("`%1`.`%2` like '%3%'")
                      .arg(table)
                      .arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_NAME))
                      .arg(names.at(NameIndex));
        }
        if (!names.at(FirstNameIndex).isEmpty()) {
            filter << QString("`%1`.`%2` like '%3%'")
                      .arg(table)
                      .arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_FIRSTNAME))
                      .arg(names.at(FirstNameIndex));
        }
        if (!names.at(SecondNameIndex).isEmpty()) {
            filter << QString("`%1`.`%2` like '%3%'")
                      .arg(table)
                      .arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_SECONDNAME))
                      .arg(names.at(SecondNameIndex));
        }

//        qWarning() << filter.join(" AND ");

        setFilter(filter.join(" AND "));
        select();
    }

    QVariant data(const QModelIndex &idx, int role) const
    {
        if (!idx.isValid())
            return QVariant();

        if (idx.column()==ColumnFullName) {
            if (role==Qt::DisplayRole || role==Qt::EditRole || role==Qt::ToolTipRole) {
                QString name = QSqlTableModel::data(QSqlTableModel::index(idx.row(), Constants::IDENTITY_NAME, idx.parent())).toString();
                QString firstName = QSqlTableModel::data(QSqlTableModel::index(idx.row(), Constants::IDENTITY_FIRSTNAME, idx.parent())).toString();
                QString secName = QSqlTableModel::data(QSqlTableModel::index(idx.row(), Constants::IDENTITY_SECONDNAME, idx.parent())).toString();
                QString r = QString("%1 %2 %3").arg(name).arg(firstName).arg(secName).simplified();
                return r;
            } else if (role==Qt::BackgroundRole) {
                const QString &g = QSqlTableModel::data(QSqlTableModel::index(idx.row(), Constants::IDENTITY_GENDER)).toString();
                if (g=="M") {
                    return Constants::maleColor;
                } else if (g=="F") {
                    return Constants::femaleColor;
                } else if (g=="H") {
                    return Constants::hermaColor;
                }
            } else if (role==Qt::DecorationRole) {
                const QString &g = QSqlTableModel::data(QSqlTableModel::index(idx.row(), Constants::IDENTITY_GENDER)).toString();
                if (g=="M") {
                    return theme()->icon(Core::Constants::ICONMALE);
                } else if (g=="F") {
                    return theme()->icon(Core::Constants::ICONFEMALE);
                } else if (g=="H") {
                    return theme()->icon(Core::Constants::ICONHERMAPHRODISM);
                }
                return QIcon();
            }
        }

        return QVariant();
    }

private:
    QString m_NameFilter;
};

class PatientBaseValidator : public QValidator
{
public:
    PatientBaseValidator(QObject *parent) :
        QValidator(parent),
        m_Model(0)
    {
    }

    QValidator::State validate(QString &string, int &pos) const
    {
        // Filter the model with the string
        m_Model->setNameFilter(QStringList() << string << "" << "" << "");
        return QValidator::Acceptable;
    }

    void fixup(QString &) const {}

    void setModelToFilter(PatientCompleterModel *model) {m_Model=model;}

private:
    PatientCompleterModel *m_Model;
};
}


namespace Patients {
namespace Internal {
class PatientBaseCompleterPrivate
{
public:
    PatientBaseCompleterPrivate() : m_Model(0), m_Validator(0) {}

    PatientCompleterModel *m_Model;
    PatientBaseValidator *m_Validator;
};
}
}

/** \todo Find a way to filter the model before the completer starts its processing on the model. --> use a qvalidator ?? */
PatientBaseCompleter::PatientBaseCompleter(QObject *parent) :
    QCompleter(parent),
    d(new Internal::PatientBaseCompleterPrivate)
{
    d->m_Model = new PatientCompleterModel(this);
    d->m_Validator = new PatientBaseValidator(this);
    d->m_Validator->setModelToFilter(d->m_Model);
    setCaseSensitivity(Qt::CaseInsensitive);
    setModel(d->m_Model);
    setCompletionColumn(PatientCompleterModel::ColumnFullName);
    popup()->setAlternatingRowColors(true);
//    qWarning() << "xxxxxxxxxxxxxxxx Completer created" << model()->columnCount() << model()->rowCount();
}

PatientBaseCompleter::~PatientBaseCompleter()
{
    if (d) delete d;
    d = 0;
}

QValidator *PatientBaseCompleter::validator() const
{
    return static_cast<QValidator*>(d->m_Validator);
}

