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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Patients::PatientLineEditCompleterSearch
 * LineEdit with a QCompleter that allow to select existing patients using the
 * Patient database content.
*/

#include "patientsearchedit.h"
#include "patientbase.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QAbstractItemView>
#include <QToolButton>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace {

class PatientCompleterModel : public QSqlQueryModel
{
public:
    enum { ColumnFullName = 0 , Uid};

    enum FilterListIndex {
        NameIndex = 0,
        FirstNameIndex,
        SecondNameIndex
    };

    PatientCompleterModel(QObject *parent) : QSqlQueryModel(parent)
    {
    }

    // See FilterListIndex for the content of the filter
    void setNameFilter(const QStringList &names)
    {
        QHash<int, QString> where;
        where.insert(Constants::IDENTITY_ISACTIVE, "=1");
        if (!names.at(NameIndex).isEmpty())
            where.insert(Constants::IDENTITY_USUALNAME, QString("like '%1%'").arg(names.at(NameIndex)));
        if (!names.at(FirstNameIndex).isEmpty())
            where.insert(Constants::IDENTITY_FIRSTNAME, QString("like '%1%'").arg(names.at(FirstNameIndex)));
        if (!names.at(SecondNameIndex).isEmpty())
            where.insert(Constants::IDENTITY_OTHERNAMES, QString("like '%1%'").arg(names.at(SecondNameIndex)));
        QString req = patientBase()->select(Constants::Table_IDENT,
                                            QList<int>()
                                            << Constants::IDENTITY_ISACTIVE
                                            << Constants::IDENTITY_USUALNAME
                                            << Constants::IDENTITY_FIRSTNAME
                                            << Constants::IDENTITY_OTHERNAMES
                                            << Constants::IDENTITY_GENDER
                                            << Constants::IDENTITY_DOB
                                            << Constants::IDENTITY_UID,
                                            where
                                            );
        req += QString("\n  ORDER BY `%1` ASC").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_USUALNAME));
        req += "\n  LIMIT 20";
        setQuery(req, patientBase()->database());
    }

    QVariant data(const QModelIndex &idx, int role) const
    {
        if (!idx.isValid())
            return QVariant();

        if (idx.column()==ColumnFullName) {
            if (role==Qt::DisplayRole || role==Qt::EditRole || role==Qt::ToolTipRole) {
                QString name = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 1, idx.parent())).toString();
                QString firstName = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 2, idx.parent())).toString();
                QString secName = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 3, idx.parent())).toString();
                QString r = QString("%1 %2 %3").arg(name).arg(firstName).arg(secName).simplified();
                return r;
            } else if (role==Qt::BackgroundRole) {
                const QString &g = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 4)).toString();
                if (g=="M") {
                    return Constants::maleColor;
                } else if (g=="F") {
                    return Constants::femaleColor;
                } else if (g=="H") {
                    return Constants::otherColor;
                } else if (g=="K") {
                    return Constants::unknownColor;
                } else {
                    return Constants::unknownColor;
                }
            } else if (role==Qt::DecorationRole) {
                const QString &g = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 4)).toString();
                if (g=="M") {
                    return theme()->icon(Core::Constants::ICONMALE);
                } else if (g=="F") {
                    return theme()->icon(Core::Constants::ICONFEMALE);
                } else if (g=="H") {
                    return theme()->icon(Core::Constants::ICONOTHERGENDER);
                } else {
                    return theme()->icon(Core::Constants::ICONGENDERUNKNOWN);
                }
            }
        } else if (idx.column()==Uid && role==Qt::DisplayRole) {
            return QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 6)).toString();
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
        Q_UNUSED(pos);
        // only one char difference for the sql filter
        if ((string.size() - m_LastString.size()) > 1) {
            return QValidator::Acceptable;
        }
        m_LastString = string;
        // Filter the model with the string
        // split using various separators

        //FIXME: what if string contains " " AND ":" ?? priority -> order!
        QString separator;
        if (string.contains("/"))
            separator = "/";
        if (string.contains(":"))
            separator = ":";
        if (string.contains(","))
            separator = ",";
        if (string.contains(";"))
            separator = ";";
        if (string.contains(" "))
            separator = " ";

        QString sql = string;
        sql = sql.replace("*", "%");

        if (!separator.isEmpty()) {
            QStringList list = sql.split(separator, QString::KeepEmptyParts);
            for(int i = 0; list.count()-4; ++i) {
                list << "";
            }
            m_Model->setNameFilter(list);
            return QValidator::Acceptable;
        }
        m_Model->setNameFilter(QStringList() << sql << "" << "" << "");
        return QValidator::Acceptable;
    }

    void fixup(QString &) const {}

    void setModelToFilter(PatientCompleterModel *model) {m_Model=model;}

