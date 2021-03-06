/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_PLUGIN_CR_EDITOR_WIDGET_H
#define EDRC_PLUGIN_CR_EDITOR_WIDGET_H

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
 * \file creditorwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 05 Oct 2013
*/

namespace eDRC {
namespace Internal {
class CrEditorWidgetPrivate;

class EDRC_EXPORT CrEditorWidget: public QWidget
{
    Q_OBJECT
public:
    CrEditorWidget(QWidget *parent = 0);
    ~CrEditorWidget();
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
    CrEditorWidgetPrivate *d;
};

} // namespace eDRC
} // namespace Internal

#endif  // EDRC_PLUGIN_CR_EDITOR_WIDGET_H
