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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/***************************************************************************
 *   Code adapted from the Qxt Librairy (LGPL 2.1)                         *
 ***************************************************************************/
#include "languagecombobox.h"

#include <translationutils/constants.h>

#include <QDir>
#include <QApplication>

#include <QDebug>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
class Language;
typedef QList<Language> LanguageList;

static QStringList findQmFiles(const QString &pathToTranslations)
{
    QDir dir(pathToTranslations);
    QStringList fileNames = dir.entryList(QStringList("qt*.qm"), QDir::Files, QDir::Name);
    QMutableStringListIterator i(fileNames);
    while (i.hasNext()) {
        i.next();
        int start = i.value().indexOf('_');
        int end = i.value().lastIndexOf('.');
        QString s = i.value().mid(start + 1, end - start - 1).toLower();
        i.setValue(s);
    }
    return fileNames;
}


class Language
{
public:
    Language(const QLocale::Language &language) :
            m_Language(language),
            m_CountryCode("C")
    {
        if (language==QLocale::C) {
            m_DisplayName = tkTr(Trans::Constants::ALL_LANGUAGE_TEXT);
            m_CountryCode = tkTr(Trans::Constants::ALL_LANGUAGE).toUpper();
        } else {
            QLocale loc(language);
            if (loc.language() == language)
                m_CountryCode = loc.name().right(2);
            else
                m_CountryCode = "";
            m_DisplayName = qApp->translate("QLocale", qPrintable(QLocale::languageToString(m_Language)));
        }
    }

    bool operator<(const Language &lang) const
    {
        return m_DisplayName.localeAwareCompare(lang.m_DisplayName) < 0;
    }

    static const LanguageList &getAllLanguages()
    {
        if (m_AllLanguages.empty()) {
            for (int idx = 0; idx < QLocale::LastLanguage; ++idx) {
                QLocale::Language l = static_cast<QLocale::Language>(idx);
                if (l == QLocale::LastLanguage)
                    continue;
                // obsolete - NorwegianNynorsk is used instead
                if (l == QLocale::Nynorsk)
                    continue;
                if (l == QLocale::C)
                    continue;
                m_AllLanguages.push_back(Language(l));
            }
            qSort(m_AllLanguages);
            m_AllLanguages.prepend(Language(QLocale::C));
        }
        return m_AllLanguages;
    }

    static LanguageList getTrLanguages(const QString &absPath)
    {
        LanguageList trLanguages;
        QStringList qms = findQmFiles(absPath);
        for (int i = 0; i < qms.size(); ++i)
        {
            QLocale locale(qms[i]);
            if (locale.language() == QLocale::C)
                continue;
            trLanguages.push_back(Language(locale.language()));
        }
        trLanguages.append(QLocale::English);
        qSort(trLanguages);
        trLanguages.prepend(Language(QLocale::C));
        return trLanguages;
    }

public:
    QLocale::Language m_Language;
    QString m_DisplayName;
    QString m_CountryCode;
    static LanguageList m_AllLanguages;
};

LanguageList Language::m_AllLanguages;

class LanguageModel : public QAbstractTableModel
{
public:
    LanguageModel(const LanguageList& languages, const QString &iconPath, QObject* parent = 0)
            : QAbstractTableModel(parent), m_Languages(languages), m_IconPath(iconPath)
    {
    }

    int rowCount(const QModelIndex&) const
    {
        return m_Languages.size();
    }

    int columnCount(const QModelIndex&) const
    {
        return 2;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        if (!index.isValid())
            return QVariant();
        if (m_Languages.empty())
            return QVariant();

        int idx = qMin(index.row(), m_Languages.size());
        const Language &l = m_Languages[idx];
        if (role == Qt::DecorationRole) {
            QIcon icon(m_IconPath + "/flags/" + l.m_CountryCode + ".png");
            return icon;
        }
        if (role == Qt::DisplayRole) {
            switch (index.column())
            {
            case 0:
                return l.m_DisplayName;
            case 1:
                return l.m_Language;
            default:
                return QVariant();
            }
        }
        return QVariant();
    }
private:
    LanguageList m_Languages;
    QString m_IconPath;
};

}  // End anonymous namespace

