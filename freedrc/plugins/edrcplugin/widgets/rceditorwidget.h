/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_EDRC_EDITOR_WIDGET_H
#define EDRC_EDRC_EDITOR_WIDGET_H

// FOR TEST ONLY
#include <edrcplugin/edrc_exporter.h>
// END

#include <QWidget>
#include <QDialog>
QT_BEGIN_NAMESPACE
class QModelIndex;
class QItemSelection;
QT_END_NAMESPACE

/**
 * \file rceditorwidget.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 18 June 2013
*/

namespace eDRC {
namespace Internal {
class RcEditorWidgetPrivate;

class EDRC_EXPORT RcEditorWidget: public QWidget
{
    Q_OBJECT
public:
      RcEditorWidget(QWidget *parent = 0);
      ~RcEditorWidget();

private Q_SLOTS:
      void onCurrentRcChanged(const QModelIndex &current, const QModelIndex &previous);
      void onSearchTextChanged(const QString &text);
      void onCriteriaItemPressed(const QModelIndex &index);
      void onSmfgAboutClicked();
      void onArgumentsClicked();

private:
      RcEditorWidgetPrivate *d;
};

} // namespace eDRC
} // namespace Internal

#endif  // EDRC_EDRC_EDITOR_WIDGET_H
