/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_EDRC_EDITOR_WIDGET_H
#define EDRC_EDRC_EDITOR_WIDGET_H

#include <QWidget>
#include <QDialog>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace eDRC {
namespace Internal {
class RcEditorWidgetPrivate;

class RcEditorWidget: public QWidget
{
    Q_OBJECT
public:
      RcEditorWidget(QWidget *parent = 0);
      ~RcEditorWidget();

private Q_SLOTS:
      void onCurrentRcChanged(const QModelIndex &current, const QModelIndex &previous);
      void onSearchTextChanged(const QString &text);
      void onSmfgAboutClicked();
      void onArgumentsClicked();

private:
      RcEditorWidgetPrivate *d;
};

} // namespace eDRC
} // namespace Internal

#endif  // EDRC_EDRC_EDITOR_WIDGET_H
