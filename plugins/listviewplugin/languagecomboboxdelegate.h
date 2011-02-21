#ifndef LANGUAGECOMBOBOXDELEGATE_H
#define LANGUAGECOMBOBOXDELEGATE_H

#include <listviewplugin/listview_exporter.h>
#include <QItemDelegate>


namespace Views {
namespace Internal {
class LangageComboBoxDelegatePrivate;
}

class LISTVIEW_EXPORT LanguageComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum DisplayMode {
        AllLanguages = 0,
        AvailableTranslations
    };

    explicit LanguageComboBoxDelegate(QObject *parent = 0, DisplayMode mode = AvailableTranslations);
    ~LanguageComboBoxDelegate();

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

}  // End namespace View


#endif // LANGUAGECOMBOBOXDELEGATE_H
