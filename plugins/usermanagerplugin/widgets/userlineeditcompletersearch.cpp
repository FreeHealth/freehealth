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
  \class UserPlugin::UserLineEditCompleterSearch
  LineEdit with a QCompleter that allow to select existing users.
*/

#include "userlineeditcompletersearch.h"
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QAbstractItemView>
#include <QToolButton>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

namespace {

class UserCompleterModel : public QSqlQueryModel
{
public:
    enum { ColumnFullName = 0 , Uid};

    enum FilterListIndex {
        NameIndex = 0,
        FirstNameIndex,
        SecondNameIndex
    };

    UserCompleterModel(QObject *parent) : QSqlQueryModel(parent)
    {
    }

    // See FilterListIndex for the content of the filter
    void setNameFilter(const QStringList &names)
    {
        QHash<int, QString> where;
        where.insert(Constants::USER_VALIDITY, "=1");
        if (!names.at(NameIndex).isEmpty())
            where.insert(Constants::USER_USUALNAME, QString("like '%1%'").arg(names.at(NameIndex)));
        if (!names.at(FirstNameIndex).isEmpty())
            where.insert(Constants::USER_FIRSTNAME, QString("like '%1%'").arg(names.at(FirstNameIndex)));
        if (!names.at(SecondNameIndex).isEmpty())
            where.insert(Constants::USER_OTHERNAMES, QString("like '%1%'").arg(names.at(SecondNameIndex)));
        QString req = userBase()->select(Constants::Table_USERS,
                                            QList<int>()
                                            << Constants::USER_USUALNAME
                                            << Constants::USER_FIRSTNAME
                                            << Constants::USER_OTHERNAMES
                                            << Constants::USER_UUID,
                                            where
                                            );
        req += QString("\n  ORDER BY `%1` ASC").arg(userBase()->fieldName(Constants::Table_USERS, Constants::USER_USUALNAME));
        req += "\n  LIMIT 20";
        setQuery(req, userBase()->database());
    }

    QVariant data(const QModelIndex &idx, int role) const
    {
        if (!idx.isValid())
            return QVariant();

        if (idx.column()==ColumnFullName) {
            if (role==Qt::DisplayRole || role==Qt::EditRole || role==Qt::ToolTipRole) {
                QString name = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 0, idx.parent())).toString();
                QString firstName = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 1, idx.parent())).toString();
                QString secName = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 2, idx.parent())).toString();
                QString r = QString("%1 %2 %3").arg(name).arg(firstName).arg(secName).simplified();
                return r;
            }
//            else if (role==Qt::BackgroundRole) {
//                const QString &g = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 4)).toString();
//                if (g=="M") {
//                    return Constants::maleColor;
//                } else if (g=="F") {
//                    return Constants::femaleColor;
//                } else if (g=="H") {
//                    return Constants::otherColor;
//                }
//            } else if (role==Qt::DecorationRole) {
//                const QString &g = QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 4)).toString();
//                if (g=="M") {
//                    return theme()->icon(Core::Constants::ICONMALE);
//                } else if (g=="F") {
//                    return theme()->icon(Core::Constants::ICONFEMALE);
//                } else if (g=="H") {
//                    return theme()->icon(Core::Constants::ICONOTHERGENDER);
//                } else if (g=="K") {
//                    return theme()->icon(Core::Constants::ICONGENDERUNKNOWN);
//                }
//                return QIcon();
//            }
        } else if (idx.column()==Uid && role==Qt::DisplayRole) {
            return QSqlQueryModel::data(QSqlQueryModel::index(idx.row(), 3)).toString();
        }


        return QVariant();
    }

private:
    QString m_NameFilter;
};

class UserValidator : public QValidator
{
public:
    UserValidator(QObject *parent) :
        QValidator(parent),
        m_Model(0)
    {
    }

    QValidator::State validate(QString &string, int &) const
    {
        // only one char difference for the sql filter
        if ((string.size() - m_LastString.size()) > 1) {
            return QValidator::Acceptable;
        }
        m_LastString = string;
        // Filter the model with the string
        // split using various separators
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

    void setModelToFilter(UserCompleterModel *model) {m_Model=model;}

private:
    UserCompleterModel *m_Model;
    mutable QString m_LastString;
};
}


namespace UserPlugin {
namespace Internal {
class UserCompleterPrivate
{
public:
    UserCompleterPrivate() : m_Model(0), m_Validator(0) {}
    ~UserCompleterPrivate()
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

    UserCompleterModel *m_Model;
    UserValidator *m_Validator;
};
}
}

// TODO: Find a way to filter the model before the completer starts its processing on the model -> use a QValidator?
UserCompleter::UserCompleter(QObject *parent) :
    QCompleter(parent),
    d(new Internal::UserCompleterPrivate)
{
    d->m_Model = new UserCompleterModel(this);
    d->m_Validator = new UserValidator(this);
    d->m_Validator->setModelToFilter(d->m_Model);
    setModel(d->m_Model);
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionColumn(UserCompleterModel::ColumnFullName);
    setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    popup()->setAlternatingRowColors(true);
}

UserCompleter::~UserCompleter()
{
    if (d) delete d;
    d = 0;
}

QValidator *UserCompleter::validator() const
{
    return static_cast<QValidator*>(d->m_Validator);
}

UserLineEditCompleterSearch::UserLineEditCompleterSearch(QWidget *parent) :
    Utils::QButtonLineEdit(parent),
    m_Completer(0)
{
    // Add the cancel button on the right
    QToolButton *cancel = new QToolButton;
    cancel->setIcon(theme()->icon(Core::Constants::ICONCLEARLINEEDIT));
    cancel->setToolTip(tkTr(Trans::Constants::CLEAR));
    setRightButton(cancel);
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelSearch()));

    // Add the completer
    m_Completer = new UserCompleter;
    setCompleter(m_Completer);
    setValidator(m_Completer->validator());

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(m_Completer, SIGNAL(activated(QModelIndex)), this, SLOT(userSelected(QModelIndex)));
}

UserLineEditCompleterSearch::~UserLineEditCompleterSearch()
{
}

void UserLineEditCompleterSearch::textChanged(const QString &newText)
{
    int diff = newText.size() - m_LastSearch.size();
    if (diff > 1 || diff < -1) {
        // more than one char difference ?
        // do not change lastsearch
        return;
    }
    m_LastSearch = newText;
    m_Completer->validator()->validate(m_LastSearch, diff);
    m_Completer->setCompletionPrefix(m_LastSearch);
}

void UserLineEditCompleterSearch::cancelSearch()
{
    setText("");
    QRect cr = rect();
    m_Completer->complete(cr); // popup it up!
}

void UserLineEditCompleterSearch::userSelected(const QModelIndex &index)
{
    QString uid = m_Completer->model()->index(index.row(), UserCompleter::Uid, index.parent()).data().toString();
    qWarning() << index.data().toString() << uid;
    Q_EMIT selectedUser(index.data().toString(), uid);
}

//void UserLineEditCompleterSearch::keyPressEvent(QKeyEvent *event)
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

