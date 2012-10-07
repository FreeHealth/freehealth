#ifndef UTILS_LANGUAGECOMBOBOXDELEGATE_H
#define UTILS_LANGUAGECOMBOBOXDELEGATE_H

#include <utils/global_exporter.h>
#include <QItemDelegate>

namespace Utils {
namespace Internal {
class LangageComboBoxDelegatePrivate;
}

class UTILS_EXPORT LanguageComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum DisplayMode {
        AllLanguages = 0,
        AvailableTranslations
    };

    explicit LanguageComboBoxDelegate(QObject *parent = 0, DisplayMode mode = AvailableTranslations);
    ~LanguageComboBoxDelegate();

    void setTranslationsPath(const QString &absFullPath);
    void setFlagsIconPath(const QString &absFullPath);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    Internal::LangageComboBoxDelegatePrivate *d;
};

}  // End namespace Utils


#endif // UTILS_LANGUAGECOMBOBOXDELEGATE_H