private:
    PatientCompleterModel *m_Model;
    mutable QString m_LastString;
};

}  // namespace Anonymous

namespace Patients {
namespace Internal {
class PatientBaseCompleterPrivate
{
public:
    PatientBaseCompleterPrivate() :
        m_Model(0), m_Validator(0)
    {}

    ~PatientBaseCompleterPrivate()
    {
        if (m_Model) {
            delete m_Model;
            m_Model = 0;
        }
        if (m_Validator) {
            delete m_Validator;
            m_Validator = 0;
        }
    }

    PatientCompleterModel *m_Model;
    PatientBaseValidator *m_Validator;
};
}
}

// TODO: Find a way to filter the model before the completer starts its processing on the model. --> use a qvalidator?
PatientBaseCompleter::PatientBaseCompleter(QObject *parent) :
    QCompleter(parent),
    d(new Internal::PatientBaseCompleterPrivate)
{
    d->m_Model = new PatientCompleterModel(this);
    d->m_Validator = new PatientBaseValidator(this);
    d->m_Validator->setModelToFilter(d->m_Model);
    setModel(d->m_Model);
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionColumn(PatientCompleterModel::ColumnFullName);
    setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    popup()->setAlternatingRowColors(true);
}

PatientBaseCompleter::~PatientBaseCompleter()
{
    if (d)
        delete d;
    d = 0;
}

QValidator *PatientBaseCompleter::validator() const
{
    return static_cast<QValidator*>(d->m_Validator);
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////   LINEEDIT   /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PatientSearchEdit::PatientSearchEdit(QWidget *parent) :
    Utils::QButtonLineEdit(parent),
    m_Completer(0)
{
    // Add the cancel button on the right
    QToolButton *cancel = new QToolButton;
    cancel->setIcon(theme()->icon(Core::Constants::ICONCLEARLINEEDIT));
    cancel->setToolTip(tkTr(Trans::Constants::CLEAR));
    setRightButton(cancel);

    // Add the completer
    m_Completer = new PatientBaseCompleter;
    setCompleter(m_Completer);
    setValidator(m_Completer->validator());

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_Completer, SIGNAL(activated(QModelIndex)), this, SLOT(onPatientSelected(QModelIndex)));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelSearch()));
}

PatientSearchEdit::~PatientSearchEdit()
{
}

void PatientSearchEdit::onTextChanged(const QString &newText)
{
    int diff = newText.size() - m_LastSearch.size();
    if (diff > 1 || diff < -1) {
        // more than one char difference?
        // do not change lastsearch
        return;
    }
    m_LastSearch = newText;
    m_Completer->validator()->validate(m_LastSearch, diff);
    m_Completer->setCompletionPrefix(m_LastSearch);
}

void PatientSearchEdit::cancelSearch()
{
    clear();
    QRect cr = rect();
    m_Completer->complete(cr); // popup it up!
}

void PatientSearchEdit::onPatientSelected(const QModelIndex &index)
{
    QString uid = m_Completer->model()->index(index.row(), PatientBaseCompleter::Uid, index.parent()).data().toString();
    Q_EMIT patientSelected(index.data().toString(), uid);
}

void PatientSearchEdit::focusInEvent(QFocusEvent *event)
{
    // connect with Qt::UniqueConnection: if connection exists already, don't connect another time
    connect(m_Completer, SIGNAL(activated(QModelIndex)), this, SLOT(onPatientSelected(QModelIndex)), Qt::UniqueConnection);
    Utils::QButtonLineEdit::focusInEvent(event);
}

//void PatientLineEditCompleterSearch::keyPressEvent(QKeyEvent *event)
//{
//    if (m_Completer && m_Completer->popup()->isVisible()) {
//        // The following keys are forwarded by the completer to the widget
//       switch (event->key()) {
//       case Qt::Key_Enter:
//       case Qt::Key_Return:
//       case Qt::Key_Escape:
//       case Qt::Key_Tab:
//       case Qt::Key_Backtab:
//            event->ignore();
//            return; // let the completer do default behavior
//       default:
//           break;
//       }
//    }

//    bool hasModifier = (event->modifiers() != Qt::NoModifier);
//    if (hasModifier) {
//        m_Completer->popup()->hide();
//        return;
//    }
//}

