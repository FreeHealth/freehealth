#include "texteditor.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStyle>

#include <QDebug>

TextEditor::TextEditor(QWidget *parent) :
    QWidget(parent)
{
    _edit = new QTextEdit(this);
    qWarning() << _edit->testAttribute(Qt::WA_NoMousePropagation);
    _edit->setAttribute(Qt::WA_NoMousePropagation, false);
    _bar = new QToolBar(this);
    _edit->viewport()->installEventFilter(this);
}

void TextEditor::resizeEvent(QResizeEvent *event)
{
    qWarning() << "RESIZE" << event->oldSize() << event->size();
    QWidget::resizeEvent(event);
}

void TextEditor::paintEvent(QPaintEvent *event)
{
    qWarning() << "PAINT" << event;
    QWidget::paintEvent(event);
}

void TextEditor::mousePressEvent(QMouseEvent *event)
{
    qWarning() << "PRESS" << event->pos();
    QWidget::mousePressEvent(event);
}

bool TextEditor::eventFilter(QObject *object, QEvent *event)
{
    qWarning() << "FILTER" << object << event->type();
    return false;
}
