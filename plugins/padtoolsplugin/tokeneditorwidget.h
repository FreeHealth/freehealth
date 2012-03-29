#ifndef PADTOOLS_TOKENEDITORWIDGET_H
#define PADTOOLS_TOKENEDITORWIDGET_H

#include <QWidget>

namespace PadTools {

namespace Ui {
class TokenEditorWidget;
}

class TokenEditorWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TokenEditorWidget(QWidget *parent = 0);
    ~TokenEditorWidget();
    
private:
    Ui::TokenEditorWidget *ui;
};


} // namespace PadTools
#endif // PADTOOLS_TOKENEDITORWIDGET_H
