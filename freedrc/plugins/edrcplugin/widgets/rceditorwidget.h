/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_PLUGIN_RC_EDITOR_WIDGET_H
#define EDRC_PLUGIN_RC_EDITOR_WIDGET_H

// FOR TEST ONLY
#include <edrcplugin/edrc_exporter.h>
// END
#include <edrcplugin/consultresult.h>

#include <QWidget>
#include <QDialog>
QT_BEGIN_NAMESPACE
class QModelIndex;
class QItemSelection;
QT_END_NAMESPACE

/**
 * \file rceditorwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 24 June 2013
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
    void clear();
    void setConsultResult(const ConsultResult &cr);
    int currentEditingConsultResultId() const;

public Q_SLOTS:
    ConsultResult submit();

Q_SIGNALS:
    void currentConsultResultIdChanged(int currentId);

private Q_SLOTS:
    void onCurrentRcChanged(const QModelIndex &current, const QModelIndex &previous);
    void onSearchTextChanged(const QString &text);
    //void onCriteriaSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onCriteriaItemPressed(const QModelIndex &index);
    void updateCodingStatus();
    void toggleValidator();

    void on_debugButton_clicked();
private:
    void changeEvent(QEvent *event);

private:
    RcEditorWidgetPrivate *d;
};

} // namespace eDRC
} // namespace Internal

#endif  // EDRC_PLUGIN_RC_EDITOR_WIDGET_H
