#include "tokeneditorwidget.h"
#include "ui_tokeneditorwidget.h"

namespace PadTools {

TokenEditorWidget::TokenEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TokenEditorWidget)
{
    ui->setupUi(this);
}

TokenEditorWidget::~TokenEditorWidget()
{
    delete ui;
}

} // namespace PadTools
