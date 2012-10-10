#include "webcampreferenceswidget.h"
#include "ui_webcampreferenceswidget.h"

WebcamPreferencesWidget::WebcamPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebcamPreferencesWidget)
{
    ui->setupUi(this);
}

WebcamPreferencesWidget::~WebcamPreferencesWidget()
{
    delete ui;
}