namespace Utils {
namespace Internal {

class LanguageComboBoxPrivate
{
public:
    LanguageComboBoxPrivate(LanguageComboBox *parent) :
            m_DisplayMode(LanguageComboBox::AllLanguages), m_Model(0),
            q(parent)
    {
    }

    void setDisplayMode(LanguageComboBox::DisplayMode mode)
    {
        if (m_DisplayMode == mode && m_Model)
            return;
        m_DisplayMode = mode;
        reset();
    }

    void reset()
    {
        if (m_IconPath.isEmpty() || m_TrPath.isEmpty())
            return;

        if (m_Model) {
            delete m_Model;
            m_Model = 0;
        }

        QLocale::Language currentLang = q->currentLanguage();
        if (m_DisplayMode == LanguageComboBox::AllLanguages)
            m_Model = new LanguageModel(Language::getAllLanguages(), m_IconPath, q);
        else
            m_Model = new LanguageModel(Language::getTrLanguages(m_TrPath), m_IconPath, q);

        q->setModel(m_Model);
        q->setModelColumn(0);

        q->setCurrentLanguage(currentLang);
    }

public:
    LanguageComboBox::DisplayMode m_DisplayMode;
    QAbstractTableModel* m_Model;
    QString m_TrPath, m_IconPath;

private:
    LanguageComboBox *q;
};

}  // End namespace Internal
}  // End namespace Utils



LanguageComboBox::LanguageComboBox(QWidget* parent) :
        QComboBox(parent), d(new LanguageComboBoxPrivate(this))
{
    setDisplayMode(AllLanguages);
    setCurrentLanguage(QLocale::system().language());
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxCurrentIndexChanged(int)));
}

LanguageComboBox::~LanguageComboBox()
{
    if (d) {
        delete d;
    }
    d = 0;
}

void LanguageComboBox::setTranslationsPath(const QString &absFullPath)
{
    d->m_TrPath = absFullPath;
    d->reset();
}

void LanguageComboBox::setFlagsIconPath(const QString &absFullPath)
{
    d->m_IconPath = QDir::cleanPath(absFullPath);
    d->reset();
}

/** \brief Return the current selected language.*/
QLocale::Language LanguageComboBox::currentLanguage() const
{
    if (!d->m_Model)
        return QLocale::C;

    QModelIndex idx = d->m_Model->index(currentIndex(), 1);
    QLocale::Language currentLang = static_cast<QLocale::Language>(idx.data().toInt());
    return currentLang;
}

/** \brief Return the name of the current selected language.*/
QString LanguageComboBox::currentLanguageName() const
{
    return currentText();
}

void LanguageComboBox::setCurrentIsoLanguage(const QString &languageIsoCode)
{
    QLocale::Language language = QLocale(languageIsoCode).language();
    setCurrentLanguage(language);
}

void LanguageComboBox::setCurrentLanguage(QLocale::Language language)
{
    if (!d->m_Model)
        return;
    // column 1 is QLocale::Language
    QModelIndex start = d->m_Model->index(0, 1);
    QModelIndexList result = d->m_Model->match(start, Qt::DisplayRole, language, 1, Qt::MatchExactly);
    if (!result.isEmpty())
        setCurrentIndex(result.first().row());
    comboBoxCurrentIndexChanged(0);
}

/** \brief the display mode of the widget. */
void LanguageComboBox::setDisplayMode(DisplayMode mode)
{
    d->setDisplayMode(mode);
}

LanguageComboBox::DisplayMode LanguageComboBox::displayMode() const
{
    return d->m_DisplayMode;
}

void LanguageComboBox::comboBoxCurrentIndexChanged(int)
{
    Q_EMIT currentLanguageChanged(currentLanguage());
    Q_EMIT currentLanguageNameChanged(currentLanguageName());
}

