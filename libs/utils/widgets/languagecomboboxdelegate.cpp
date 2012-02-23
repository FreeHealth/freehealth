#include "languagecomboboxdelegate.h"
#include "languagecombobox.h"

#include <QDebug>

using namespace Utils;
using namespace Internal;

namespace Utils {
namespace Internal {
class LangageComboBoxDelegatePrivate
{
public:
    LangageComboBoxDelegatePrivate() {}
    ~LangageComboBoxDelegatePrivate() {}

public:
    int m_DisplayMode;
    QString m_TrPath, m_IconPath;
};
}
}

LanguageComboBoxDelegate::LanguageComboBoxDelegate(QObject *parent, DisplayMode mode) :
        QItemDelegate(parent), d(new Internal::LangageComboBoxDelegatePrivate)
{
    d->m_DisplayMode = mode;
}

LanguageComboBoxDelegate::~LanguageComboBoxDelegate()
{
    if (d)
        delete d;
    d = 0;
}

void LanguageComboBoxDelegate::setTranslationsPath(const QString &absFullPath)
{
    d->m_TrPath = absFullPath;
}

void LanguageComboBoxDelegate::setFlagsIconPath(const QString &absFullPath)
{
    d->m_IconPath = absFullPath;
}

QWidget *LanguageComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    Q_UNUSED(option);
    LanguageComboBox *editor = new LanguageComboBox(parent);
    editor->setTranslationsPath(d->m_TrPath);
    editor->setFlagsIconPath(d->m_IconPath);
    if (d->m_DisplayMode == AllLanguages)
        editor->setDisplayMode(LanguageComboBox::AllLanguages);
    else
        editor->setDisplayMode(LanguageComboBox::AvailableTranslations);
    editor->setCurrentLanguage(static_cast<QLocale::Language>(index.data(Qt::EditRole).toInt()));
    return editor;
}

void LanguageComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    LanguageComboBox *combo = qobject_cast<LanguageComboBox*>(editor);
    if (combo) {
        combo->setCurrentLanguage(static_cast<QLocale::Language>(index.data(Qt::EditRole).toInt()));
    }
}

void LanguageComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    LanguageComboBox *combo = qobject_cast<LanguageComboBox*>(editor);
    if (combo) {
        model->setData(index, combo->currentLanguage(), Qt::EditRole);
    }
}

void LanguageComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

